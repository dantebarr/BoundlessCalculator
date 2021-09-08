/* -------------------------------------------------------------------------- *
 *                                   Lepton                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the Lepton expression parser originating from              *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2009-2013 Stanford University and the Authors.      *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

#include "lepton/Parser.h"
#include "lepton/CustomFunction.h"
#include "lepton/Exception.h"
#include "lepton/ExpressionTreeNode.h"
#include "lepton/Operation.h"
#include "lepton/ParsedExpression.h"
#include "DisabledFeatureException.h"
#include <cctype>
#include <iostream>
#include <set>
#include <complex>

using namespace Lepton;
using namespace std;

static const string Digits = "0123456789";
static const string Operators = "+-*/^";
static const bool LeftAssociative[] = {true, true, true, true, false};
static const int Precedence[] = {0, 0, 1, 1, 3};
static const Operation::Id OperationId[] = {Operation::ADD, Operation::SUBTRACT, Operation::MULTIPLY, Operation::DIVIDE, Operation::POWER};
static set<string> disabledFeatures;
static bool isDegree = false;

class Lepton::ParseToken {
public:
    enum Type {Number, Matrix, Operator, Variable, Function, LeftParen, RightParen, Comma, Whitespace, Complex};

    ParseToken(string text, Type type) : text(text), type(type) {
    }
    const string& getText() const {
        return text;
    }
    Type getType() const {
        return type;
    }
private:
    string text;
    Type type;
};

bool Parser::checkFeatureDisabled(const string featureName){
   return disabledFeatures.find(featureName) != disabledFeatures.end();
}

void Parser::addFeatureToDisable(string featureName) {
    disabledFeatures.insert(featureName);
}

void Parser::setAngleUnit(bool degreeSet) {
    isDegree = degreeSet;
}

bool Parser::getAngleUnit() {
    return isDegree;
}

void Parser::clearFeaturesToDisable() {
    disabledFeatures.clear();
}

string Parser::trim(const string& expression) {
    // Remove leading and trailing spaces.
    
    int start, end;
    for (start = 0; start < (int) expression.size() && isspace(expression[start]); start++)
        ;
    for (end = (int) expression.size()-1; end > start && isspace(expression[end]); end--)
        ;
    if (start == end && isspace(expression[end]))
        return "";
    return expression.substr(start, end-start+1);
}

// returns a vector of tokens for a string that has real numbers, decimal points, and imaginary numbers
std::vector<ParseToken> Parser::parseRealAndImaginary(string numberString) {
    vector<ParseToken> tokens;
    int startIndex = 0;
    char c;
    for (int pos = 0; pos < numberString.size(); pos++) {
        c = numberString[pos];
        if (c == 'i' || pos == numberString.size() - 1) {
            // insert a multiplication operation if there was a previous token already seen
            if (tokens.size() > 0) {
                tokens.push_back(ParseToken("*", ParseToken::Operator));
            }
            // an imaginary number
            if (c == 'i') {
                // real number preceding "i" 
                if (pos - startIndex > 0) {
                    tokens.push_back(ParseToken(numberString.substr(startIndex, pos), ParseToken::Number));
                    tokens.push_back(ParseToken("*", ParseToken::Operator));
                }                
                tokens.push_back(ParseToken("i", ParseToken::Complex));
                startIndex = pos + 1;
            // last portion of string is real
            } else {
                tokens.push_back(ParseToken(numberString.substr(startIndex, numberString.size() - startIndex), ParseToken::Number));
                startIndex = pos + 1;
            }
        }
    }
    return tokens;
}

