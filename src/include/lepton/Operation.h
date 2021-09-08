#ifndef LEPTON_OPERATION_H_
#define LEPTON_OPERATION_H_

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

#include "windowsIncludes.h"
#include "CustomFunction.h"
#include "Exception.h"
#include "ParsedExpression.h"
#include <cmath>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <random>
#include <complex>

#include <iostream>

namespace Lepton {

class ExpressionTreeNode;

inline bool isMatrix(InputArgType input) {
    if (input.dataTypeEnum != DataTypeEnum::matrix)
        return false;

    return true;
}

/**
 * An Operation represents a single step in the evaluation of an expression, such as a function,
 * an operator, or a constant value.  Each Operation takes some number of values as arguments
 * and produces a single value.
 *
 * This is an abstract class with subclasses for specific operations.
 */

class LEPTON_EXPORT Operation {
public:
    virtual ~Operation() {
    }
    /**
     * This enumeration lists all Operation subclasses.  This is provided so that switch statements
     * can be used when processing or analyzing parsed expressions.
     */
    enum Id {COMPLEX_NUMBER, CONSTANT, VARIABLE, MATRIX, TRANSPOSE, DET, DOT, CROSS, SOLVE_SOLE, CUSTOM, AND, OR, NOT, XOR, LOGICAL_LEFT_SHIFT, LOGICAL_RIGHT_SHIFT,
             ADD, SUBTRACT, MULTIPLY, DIVIDE, MODULUS, POWER, NEGATE, SQRT, EXP, LOG, LN, FACTORIAL, GCD, LCM, FNINT, SIGMA, PROD,
             NPR, NCR, SIN, COS, SEC, CSC, TAN, COT, ASIN, ACOS, ATAN, SINH, COSH, TANH, ASINH, ACOSH, ATANH, ACSC, CSCH, COTH, SECH, ASEC, ACOT, ACSCH, ASECH, ACOTH,
             ERF, ERFC, STEP, DELTA, SQUARE, CUBE, RECIPROCAL, ADD_CONSTANT, MULTIPLY_CONSTANT, POWER_CONSTANT, MIN, MAX, ABS, RAND};
    /**
     * Get the name of this Operation.
     */
    virtual std::string getName() const = 0;
    /**
     * Get this Operation's ID.
     */
    virtual Id getId() const = 0;
    /**
     * Get the number of arguments this operation expects.
     */
    virtual int getNumArguments() const = 0;
    /**
     * Create a clone of this Operation.
     */
    virtual Operation* clone() const = 0;
    /**
     * Perform the computation represented by this operation.
     *
     * @param args        instance of class Args
     * @param variables   a map containing the values of all variables
     * @return the result of performing the computation.
     */
    virtual Result evaluate(Args& args, const std::map<std::string, double>& variables) const = 0;
    /**
     * Return an ExpressionTreeNode which represents the analytic derivative of this Operation with respect to a variable.
     *
     * @param children     the child nodes
     * @param childDerivs  the derivatives of the child nodes with respect to the variable
     * @param variable     the variable with respect to which the derivate should be taken
     */
    virtual ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const = 0;
    /**
     * Get whether this operation should be displayed with infix notation.
     */
    virtual bool isInfixOperator() const {
        return false;
    }
    /**
     * Get whether this is a symmetric binary operation, such that exchanging its arguments
     * does not affect the result.
     */
    virtual bool isSymmetric() const {
        return false;
    }
    virtual bool operator!=(const Operation& op) const {
        return op.getId() != getId();
    }
    virtual bool operator==(const Operation& op) const {
        return !(*this != op);
    }
    class ComplexNumber;
    class Constant;
    class Variable;
    class Matrix;
    class Transpose;
    class Det;
    class Dot;
    class Cross;
    class SolveSOLE;
    class Custom;
    class And;
    class Or;
    class Not;
    class Xor;
    class LogicalLeftShift;
    class LogicalRightShift;
    class Add;
    class Subtract;
    class Multiply;
    class Divide;
    class Modulus;
    class Power;
    class Negate;
    class Sqrt;
    class Exp;
    class Log;
    class Ln;
    class Factorial;
    class Gcd;
    class Lcm;
    class FnInt;
    class Sigma;
    class Prod;
    class Npr;
    class Ncr;
    class Sin;
    class Cos;
    class Sec;
    class Csc;
    class Tan;
    class Cot;
    class Asin;
    class Acos;
    class Atan;
    class Sinh;
    class Cosh;
    class Tanh;
    class Asinh;
    class Acosh;
    class Atanh;
    class Acsc;
    class Csch;
    class Coth;
    class Sech;
    class Asec;
    class Acot;
    class Acsch;
    class Asech;
    class Acoth;
    class Erf;
    class Erfc;
    class Step;
    class Delta;
    class Square;
    class Cube;
    class Reciprocal;
    class AddConstant;
    class MultiplyConstant;
    class PowerConstant;
    class Min;
    class Max;
    class Abs;
    class Rand;
    class TrigonometricFunction;
};

class LEPTON_EXPORT Operation::TrigonometricFunction : public Operation {
protected:
    TrigonometricFunction() : isDegree(false) {}
    TrigonometricFunction(bool isDegree) : isDegree(isDegree) {}