std::vector<ParseToken> Parser::parseSingleNumber(string numberString) {
    vector<ParseToken> tokens;
    // return string as single token if no imaginary component found
    if (numberString.find("i") == string::npos) {
        tokens.push_back(ParseToken(numberString, ParseToken::Number));
        return tokens;
    }

    // Split string into two strings which contain numerical values before and after the scientific notation
    string nonScientificNotation = "";
    string scientificNotation = "";
    if (numberString.find("e") != string::npos || numberString.find("E") != string::npos) {
        size_t pivot = numberString.find("e") != string::npos ? numberString.find("e") : numberString.find("E");
        nonScientificNotation = numberString.substr(0, pivot);
        scientificNotation = numberString.substr(pivot, numberString.size() - pivot);        
    }
    else {
        nonScientificNotation = numberString;
    }

    // handle numbers preceding scientific notation portion
    tokens = parseRealAndImaginary(nonScientificNotation);
    
    // handle scientific notation portion
    if (scientificNotation.size() == 1) {
        tokens.push_back(ParseToken("*", ParseToken::Operator));
        tokens.push_back(ParseToken("e", ParseToken::Variable));
    // emulate parser's default behavior when there are no numbers after the sign of scientific notation
    } else if (scientificNotation.size() == 2 && Operators.find(scientificNotation[1]) != string::npos) {
        tokens.push_back(ParseToken("*", ParseToken::Operator));
        tokens.push_back(ParseToken("0", ParseToken::Number));
    // handle general case
    } else if (scientificNotation.size() > 1) {
        tokens.push_back(ParseToken("*", ParseToken::Operator));
        tokens.push_back(ParseToken("10", ParseToken::Number));
        tokens.push_back(ParseToken("^", ParseToken::Operator));
        tokens.push_back(ParseToken("(", ParseToken::LeftParen));
        vector<ParseToken> scientificNotationTokens;
        // if e/E notation is followed by +/- symbol
        if (Operators.find(scientificNotation[1]) != string::npos) {
            if (scientificNotation[1] == '-') {
                tokens.push_back(ParseToken("-", ParseToken::Operator));
            }
            scientificNotationTokens = parseRealAndImaginary(scientificNotation.substr(2, scientificNotation.size() - 2));            
        } else {
            scientificNotationTokens = parseRealAndImaginary(scientificNotation.substr(1, scientificNotation.size() - 1));
        }
        tokens.insert(tokens.end(), scientificNotationTokens.begin(), scientificNotationTokens.end());
        tokens.push_back(ParseToken(")", ParseToken::RightParen));
    }
    return tokens;
}

// Caution when modifying start parameter as it is passed by reference
std::vector<ParseToken> Parser::getNextToken(const string& expression, int &start) {
    vector<ParseToken> tokensToReturn;
    char c = expression[start];
    if (c == '(') {
        tokensToReturn.push_back(ParseToken("(", ParseToken::LeftParen));
        start += 1;
        return tokensToReturn;
    }
    if (c == ')') {
        tokensToReturn.push_back(ParseToken(")", ParseToken::RightParen));
        start += 1;
        return tokensToReturn;
    }
    if (c == ',') {
        tokensToReturn.push_back(ParseToken(",", ParseToken::Comma));
        start += 1;
        return tokensToReturn;
    }
    if (Operators.find(c) != string::npos) {
        tokensToReturn.push_back(ParseToken(string(1, c), ParseToken::Operator));
        start += 1;
        return tokensToReturn;
    }
    if (isspace(c)) {
        // White space

        for (int pos = start+1; pos < (int) expression.size(); pos++) {
            if (!isspace(expression[pos])) {
                tokensToReturn.push_back(ParseToken(expression.substr(start, pos-start), ParseToken::Whitespace));
                start += (int) expression.substr(start, pos-start).size();
                return tokensToReturn;
            }
        }
        tokensToReturn.push_back(ParseToken(expression.substr(start, string::npos), ParseToken::Whitespace));
        start += (int) expression.substr(start, string::npos).size();
        return tokensToReturn;
    }
    if (c == '.' || Digits.find(c) != string::npos || c == 'i') {
        // A number
        if (c == 'i' && disabledFeatures.find("Disable complex") != disabledFeatures.end()) {
            throw DisabledFeatureException("Complex numbers");
        }
        
        bool foundDecimal = (c == '.');
        bool foundExp = false;
        int pos;
        for (pos = start+1; pos < (int) expression.size(); pos++) {
            c = expression[pos];
            if (Digits.find(c) != string::npos || c == 'i')
                continue;
            if (c == '.' && !foundDecimal) {
                foundDecimal = true;
                continue;
            }
            if ((c == 'e' || c == 'E') && !foundExp) {
                foundExp = true;
                if (pos < (int) expression.size()-1 && (expression[pos+1] == '-' || expression[pos+1] == '+'))
                    pos++;
                continue;
            }
            break;
        }
        int prevStartVal = start;
        start += (int) expression.substr(prevStartVal, pos - prevStartVal).size();
        return parseSingleNumber(expression.substr(prevStartVal, pos - prevStartVal));
    }

    // A variable, function, or left parenthesis

    for (int pos = start; pos < (int) expression.size(); pos++) {
        c = expression[pos];
        if (c == '(') {
            // Check if function is part of the disabled features set
            std::string functionName = expression.substr(start, pos-start);
            if (disabledFeatures.find(functionName) != disabledFeatures.end()) {
                throw DisabledFeatureException(functionName, "function");
            }
            tokensToReturn.push_back(ParseToken(expression.substr(start, pos-start+1), ParseToken::Function));
            start += (int) expression.substr(start, pos-start+1).size();
            return tokensToReturn;
        }
        if (c == '[') {
            //Matrix -user will not have the ability to enter square brackets unless they input a matrix via the matrix menu 
            if (disabledFeatures.find("[A]") != disabledFeatures.end()) {
                throw DisabledFeatureException("Matrices");
            }
            
            tokensToReturn.push_back(ParseToken(expression.substr(start, 3), ParseToken::Matrix));
            start += (int) expression.substr(start, 3).size();
            return tokensToReturn;
        }
        if (Operators.find(c) != string::npos || c == ',' || c == ')' || isspace(c)) {
            tokensToReturn.push_back(ParseToken(expression.substr(start, pos-start), ParseToken::Variable));
            start += (int) expression.substr(start, pos-start).size();
            return tokensToReturn;
        }
    }
    
    if (disabledFeatures.find("Disable variable") != disabledFeatures.end()) {
        throw DisabledFeatureException("Variables");
    }
    tokensToReturn.push_back(ParseToken(expression.substr(start, string::npos), ParseToken::Variable));
    start += (int) expression.substr(start, string::npos).size();
    return tokensToReturn;
}

vector<ParseToken> Parser::tokenize(const string& expression) {
    vector<ParseToken> tokens;
    vector<ParseToken> tokensReceived;
    // pos is advanced by call to getNextToken rather than incrementing here
    int pos = 0;
    while (pos < (int) expression.size()) {
        std::vector<ParseToken> tokensReceived = getNextToken(expression, pos);
        if (tokensReceived.size() > 1) {
            tokens.insert(tokens.end(), tokensReceived.begin(), tokensReceived.end());
        } else {
            if (tokensReceived[0].getType() != ParseToken::Whitespace) {                
                tokens.push_back(tokensReceived[0]);
            }
        }
    }
    return tokens;
}

ParsedExpression Parser::parse(const string& expression) {
    return parse(expression, map<string, CustomFunction*>());
}

ParsedExpression Parser::parse(const string& expression, const map<string, CustomFunction*>& customFunctions) {
    // First split the expression into subexpressions.

    string primaryExpression = expression;
    vector<string> subexpressions;
    while (true) {
        string::size_type pos = primaryExpression.find_last_of(';');
        if (pos == string::npos)
            break;
        string sub = trim(primaryExpression.substr(pos+1));
        if (sub.size() > 0)
            subexpressions.push_back(sub);
        primaryExpression = primaryExpression.substr(0, pos);
    }

    // Parse the subexpressions.

    map<string, ExpressionTreeNode> subexpDefs;
    for (int i = 0; i < (int) subexpressions.size(); i++) {
        string::size_type equalsPos = subexpressions[i].find('=');
        if (equalsPos == string::npos)
            throw Exception("Parse error: subexpression does not specify a name");
        string name = trim(subexpressions[i].substr(0, equalsPos));
        if (name.size() == 0)
            throw Exception("Parse error: subexpression does not specify a name");
        vector<ParseToken> tokens = tokenize(subexpressions[i].substr(equalsPos+1));
        int pos = 0;
        subexpDefs[name] = parsePrecedence(tokens, pos, customFunctions, subexpDefs, 0);
        if (pos != tokens.size())
            throw Exception("Parse error: unexpected text at end of subexpression: "+tokens[pos].getText());
    }

    // Now parse the primary expression.

    vector<ParseToken> tokens = tokenize(primaryExpression);
    int pos = 0;
    ExpressionTreeNode result = parsePrecedence(tokens, pos, customFunctions, subexpDefs, 0);
    if (pos != tokens.size()) {
        throw Exception("Parse error: unexpected text at end of expression: "+tokens[pos].getText());
    }
    return ParsedExpression(result);
}