    bool isDegree;
    const double radianConversionFactor = 3.14159265358979323846 / 180;
};

class LEPTON_EXPORT Operation::ComplexNumber : public Operation {
public:
    ComplexNumber(double real, double imaginary) : value(real, imaginary) {
        //value = complex<double>(real, imaginary);
    }
    std::string getName() const {
        std::stringstream name;
        std::string operation = imag(value) < 0 ? " - " : " + ";
        name << real(value) << operation << abs(imag(value));
        return name.str();
    }
    Id getId() const {
        return COMPLEX_NUMBER;
    }
    int getNumArguments() const {
        return 0;
    }
    Operation* clone() const {
        return new ComplexNumber(real(value), imag(value));
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        return Result(value);
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    bool operator!=(const Operation& op) const {
        const ComplexNumber* o = dynamic_cast<const ComplexNumber*>(&op);
        return (o == NULL || o->value != value);
    }
private:
    std::complex<double> value;
};

class LEPTON_EXPORT Operation::Constant : public Operation {
public:
    Constant(double value) : value(value) {
    }
    std::string getName() const {
        std::stringstream name;
        name << value;
        return name.str();
    }
    Id getId() const {
        return CONSTANT;
    }
    int getNumArguments() const {
        return 0;
    }
    Operation* clone() const {
        return new Constant(value);
    }

    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        return Result(value);
    }

    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    double getValue() const {
        return value;
    }
    bool operator!=(const Operation& op) const {
        const Constant* o = dynamic_cast<const Constant*>(&op);
        return (o == NULL || o->value != value);
    }
private:
    double value;
};

class LEPTON_EXPORT Operation::Variable : public Operation {
public:
    Variable(const std::string& name) : name(name) {
    }
    std::string getName() const {
        return name;
    }
    Id getId() const {
        return VARIABLE;
    }
    int getNumArguments() const {
        return 0;
    }
    Operation* clone() const {
        return new Variable(name);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        std::map<std::string, double>::const_iterator iter = variables.find(name);
        if (iter == variables.end())
            throw Exception("No value specified for variable "+name);
        return Result(iter->second);
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    bool operator!=(const Operation& op) const {
        const Variable* o = dynamic_cast<const Variable*>(&op);
        return (o == NULL || o->name != name);
    }
private:
    std::string name;
};

class LEPTON_EXPORT Operation::Matrix : public Operation {
public:
    Matrix(const std::string& name) : name(name) {
    }
    std::string getName() const {
        return name;
    }
    Id getId() const {
        return MATRIX;
    }
    int getNumArguments() const {
        return 0;
    }
    Operation* clone() const {
        return new Matrix(name);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        return Result(getMatrixValue(name));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
private:
    std::string name;
};

class LEPTON_EXPORT Operation::Transpose : public Operation {
public:
    Transpose() {
    }
    std::string getName() const {
        return "transpose";
    }
    Id getId() const {
        return TRANSPOSE;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Transpose();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (!isMatrix(args.inputs[0]))
            throw Exception("Error: Argument is not a matrix");

        return Result(args.inputs[0].matrix.transpose());
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Det : public Operation {
public:
    Det() {
    }
    std::string getName() const {
        return "det";
    }
    Id getId() const {
        return DET;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Det();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (!isMatrix(args.inputs[0]))
            throw Exception("Error: Argument is not a matrix");

        return Result(args.inputs[0].matrix.determinant());
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Dot : public Operation {
public:
    Dot() {
    }
    std::string getName() const {
        return "dot";
    }
    Id getId() const {
        return DOT;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new Dot();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if ((!isMatrix(args.inputs[0]) || (!isMatrix(args.inputs[1]))))
            throw Exception("Error: Argument is not a matrix");

        if (args.inputs[0].matrix.cols() != 1 || args.inputs[1].matrix.cols() != 1) {
            throw Exception("Error: Not a nx1 matrix");
        }

        Eigen::VectorXd v(Eigen::Map<Eigen::VectorXd>(args.inputs[0].matrix.data(), args.inputs[0].matrix.rows()));
        Eigen::VectorXd w(Eigen::Map<Eigen::VectorXd>(args.inputs[1].matrix.data(), args.inputs[1].matrix.rows()));
        return Result(v.dot(w));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Cross : public Operation {
public:
    Cross() {
    }
    std::string getName() const {
        return "cross";
    }
    Id getId() const {
        return CROSS;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new Cross();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if ((!isMatrix(args.inputs[0]) || (!isMatrix(args.inputs[1]))))
            throw Exception("Error: Argument is not a Matrix");

        if (args.inputs[0].matrix.rows() != 3 || args.inputs[0].matrix.cols() != 1 ||
            args.inputs[1].matrix.rows() != 3 || args.inputs[1].matrix.cols() != 1) {
            throw Exception("Error: Not a 3x1 matrix");
        }

        Eigen::Vector3d v(Eigen::Map<Eigen::Vector3d>(args.inputs[0].matrix.data(), 3));
        Eigen::Vector3d w(Eigen::Map<Eigen::Vector3d>(args.inputs[1].matrix.data(), 3));
        return Result(v.cross(w));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::SolveSOLE : public Operation {
public:
    SolveSOLE() {
    }
    std::string getName() const {
        return "solveSOLE";
    }
    Id getId() const {
        return SOLVE_SOLE;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new SolveSOLE();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if ((!isMatrix(args.inputs[0]) || (!isMatrix(args.inputs[1]))))
            throw Exception("Error: Argument is not a Matrix");

        Eigen::MatrixXd x = args.inputs[0].matrix.householderQr().solve(args.inputs[1].matrix);
        return Result(x);
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Custom : public Operation {
public:
    Custom(const std::string& name, CustomFunction* function) : name(name), function(function), isDerivative(false), derivOrder(function->getNumArguments(), 0) {
    }
    Custom(const Custom& base, int derivIndex) : name(base.name), function(base.function->clone()), isDerivative(true), derivOrder(base.derivOrder) {
        derivOrder[derivIndex]++;
    }
    ~Custom() {
        delete function;
    }
    std::string getName() const {
        return name;
    }
    Id getId() const {
        return CUSTOM;
    }
    int getNumArguments() const {
        return function->getNumArguments();
    }
    Operation* clone() const {
        Custom* clone = new Custom(name, function->clone());
        clone->isDerivative = isDerivative;
        clone->derivOrder = derivOrder;
        return clone;
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Argument must be purely real");
        }

        double val = args.inputs[0].getReal();
        
        if (isDerivative)
            return function->evaluateDerivative(&args.inputs[0].realVal, &derivOrder[0]);
        return Result(function->evaluate(&args.inputs[0].realVal));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    const std::vector<int>& getDerivOrder() const {
        return derivOrder;
    }
    bool operator!=(const Operation& op) const {
        const Custom* o = dynamic_cast<const Custom*>(&op);
        return (o == NULL || o->name != name || o->isDerivative != isDerivative || o->derivOrder != derivOrder);
    }
private:
    std::string name;
    CustomFunction* function;
    bool isDerivative;
    std::vector<int> derivOrder;
};

class LEPTON_EXPORT Operation::And : public Operation {
public:
    And() {
    }
    std::string getName() const {
        return "and";
    }
    Id getId() const {
        return AND;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new And();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0]) || isMatrix(args.inputs[1])) {
                throw Exception("Matrices are not supported with and");
        }
        
        if (args.inputs[0].getImag() != 0 || args.inputs[1].getImag() != 0) {
            throw Exception("Error: Arguments must be purely real");
        }
        return Result((long)round(args.inputs[0].getReal()) & (long)round(args.inputs[1].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    bool isInfixOperator() const {
        return true;
    }
    bool isSymmetric() const {
        return true;
    }
};

class LEPTON_EXPORT Operation::Or : public Operation {
public:
    Or() {
    }
    std::string getName() const {
        return "or";
    }
    Id getId() const {
        return OR;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new Or();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0]) || isMatrix(args.inputs[1])) {
                throw Exception("Matrices are not supported with or");
        }
        
        if (args.inputs[0].getImag() != 0 || args.inputs[1].getImag() != 0) {
            throw Exception("Error: Arguments must be purely real");
        }
        return Result((long)round(args.inputs[0].getReal()) | (long)round(args.inputs[1].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    bool isInfixOperator() const {
        return true;
    }
    bool isSymmetric() const {
        return true;
    }
};

class LEPTON_EXPORT Operation::Not : public Operation {
public:
    Not() {
    }
    std::string getName() const {
        return "not";
    }
    Id getId() const {
        return NOT;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Not();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
                throw Exception("Matrices are not supported with not");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Error: Arguments must be purely real");
        }
        return Result(~(long)round(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Xor : public Operation {
public:
    Xor() {
    }
    std::string getName() const {
        return "xor";
    }
    Id getId() const {
        return XOR;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new Xor();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0]) || isMatrix(args.inputs[1])) {
                throw Exception("Matrices are not supported with xor");
        }
        
        if (args.inputs[0].getImag() != 0 || args.inputs[1].getImag() != 0) {
            throw Exception("Error: Arguments must be purely real");
        }
        return Result((long)round(args.inputs[0].getReal()) ^ (long)round(args.inputs[1].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    bool isInfixOperator() const {
        return true;
    }
    bool isSymmetric() const {
        return true;
    }
};

class LEPTON_EXPORT Operation::LogicalLeftShift : public Operation {
public:
    LogicalLeftShift() {
    }
    std::string getName() const {
        return "lls";
    }
    Id getId() const {
        return LOGICAL_LEFT_SHIFT;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new LogicalLeftShift();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0]) || isMatrix(args.inputs[1])) {
                throw Exception("Matrices are not supported with lls");
        }
        
        if (args.inputs[0].getImag() != 0 || args.inputs[1].getImag() != 0) {
            throw Exception("Error: Arguments must be purely real");
        }
        return Result((long)round(args.inputs[0].getReal()) << (long)round(args.inputs[1].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    bool isInfixOperator() const {
        return true;
    }
};

class LEPTON_EXPORT Operation::LogicalRightShift : public Operation {
public:
    LogicalRightShift() {
    }
    std::string getName() const {
        return "lrs";
    }
    Id getId() const {
        return LOGICAL_RIGHT_SHIFT;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new LogicalRightShift();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0]) || isMatrix(args.inputs[1])) {
                throw Exception("Matrices are not supported with lrs");
        }
        
        if (args.inputs[0].getImag() != 0 || args.inputs[1].getImag() != 0) {
            throw Exception("Error: Arguments must be purely real");
        }
        return Result((long)round(args.inputs[0].getReal()) >> (long)round(args.inputs[1].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    bool isInfixOperator() const {
        return true;
    }
};

class LEPTON_EXPORT Operation::Add : public Operation {
public:
    Add() {
    }
    std::string getName() const {
        return "+";
    }
    Id getId() const {
        return ADD;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new Add();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (args.inputs[0].dataTypeEnum == DataTypeEnum::matrix && args.inputs[1].dataTypeEnum == DataTypeEnum::matrix) {
            if (args.inputs[0].matrix.rows() != args.inputs[1].matrix.rows() || 
                args.inputs[0].matrix.cols() != args.inputs[1].matrix.cols()) {
                throw Exception("LHS rows or cols != RHS rows or cols");
            }
            
            return Result(args.inputs[0].matrix+args.inputs[1].matrix);
        }
        else if (args.inputs[0].dataTypeEnum == DataTypeEnum::complexVal && args.inputs[1].dataTypeEnum == DataTypeEnum::complexVal) {
            return Result(args.inputs[0].getComplex()+args.inputs[1].getComplex());
        }

        throw Exception("Error: Cannot add scalar and matrix");
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    bool isInfixOperator() const {
        return true;
    }
    bool isSymmetric() const {
        return true;
    }
};

class LEPTON_EXPORT Operation::Subtract : public Operation {
public:
    Subtract() {
    }
    std::string getName() const {
        return "-";
    }
    Id getId() const {
        return SUBTRACT;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new Subtract();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (args.inputs[0].dataTypeEnum == DataTypeEnum::matrix && args.inputs[1].dataTypeEnum == DataTypeEnum::matrix) {
            if (args.inputs[0].matrix.rows() != args.inputs[1].matrix.rows() || 
                args.inputs[0].matrix.cols() != args.inputs[1].matrix.cols()) {
                throw Exception("LHS rows or cols != RHS rows or cols");
            }
            
            return Result(args.inputs[0].matrix-args.inputs[1].matrix);
        }
        else if (args.inputs[0].dataTypeEnum == DataTypeEnum::complexVal && args.inputs[1].dataTypeEnum == DataTypeEnum::complexVal) {
            return Result(args.inputs[0].getComplex()-args.inputs[1].getComplex());
        }

        throw Exception("Error: Cannot add scalar and matrix");
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    bool isInfixOperator() const {
        return true;
    }
};

class LEPTON_EXPORT Operation::Multiply : public Operation {
public:
    Multiply() {
    }
    std::string getName() const {
        return "*";
    }
    Id getId() const {
        return MULTIPLY;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new Multiply();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (args.inputs[0].dataTypeEnum == DataTypeEnum::matrix) {
            if (args.inputs[1].dataTypeEnum == DataTypeEnum::complexVal && args.inputs[1].getImag() != 0) {
                throw Exception("Matrix multiplication with complex numbers is not supported");
            }
            if (args.inputs[1].dataTypeEnum == DataTypeEnum::matrix) {
                if (args.inputs[0].matrix.cols() != args.inputs[1].matrix.rows()) {
                    throw Exception("LHS cols != RHS rows");
                }

                return Result(args.inputs[0].matrix*args.inputs[1].matrix);
            }

            return Result(args.inputs[0].matrix*args.inputs[1].getReal());
        }
        else if (args.inputs[1].dataTypeEnum == DataTypeEnum::matrix) {
            if (args.inputs[0].getImag() != 0) {
                throw Exception("Matrix multiplication with complex numbers is not supported");
            }
            return Result(args.inputs[0].getReal()*args.inputs[1].matrix);
        }
        
        return Result(args.inputs[0].getComplex()*args.inputs[1].getComplex());
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    bool isInfixOperator() const {
        return true;
    }
    bool isSymmetric() const {
        return true;
    }
};

class LEPTON_EXPORT Operation::Divide : public Operation {
public:
    Divide() {
    }
    std::string getName() const {
        return "/";
    }
    Id getId() const {
        return DIVIDE;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new Divide();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (args.inputs[0].dataTypeEnum == DataTypeEnum::matrix || args.inputs[1].dataTypeEnum == DataTypeEnum::matrix) {
            throw Exception("Error: Cannot divide with matrices");
        }

        return Result(args.inputs[0].getComplex()/args.inputs[1].getComplex());
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    bool isInfixOperator() const {
        return true;
    }
};

class LEPTON_EXPORT Operation::Modulus : public Operation {
public:
    Modulus() {
    }
    std::string getName() const {
        return "MOD";
    }
    Id getId() const {
        return MODULUS;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new Modulus();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0]) || isMatrix(args.inputs[1])) {
                throw Exception("Matrices are not supported with modulus");
        }
        
        if (args.inputs[0].getImag() != 0 || args.inputs[1].getImag() != 0) {
            throw Exception("Modulus operation not supported for complex numbers");
        }

        return Result((int)args.inputs[0].getReal() % (int)args.inputs[1].getReal());
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    bool isInfixOperator() const {
        return true;
    }
};

class LEPTON_EXPORT Operation::Power : public Operation {
public:
    Power() {
    }
    std::string getName() const {
        return "^";
    }
    Id getId() const {
        return POWER;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new Power();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (args.inputs[1].dataTypeEnum == DataTypeEnum::matrix) {
            throw Exception("Error: cannot raise to the power of a matrix");
        }
        
        if (args.inputs[0].dataTypeEnum == DataTypeEnum::matrix) {
            if (args.inputs[1].getImag() != 0) {
                throw Exception("Error: cannot raise matrix to power of complex number");
            }
            return Result(args.inputs[0].matrix.pow(args.inputs[1].getReal()));
        }

        return Result(std::pow(args.inputs[0].getComplex(), args.inputs[1].getComplex()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    bool isInfixOperator() const {
        return true;
    }
};

class LEPTON_EXPORT Operation::Negate : public Operation {
public:
    Negate() {
    }
    std::string getName() const {
        return "-";
    }
    Id getId() const {
        return NEGATE;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Negate();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            return Result(args.inputs[0].matrix * -1);
        }

        return Result(-args.inputs[0].getComplex());
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Sqrt : public Operation {
public:
    Sqrt() {
    }
    std::string getName() const {
        return "sqrt";
    }
    Id getId() const {
        return SQRT;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Sqrt();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
                throw Exception("Matrices are not supported with sqrt");
        }

        return Result(std::sqrt(args.inputs[0].getComplex()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Exp : public Operation {
public:
    Exp() {
    }
    std::string getName() const {
        return "exp";
    }
    Id getId() const {
        return EXP;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Exp();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
                throw Exception("Matrices are not supported with exp");
        }

        return Result(std::exp(args.inputs[0].getComplex()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Log : public Operation {
public:
    Log() {
    }
    std::string getName() const {
        return "log";
    }
    Id getId() const {
        return LOG;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new Log();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0]) || isMatrix(args.inputs[1])) {
                throw Exception("Matrices are not supported with log");
        }

        return Result(std::log(args.inputs[1].getComplex())/std::log(args.inputs[0].getComplex()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Ln : public Operation {
public:
    Ln() {
    }
    std::string getName() const {
        return "ln";
    }
    Id getId() const {
        return LN;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Ln();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
                throw Exception("Matrices are not supported with ln");
        }

        return Result(std::log(args.inputs[0].getComplex()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Factorial : public Operation {
public:
    Factorial() {
    }
    std::string getName() const {
        return "factorial";
    }
    Id getId() const {
        return FACTORIAL;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Factorial();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
                throw Exception("Matrices are not supported with factorial");
        }
        
        // Computation with complex numbers involves integral calculations for the gamma function
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Error: Factorial for complex numbers is not supported");
        }

        if (args.inputs[0].getReal() < 0)
            throw Exception("Math Error: Factorial of a negative number does not exist");
        
        double res = 1;
        for(int i = 1; i <=args.inputs[0].getReal(); ++i) {
            res *= i;
        }

        return Result(res);
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Gcd : public Operation {
public:
    Gcd() {
    }
    std::string getName() const {
        return "GCD";
    }
    Id getId() const {
        return GCD;
    }
    int getNumArguments() const {
        return -1;
    }
    Operation* clone() const {
        return new Gcd();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        for (int i = 0; i < args.inputs.size(); i++) {
            if (isMatrix(args.inputs[i])) {
                throw Exception("Matrices are not supported with gcd");
            }

            if (args.inputs[0].getImag() != 0) {
                throw Exception("GCD with complex numbers not supported");
            }
        }

        double result = args.inputs[0].getReal();
        for (int i = 1; i < args.inputs.size(); i++)
        {
            result = std::gcd((long)args.inputs[i].getReal(), (long)result);
            if(result == 1)
            {
                return 1;
            }
        }
        return Result(result);
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Lcm : public Operation {
public:
    Lcm() {
    }
    std::string getName() const {
        return "LCM";
    }
    Id getId() const {
        return LCM;
    }
    int getNumArguments() const {
        return -1;
    }
    Operation* clone() const {
        return new Lcm();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        for (int i = 0; i<args.inputs.size(); i++) {
            if (isMatrix(args.inputs[0]) || isMatrix(args.inputs[1])) {
                throw Exception("Matrices are not supported with lcm");
            }

            if (args.inputs[i].getImag() != 0) {
                throw Exception("LCM with complex numbers not supported");
            }
        }
        double result = args.inputs[0].getReal();
        for (int i = 1; i < args.inputs.size(); i++) {
            result = (((args.inputs[i].getReal() * result)) /
                    (std::gcd((long)args.inputs[i].getReal(), (long)result)));
        }
    
        return Result(result);
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Npr : public Operation {
public:
    Npr() {
    }
    std::string getName() const {
        return "nPr";
    }
    Id getId() const {
        return NPR;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new Npr();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0]) || isMatrix(args.inputs[1])) {
            throw Exception("Matrices are not supported with nPr");
        }
        
        if (args.inputs[0].getReal() != 0 || args.inputs[1].getReal() != 0) {
            throw Exception("NPR with complex numbers not supported");
        }

        Factorial factorial;

        double numerator = (factorial.evaluate(args, variables)).getReal();
        double diff = args.inputs[0].getReal() - args.inputs[1].getReal();
        std::vector<InputArgType> vec(1);
        vec[0] = InputArgType(diff);
        Args tempArgs;
        tempArgs.inputs = vec;
        double denominator = (factorial.evaluate(tempArgs, variables)).getReal();
        return Result(numerator/denominator);
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

//Returns a root of the equation, or nan if there wasn't
static double getZero(Args& args, const std::map<std::string, double>& variables, double xLeft, double xRight){
    double retVal;

    std::map<std::string, double> tempVariables;
    tempVariables.insert(variables.begin(), variables.end());
    tempVariables[args.variableName] = xLeft;

    //First check to see a difference in sign
    bool leftSign = (ParsedExpression::publicEvaluate(args.node, tempVariables)).getReal() < 0;
    tempVariables[args.variableName] = xRight;
    bool rightSign = (ParsedExpression::publicEvaluate(args.node, tempVariables)).getReal() < 0;

    if(!(leftSign ^ rightSign))
        return nan(""); //TODO: Will quit prematurely when there are an even number of roots...

    for(int i = 0; i < 50; i++){//Error is halved on each iteration
        retVal = xLeft + (xRight-xLeft)/2.0;//Apparently there is less subtractive cancellation when calculating this way
        tempVariables[args.variableName] = retVal;
        rightSign = (ParsedExpression::publicEvaluate(args.node, tempVariables)).getReal() < 0;
        if(leftSign ^ rightSign)
            xRight = retVal;
        else{
            xLeft = retVal;
            leftSign = rightSign;
        }
    }

    return retVal;
}

//Returns the x value of the extrema of the given function
static double getExtrema(Args& args, const std::map<std::string, double>& variables, double xLeft, double xRight, bool max){
    unsigned int numDivisions = 160; //Safety margin of screen width, decreases later for faster convergence
    double step = (xRight-xLeft)/numDivisions;

    //Set up to evaluate
    std::map<std::string, double> tempVariables;
    tempVariables.insert(variables.begin(), variables.end());
    tempVariables[args.variableName] = xLeft;

    double extreme = (ParsedExpression::publicEvaluate(args.node, tempVariables)).getReal();
    double retVal = xLeft;

    for(int i = 0; i < 30; i++){//Tuneable for precision
        for(unsigned int j = 0; j < numDivisions; j++){
            double y = (ParsedExpression::publicEvaluate(args.node, tempVariables)).getReal();
            if((max && y > extreme) || (!max && y < extreme)){
                retVal = tempVariables[args.variableName];
                extreme = y;
            }
            tempVariables[args.variableName] += step;
        }
        numDivisions = 4; //Error halved on each iteration
        //Clamp the ret value to the interval
        if(retVal-step < xLeft)
            tempVariables[args.variableName] = xLeft;
        else if(retVal+step > xRight)
            tempVariables[args.variableName] = xRight-2.0*step;
        else
            tempVariables[args.variableName] = retVal-step;//Step through the left eval point to the right eval point
        step *= 2.0/numDivisions;
    }

    //Result at this point is guaranteed to be no lower than xLeft
    //Result is not guaranteed to be less than xRight (possible floating point error)
    return retVal > xRight ? xRight : retVal;
}

class LEPTON_EXPORT Operation::FnInt : public Operation {
public:
    FnInt() {
    }
    std::string getName() const {
        return "fnInt";
    }
    Id getId() const {
        return FNINT;
    }
    int getNumArguments() const {
        return 4;
    }
    Operation* clone() const {
        return new FnInt();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0]) || isMatrix(args.inputs[1])) {
            throw Exception("Matrices are not supported with nCr");
        }
        
        // Simpons three point rule
        double start = args.inputs[0].getReal();
        double end = args.inputs[1].getReal();
        if (start > end)
            return 0;

        std::map<std::string, double> tempVariables;
        tempVariables.insert(variables.begin(), variables.end());

        // can increase/decrease num intervals to adjust accuracy and speed
        const std::size_t number_of_intervals = 1000;
        const double h = (end - start) / (2 * number_of_intervals);
        double result = 0;

        for (std::size_t i = 0; i < number_of_intervals; ++i)
        {
            tempVariables[args.variableName] = start + 2 * i * h;
            const double y0 = (ParsedExpression::publicEvaluate(args.node, tempVariables)).getReal(); tempVariables[args.variableName] += h;
            const double y1 = (ParsedExpression::publicEvaluate(args.node, tempVariables)).getReal(); tempVariables[args.variableName] += h;
            const double y2 = (ParsedExpression::publicEvaluate(args.node, tempVariables)).getReal(); tempVariables[args.variableName] += h;
            result += h * (y0 + 4 * y1 + y2) / 3;
        }

        return Result(result);
    }
    
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Sigma : public Operation {
public:
    Sigma() {
    }
    std::string getName() const {
        return "sigma";
    }
    Id getId() const {
        return SIGMA;
    }
    int getNumArguments() const {
        return 4;
    }
    Operation* clone() const {
        return new Sigma();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0]) || isMatrix(args.inputs[1])) {
            throw Exception("Matrices are not supported with nCr");
        }
        
        if (args.inputs[0].getImag() != 0 || args.inputs[1].getImag() != 0) {
            throw Exception("Summation limits must be purely real");
        }
        int start = (int) args.inputs[0].getReal();
        int end = (int) args.inputs[1].getReal();
        if (start > end)
            return 0;

        std::map<std::string, double> tempVariables;
        tempVariables.insert(variables.begin(), variables.end());

        // double result = 0;
        std::complex<double> result(0, 0);
        for (;start<=end; start++)
        {
            tempVariables[args.variableName] = start;
            result += ParsedExpression::publicEvaluate(args.node, tempVariables).getComplex();
        }

        return Result(result);
    }
    
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Prod : public Operation {
public:
    Prod() {
    }
    std::string getName() const {
        return "prod";
    }
    Id getId() const {
        return PROD;
    }
    int getNumArguments() const {
        return 4;
    }
    Operation* clone() const {
        return new Prod();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0]) || isMatrix(args.inputs[1])) {
            throw Exception("Matrices are not supported with prod");
        }

        if (args.inputs[0].getImag() != 0 || args.inputs[1].getImag() != 0) {
            throw Exception("prod limits must be purely real");
        }
        
        int start = (int) args.inputs[0].getReal();
        int end = (int) args.inputs[1].getReal();
        if (start > end)
            return 0;

        std::map<std::string, double> tempVariables;
        tempVariables.insert(variables.begin(), variables.end());

        // double result = 1;
        tempVariables[args.variableName] = start;
        std::complex<double> result = ParsedExpression::publicEvaluate(args.node, tempVariables).getComplex();        
        start++;
        for (;start<=end; start++)
        {
            tempVariables[args.variableName] = start;
            result *= ParsedExpression::publicEvaluate(args.node, tempVariables).getComplex();
        }
        return Result(result);
    }
    
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Ncr : public Operation {
public:
    Ncr() {
    }
    std::string getName() const {
        return "nCr";
    }
    Id getId() const {
        return NCR;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new Ncr();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0]) || isMatrix(args.inputs[1])) {
            throw Exception("Matrices are not supported with nCr");
        }
        
        if (args.inputs[0].getImag() != 0 || args.inputs[1].getImag() != 0) {
            throw Exception("NCR with complex numbers not supported");
        }

        Factorial factorial;
        Npr npr;
        
        std::vector<InputArgType> vec(1);
        vec[0] = InputArgType(args.inputs[1].getReal());
        Args tempArgs;
        tempArgs.inputs = vec;
        return Result((npr.evaluate(args, variables)).getReal()/(factorial.evaluate(tempArgs, variables)).getReal());
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Sin : public TrigonometricFunction {
public:
    Sin() : TrigonometricFunction() {
    }
    Sin(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "sin";
    }
    Id getId() const {
        return SIN;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Sin(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return Result(std::sin(args.inputs[0].getReal() * radianConversionFactor));
        }
        return Result(std::sin(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Cos : public TrigonometricFunction {
public:
    Cos() : TrigonometricFunction() {
    }
    Cos(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "cos";
    }
    Id getId() const {
        return COS;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Cos(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return Result(std::cos(args.inputs[0].getReal() * radianConversionFactor));
        }
        return Result(std::cos(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Sec : public TrigonometricFunction {
public:
    Sec() : TrigonometricFunction() {
    }
    Sec(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "sec";
    }
    Id getId() const {
        return SEC;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Sec(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return Result(1.0/std::cos(args.inputs[0].getReal() * radianConversionFactor));
        }
        return Result(1.0/std::cos(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Csc : public TrigonometricFunction {
public:
    Csc() : TrigonometricFunction() {
    }
    Csc(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "csc";
    }
    Id getId() const {
        return CSC;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Csc(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return Result(1.0/std::sin(args.inputs[0].getReal() * radianConversionFactor));
        }
        return Result(1.0/std::sin(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Tan : public TrigonometricFunction {
public:
    Tan() : TrigonometricFunction() {
    }
    Tan(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "tan";
    }
    Id getId() const {
        return TAN;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Tan(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return Result(std::tan(args.inputs[0].getReal() * radianConversionFactor));
        }        
        return Result(std::tan(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Cot : public TrigonometricFunction {
public:
    Cot() : TrigonometricFunction() {
    }
    Cot(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "cot";
    }
    Id getId() const {
        return COT;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Cot(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }

        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return Result(1.0/std::tan(args.inputs[0].getReal() * radianConversionFactor));
        }  
        return Result(1.0/std::tan(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Asin : public TrigonometricFunction {
public:
    Asin() : TrigonometricFunction() {
    }
    Asin(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "asin";
    }
    Id getId() const {
        return ASIN;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Asin(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return Result(std::asin(args.inputs[0].getReal() * radianConversionFactor));
        } 
        return Result(std::asin(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Acos : public TrigonometricFunction {
public:
    Acos() : TrigonometricFunction() {
    }
    Acos(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "acos";
    }
    Id getId() const {
        return ACOS;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Acos(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return Result(std::acos(args.inputs[0].getReal() * radianConversionFactor));
        } 
        return Result(std::acos(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Atan : public TrigonometricFunction {
public:
    Atan() : TrigonometricFunction() {
    }
    Atan(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "atan";
    }
    Id getId() const {
        return ATAN;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Atan(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return Result(std::atan(args.inputs[0].getReal() * radianConversionFactor));
        } 
        return Result(std::atan(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Sinh : public TrigonometricFunction {
public:
    Sinh() : TrigonometricFunction() {
    }
    Sinh(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "sinh";
    }
    Id getId() const {
        return SINH;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Sinh(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return Result(std::sinh(args.inputs[0].getReal() * radianConversionFactor));
        } 
        return std::sinh(args.inputs[0].getReal());
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Cosh : public TrigonometricFunction {
public:
    Cosh() : TrigonometricFunction() {
    }
    Cosh(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "cosh";
    }
    Id getId() const {
        return COSH;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Cosh(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return std::cosh(args.inputs[0].getReal() * radianConversionFactor);
        } 
        return Result(std::cosh(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Tanh : public TrigonometricFunction {
public:
    Tanh() : TrigonometricFunction() {
    }
    Tanh(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "tanh";
    }
    Id getId() const {
        return TANH;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Tanh(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return Result(std::tanh(args.inputs[0].getReal() * radianConversionFactor));
        } 
        return Result(std::tanh(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Asinh : public TrigonometricFunction {
public:
    Asinh() : TrigonometricFunction() {
    }
    Asinh(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "asinh";
    }
    Id getId() const {
        return ASINH;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Asinh(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return Result(std::asinh(args.inputs[0].getReal() * radianConversionFactor));
        } 
        return Result(std::asinh(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Acosh : public TrigonometricFunction {
public:
    Acosh() : TrigonometricFunction() {
    }
    Acosh(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "acosh";
    }
    Id getId() const {
        return ACOSH;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Acosh(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return Result(std::acosh(args.inputs[0].getReal() * radianConversionFactor));
        } 
        return Result(std::acosh(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Atanh : public TrigonometricFunction {
public:
    Atanh() : TrigonometricFunction() {
    }
    Atanh(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "atanh";
    }
    Id getId() const {
        return ATANH;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Atanh(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return Result(std::atanh(args.inputs[0].getReal() * radianConversionFactor));
        } 
        return Result(std::atanh(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Acsc : public TrigonometricFunction {
public:
    Acsc() : TrigonometricFunction() {
    }
    Acsc(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "acsc";
    }
    Id getId() const {
        return ACSC;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Acsc(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return Result(std::asin(1 / (args.inputs[0].getReal() * radianConversionFactor)));
        }
        return Result(std::asin(1 / args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Csch : public TrigonometricFunction {
public:
    Csch() : TrigonometricFunction() {
    }
    Csch(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "csch";
    }
    Id getId() const {
        return CSCH;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Csch(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }

        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }
        
        if (isDegree) {
            return Result(1/std::sinh(args.inputs[0].getReal() * radianConversionFactor));
        }
        return Result(1/std::sinh(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Coth : public TrigonometricFunction {
public:
    Coth() : TrigonometricFunction() {
    }
    Coth(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "coth";
    }
    Id getId() const {
        return COTH;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Coth(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }

        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }
        
        if (isDegree) {
            return Result(std::cosh(args.inputs[0].getReal() * radianConversionFactor)/std::sinh(args.inputs[0].getReal() * radianConversionFactor));
        }
        return Result(std::cosh(args.inputs[0].getReal())/std::sinh(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Sech : public TrigonometricFunction {
public:
    Sech() : TrigonometricFunction() {
    }
    Sech(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "sech";
    }
    Id getId() const {
        return SECH;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Sech(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }

        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }
        
        if (isDegree) {
            return Result(1/std::cosh(args.inputs[0].getReal() * radianConversionFactor));
        }
        return Result(1/std::cosh(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Asec : public TrigonometricFunction {
public:
    Asec() : TrigonometricFunction() {
    }
    Asec(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "asec";
    }
    Id getId() const {
        return ASEC;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Asec(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }

        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }
        
        if (isDegree) {
            return Result(std::acos(1/(args.inputs[0].getReal() * radianConversionFactor)));
        }
        return Result(std::acos(1/(args.inputs[0].getReal())));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Acot : public TrigonometricFunction {
public:
    Acot() : TrigonometricFunction() {
    }
    Acot(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "acot";
    }
    Id getId() const {
        return ACOT;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Acot(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }

        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return Result(M_PI_2 - std::atan(args.inputs[0].getReal() * radianConversionFactor));
        }
        return Result(M_PI_2 - std::atan(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Acsch : public TrigonometricFunction {
public:
    Acsch() : TrigonometricFunction() {
    }
    Acsch(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "acsch";
    }
    Id getId() const {
        return ACSCH;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Acsch(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }

        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }

        if (isDegree) {
            return Result(std::log(std::sqrt(1+1/std::pow(args.inputs[0].getReal()*radianConversionFactor, 2))+1/(args.inputs[0].getReal()*radianConversionFactor)));
        }

        return Result(std::log(std::sqrt(1+1/std::pow(args.inputs[0].getReal(), 2))+1/args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Asech : public TrigonometricFunction {
public:
    Asech() : TrigonometricFunction() {
    }
    Asech(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "asech";
    }
    Id getId() const {
        return ASECH;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Asech(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }

        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }
        
        if (isDegree) {
            return Result(std::log(std::sqrt(1/(args.inputs[0].getReal()*radianConversionFactor)-1)*std::sqrt(1+1/(args.inputs[0].getReal()*radianConversionFactor))+1/(args.inputs[0].getReal()*radianConversionFactor)));
        }

        return Result(std::log(std::sqrt(1/args.inputs[0].getReal()-1)*std::sqrt(1+1/args.inputs[0].getReal())+1/args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Acoth : public TrigonometricFunction {
public:
    Acoth() : TrigonometricFunction() {
    }
    Acoth(bool isDegree) : TrigonometricFunction(isDegree) {}
    std::string getName() const {
        return "acoth";
    }
    Id getId() const {
        return ACOTH;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Acoth(isDegree);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with trig functions");
        }

        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with trigonometric functions");
        }
        
        if (isDegree) {
            return Result(std::atanh(1/(args.inputs[0].getReal()*radianConversionFactor)));
        }

        return Result(std::atanh(1/args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Erf : public Operation {
public:
    Erf() {
    }
    std::string getName() const {
        return "erf";
    }
    Id getId() const {
        return ERF;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Erf();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const;
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Erfc : public Operation {
public:
    Erfc() {
    }
    std::string getName() const {
        return "erfc";
    }
    Id getId() const {
        return ERFC;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Erfc();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const;
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Step : public Operation {
public:
    Step() {
    }
    std::string getName() const {
        return "step";
    }
    Id getId() const {
        return STEP;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Step();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with step");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with step");
        }

        return Result((args.inputs[0].getReal() >= 0.0 ? 1.0 : 0.0));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Delta : public Operation {
public:
    Delta() {
    }
    std::string getName() const {
        return "delta";
    }
    Id getId() const {
        return DELTA;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Delta();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with delta");
        }
        
        if (args.inputs[0].getImag() != 0) {
            throw Exception("Complex numbers not supported with delta");
        }

        return Result((args.inputs[0].getReal() == 0.0 ? 1.0 : 0.0));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Square : public Operation {
public:
    Square() {
    }
    std::string getName() const {
        return "square";
    }
    Id getId() const {
        return SQUARE;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Square();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with square");
        }
        return Result(args.inputs[0].getComplex()*args.inputs[0].getComplex());
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Cube : public Operation {
public:
    Cube() {
    }
    std::string getName() const {
        return "cube";
    }
    Id getId() const {
        return CUBE;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Cube();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with cube");
        }
        return Result(args.inputs[0].getComplex()*args.inputs[0].getComplex()*args.inputs[0].getComplex());
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Reciprocal : public Operation {
public:
    Reciprocal() {
    }
    std::string getName() const {
        return "recip";
    }
    Id getId() const {
        return RECIPROCAL;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Reciprocal();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with recip");
        }
        return Result(1.0/args.inputs[0].getComplex());
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::AddConstant : public Operation {
public:
    AddConstant(double value) : value(value) {
    }
    std::string getName() const {
        std::stringstream name;
        name << value << "+";
        return name.str();
    }
    Id getId() const {
        return ADD_CONSTANT;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new AddConstant(value);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {

        return Result(args.inputs[0].getComplex()+value);
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    double getValue() const {
        return value;
    }
    bool operator!=(const Operation& op) const {
        const AddConstant* o = dynamic_cast<const AddConstant*>(&op);
        return (o == NULL || o->value != value);
    }
private:
    double value;
};

class LEPTON_EXPORT Operation::MultiplyConstant : public Operation {
public:
    MultiplyConstant(double value) : value(value) {
    }
    std::string getName() const {
        std::stringstream name;
        name << value << "*";
        return name.str();
    }
    Id getId() const {
        return MULTIPLY_CONSTANT;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new MultiplyConstant(value);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {

        return Result(args.inputs[0].getComplex()*value);
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    double getValue() const {
        return value;
    }
    bool operator!=(const Operation& op) const {
        const MultiplyConstant* o = dynamic_cast<const MultiplyConstant*>(&op);
        return (o == NULL || o->value != value);
    }
private:
    double value;
};

class LEPTON_EXPORT Operation::PowerConstant : public Operation {
public:
    PowerConstant(double value) : value(value) {
        intValue = (int) value;
        isIntPower = (intValue == value);
    }
    std::string getName() const {
        std::stringstream name;
        name << "^" << value;
        return name.str();
    }
    Id getId() const {
        return POWER_CONSTANT;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new PowerConstant(value);
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {

        if (isIntPower && args.inputs[0].getImag() == 0) {
            // Integer powers can be computed much more quickly by repeated multiplication.
            
            int exponent = intValue;
            double base = args.inputs[0].getReal();
            if (exponent < 0) {
                exponent = -exponent;
                base = 1.0/base;
            }
            double result = 1.0;
            while (exponent != 0) {
                if ((exponent&1) == 1)
                    result *= base;
                base *= base;
                exponent = exponent>>1;
           }
           return Result(result);
        }
        else
        return Result(std::pow(args.inputs[0].getComplex(), value));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
    double getValue() const {
        return value;
    }
    bool operator!=(const Operation& op) const {
        const PowerConstant* o = dynamic_cast<const PowerConstant*>(&op);
        return (o == NULL || o->value != value);
    }
    bool isInfixOperator() const {
        return true;
    }
private:
    double value;
    int intValue;
    bool isIntPower;
};

class LEPTON_EXPORT Operation::Min : public Operation {
public:
    Min() {
    }
    std::string getName() const {
        return "min";
    }
    Id getId() const {
        return MIN;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new Min();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0]) || isMatrix(args.inputs[1])) {
            throw Exception("Matrices are not supported with min");
        }
        
        if (args.inputs[0].getImag() == 0 && args.inputs[1].getImag() == 0) {
            // parens around (std::min) are workaround for horrible microsoft max/min macro trouble
            return Result((std::min)(args.inputs[0].getReal(), args.inputs[1].getReal()));
        }

        return Result(abs(args.inputs[0].getComplex()) < abs(args.inputs[1].getComplex()) ? args.inputs[0].getComplex() : args.inputs[1].getComplex());

    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Max : public Operation {
public:
    Max() {
    }
    std::string getName() const {
        return "max";
    }
    Id getId() const {
        return MAX;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new Max();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0]) || isMatrix(args.inputs[1])) {
            throw Exception("Matrices are not supported with max");
        }
        
        if (args.inputs[0].getImag() == 0 && args.inputs[1].getImag() == 0) {
            // parens around (std::min) are workaround for horrible microsoft max/min macro trouble
            return Result((std::max)(args.inputs[0].getReal(), args.inputs[1].getReal()));
        }
        return Result(abs(args.inputs[0].getComplex()) > abs(args.inputs[1].getComplex()) ? args.inputs[0].getComplex() : args.inputs[1].getComplex());
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Abs : public Operation {
public:
    Abs() {
    }
    std::string getName() const {
        return "abs";
    }
    Id getId() const {
        return ABS;
    }
    int getNumArguments() const {
        return 1;
    }
    Operation* clone() const {
        return new Abs();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0])) {
            throw Exception("Matrices are not supported with abs");
        }
        
        if (args.inputs[0].getImag() != 0) {
            return Result(abs(args.inputs[0].getComplex()));
        }
        return Result(std::abs(args.inputs[0].getReal()));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

class LEPTON_EXPORT Operation::Rand : public Operation {
public:
    Rand() {
    }
    std::string getName() const {
        return "RNG";
    }
    Id getId() const {
        return RAND;
    }
    int getNumArguments() const {
        return 2;
    }
    Operation* clone() const {
        return new Rand();
    }
    Result evaluate(Args& args, const std::map<std::string, double>& variables) const {
        if (isMatrix(args.inputs[0]) || isMatrix(args.inputs[1])) {
            throw Exception("Matrices are not supported with RNG");
        }
        
        if (args.inputs[0].getImag() != 0 || args.inputs[1].getImag() != 0) {
            throw Exception("Complex numbers not supported with RNG");
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        // set min and max
        std::uniform_int_distribution<> dist(args.inputs[0].getReal(), args.inputs[1].getReal());
        return Result(dist(gen));
    }
    ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const;
};

} // namespace Lepton

#endif /*LEPTON_OPERATION_H_*/