ExpressionTreeNode Parser::parsePrecedence(const vector<ParseToken>& tokens, int& pos, const map<string, CustomFunction*>& customFunctions,
            const map<string, ExpressionTreeNode>& subexpressionDefs, int precedence) {
    if (pos == tokens.size())
        throw Exception("Parse error: unexpected end of expression");

    // Parse the next value (number, variable, function, parenthesized expression)

    ParseToken token = tokens[pos];
    ExpressionTreeNode result;
    if (token.getType() == ParseToken::Complex) {
        // The parser currently only return tokens that are unit imaginary numbers
        result = ExpressionTreeNode(new Operation::ComplexNumber(0, 1));
        pos++;
    } else if (token.getType() == ParseToken::Number) {
        double value;
        stringstream(token.getText()) >> value;
        result = ExpressionTreeNode(new Operation::ComplexNumber(value, 0));
        pos++;
    }
    else if (token.getType() == ParseToken::Matrix) {
        result = ExpressionTreeNode(new Operation::Matrix(token.getText()));
        pos++;
    }
    else if (token.getType() == ParseToken::Variable) {
        map<string, ExpressionTreeNode>::const_iterator subexp = subexpressionDefs.find(token.getText());
        if (subexp == subexpressionDefs.end()) {
            Operation* op = new Operation::Variable(token.getText());
            result = ExpressionTreeNode(op);
        }
        else
            result = subexp->second;
        pos++;
    }
    else if (token.getType() == ParseToken::LeftParen) {
        pos++;
        result = parsePrecedence(tokens, pos, customFunctions, subexpressionDefs, 0);
        if (pos == tokens.size() || tokens[pos].getType() != ParseToken::RightParen)
            throw Exception("Parse error: unbalanced parentheses");
        pos++;
    }
    else if (token.getType() == ParseToken::Function) {
        pos++;
        vector<ExpressionTreeNode> args;
        bool moreArgs;
        do {
            args.push_back(parsePrecedence(tokens, pos, customFunctions, subexpressionDefs, 0));
            moreArgs = (pos < (int) tokens.size() && tokens[pos].getType() == ParseToken::Comma);
            if (moreArgs)
                pos++;
        } while (moreArgs);
        if (pos == tokens.size() || tokens[pos].getType() != ParseToken::RightParen)
            throw Exception("Parse error: unbalanced parentheses");
        pos++;
        Operation* op = getFunctionOperation(token.getText(), customFunctions);

        try {
            result = ExpressionTreeNode(op, args);
        }
        catch (...) {
            delete op;
            throw;
        }
    }
    else if (token.getType() == ParseToken::Operator && token.getText() == "-") {
        pos++;
        ExpressionTreeNode toNegate = parsePrecedence(tokens, pos, customFunctions, subexpressionDefs, 2);
        result = ExpressionTreeNode(new Operation::Negate(), toNegate);
    }
    else
        throw Exception("Parse error: unexpected token: "+token.getText());

    // Now deal with the next binary operator.

    while (pos < (int) tokens.size() && tokens[pos].getType() == ParseToken::Operator) {
        token = tokens[pos];
        int opIndex = (int) Operators.find(token.getText());
        int opPrecedence = Precedence[opIndex];
        if (opPrecedence < precedence)
            return result;
        pos++;
        ExpressionTreeNode arg = parsePrecedence(tokens, pos, customFunctions, subexpressionDefs, LeftAssociative[opIndex] ? opPrecedence+1 : opPrecedence);
        Operation* op = getOperatorOperation(token.getText());
        try {
            result = ExpressionTreeNode(op, result, arg);
        }
        catch (...) {
            delete op;
            throw;
        }
    }
    return result;
}

Operation* Parser::getOperatorOperation(const std::string& name) {
    switch (OperationId[Operators.find(name)]) {
        case Operation::ADD:
            return new Operation::Add();
        case Operation::SUBTRACT:
            return new Operation::Subtract();
        case Operation::MULTIPLY:
            return new Operation::Multiply();
        case Operation::DIVIDE:
            return new Operation::Divide();
        case Operation::POWER:
            return new Operation::Power();
        default:
            throw Exception("Parse error: unknown operator");
    }
}

Operation* Parser::getFunctionOperation(const std::string& name, const map<string, CustomFunction*>& customFunctions) {

    static map<string, Operation::Id> opMap;
    if (opMap.size() == 0) {
        opMap["sqrt"] = Operation::SQRT;
        opMap["exp"] = Operation::EXP;
        opMap["log"] = Operation::LOG;
        opMap["ln"] = Operation::LN;
        opMap["factorial"] = Operation::FACTORIAL;
        opMap["GCD"] = Operation::GCD;
        opMap["LCM"] = Operation::LCM;
        opMap["fnInt"] = Operation::FNINT;
        opMap["sigma"] = Operation::SIGMA;
        opMap["prod"] = Operation::PROD;
        opMap["nPr"] = Operation::NPR;
        opMap["nCr"] = Operation::NCR;     
        opMap["sin"] = Operation::SIN;
        opMap["cos"] = Operation::COS;
        opMap["sec"] = Operation::SEC;
        opMap["csc"] = Operation::CSC;
        opMap["tan"] = Operation::TAN;
        opMap["cot"] = Operation::COT;
        opMap["asin"] = Operation::ASIN;
        opMap["acos"] = Operation::ACOS;
        opMap["atan"] = Operation::ATAN;
        opMap["sinh"] = Operation::SINH;
        opMap["cosh"] = Operation::COSH;
        opMap["tanh"] = Operation::TANH;
        opMap["asinh"] = Operation::ASINH;
        opMap["acosh"] = Operation::ACOSH;
        opMap["atanh"] = Operation::ATANH;
        opMap["acsc"] = Operation::ACSC;
        opMap["csch"] = Operation::CSCH;
        opMap["sech"] = Operation::SECH;
        opMap["coth"] = Operation::COTH;
        opMap["asec"] = Operation::ASEC;
        opMap["acot"] = Operation::ACOT;
        opMap["acsch"] = Operation::ACSCH;
        opMap["asech"] = Operation::ASECH;
        opMap["acoth"] = Operation::ACOTH;
        opMap["and"] = Operation::AND;
        opMap["or"] = Operation::OR;
        opMap["not"] = Operation::NOT;
        opMap["xor"] = Operation::XOR;
        opMap["lls"] = Operation::LOGICAL_LEFT_SHIFT;
        opMap["lrs"] = Operation::LOGICAL_RIGHT_SHIFT;
        opMap["erf"] = Operation::ERF;
        opMap["erfc"] = Operation::ERFC;
        opMap["step"] = Operation::STEP;
        opMap["delta"] = Operation::DELTA;
        opMap["square"] = Operation::SQUARE;
        opMap["cube"] = Operation::CUBE;
        opMap["recip"] = Operation::RECIPROCAL;
        opMap["min"] = Operation::MIN;
        opMap["max"] = Operation::MAX;
        opMap["abs"] = Operation::ABS;
        opMap["RNG"] = Operation::RAND;
        opMap["transpose"] = Operation::TRANSPOSE;
        opMap["det"] = Operation::DET;
        opMap["dot"] = Operation::DOT;
        opMap["cross"] = Operation::CROSS;
        opMap["solveSOLE"] = Operation::SOLVE_SOLE;
        opMap["MOD"] = Operation::MODULUS;
    }
    string trimmed = name.substr(0, name.size()-1);

    // First check custom functions.

    map<string, CustomFunction*>::const_iterator custom = customFunctions.find(trimmed);
    if (custom != customFunctions.end())
        return new Operation::Custom(trimmed, custom->second->clone());

    // Now try standard functions.

    map<string, Operation::Id>::const_iterator iter = opMap.find(trimmed);
    if (iter == opMap.end())
        throw Exception("Parse error: unknown function: "+trimmed);
    switch (iter->second) {
        case Operation::SQRT:
            return new Operation::Sqrt();
        case Operation::EXP:
            return new Operation::Exp();
        case Operation::LOG:
            return new Operation::Log();
        case Operation::LN:
            return new Operation::Ln();
        case Operation::FACTORIAL:
            return new Operation::Factorial();
        case Operation::GCD:
            return new Operation::Gcd();
        case Operation::LCM:
            return new Operation::Lcm();
        case Operation::FNINT:
            return new Operation::FnInt();
        case Operation::SIGMA:
            return new Operation::Sigma();
        case Operation::PROD:
            return new Operation::Prod();
        case Operation::NPR:
            return new Operation::Npr();
        case Operation::NCR:
            return new Operation::Ncr();
        case Operation::SIN:
            return new Operation::Sin(isDegree);
        case Operation::COS:
            return new Operation::Cos(isDegree);
        case Operation::SEC:
            return new Operation::Sec(isDegree);
        case Operation::CSC:
            return new Operation::Csc(isDegree);
        case Operation::TAN:
            return new Operation::Tan(isDegree);
        case Operation::COT:
            return new Operation::Cot(isDegree);
        case Operation::ASIN:
            return new Operation::Asin(isDegree);
        case Operation::ACOS:
            return new Operation::Acos(isDegree);
        case Operation::ATAN:
            return new Operation::Atan(isDegree);
        case Operation::SINH:
            return new Operation::Sinh(isDegree);
        case Operation::COSH:
            return new Operation::Cosh(isDegree);
        case Operation::TANH:
            return new Operation::Tanh(isDegree);
        case Operation::ASINH:
            return new Operation::Asinh(isDegree);
        case Operation::ACOSH:
            return new Operation::Acosh(isDegree);
        case Operation::ATANH:
            return new Operation::Atanh(isDegree);
        case Operation::ACSC:
            return new Operation::Acsc(isDegree);
        case Operation::CSCH:
            return new Operation::Csch(isDegree);
        case Operation::COTH:
            return new Operation::Coth(isDegree);
        case Operation::SECH:
            return new Operation::Sech(isDegree);
        case Operation::ASEC:
            return new Operation::Asec(isDegree);
        case Operation::ACOT:
            return new Operation::Acot(isDegree);
        case Operation::ACSCH:
            return new Operation::Acsch(isDegree);
        case Operation::ASECH:
            return new Operation::Asech(isDegree);
        case Operation::ACOTH:
            return new Operation::Acoth(isDegree);
        case Operation::AND:
            return new Operation::And();
        case Operation::OR:
            return new Operation::Or();
        case Operation::NOT:
            return new Operation::Not();
        case Operation::XOR:
            return new Operation::Xor();
        case Operation::LOGICAL_LEFT_SHIFT:
            return new Operation::LogicalLeftShift();
        case Operation::LOGICAL_RIGHT_SHIFT:
            return new Operation::LogicalRightShift();
        case Operation::ERF:
            return new Operation::Erf();
        case Operation::ERFC:
            return new Operation::Erfc();
        case Operation::STEP:
            return new Operation::Step();
        case Operation::DELTA:
            return new Operation::Delta();
        case Operation::SQUARE:
            return new Operation::Square();
        case Operation::CUBE:
            return new Operation::Cube();
        case Operation::RECIPROCAL:
            return new Operation::Reciprocal();
        case Operation::MIN:
            return new Operation::Min();
        case Operation::MAX:
            return new Operation::Max();
        case Operation::ABS:
            return new Operation::Abs();
        case Operation::RAND:
            return new Operation::Rand();
        case Operation::TRANSPOSE:
            return new Operation::Transpose();
        case Operation::DET:
            return new Operation::Det();
        case Operation::DOT:
            return new Operation::Dot();
        case Operation::CROSS:
            return new Operation::Cross();
        case Operation::SOLVE_SOLE:
            return new Operation::SolveSOLE();
        case Operation::MODULUS:
            return new Operation::Modulus();
        default:
            throw Exception("Parse error: unknown function");
    }
}
