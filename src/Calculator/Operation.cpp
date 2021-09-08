
/* -------------------------------------------------------------------------- *
 *                                   Lepton                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the Lepton expression parser originating from              *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2009 Stanford University and the Authors.           *
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

#include "lepton/Operation.h"
#include "lepton/ExpressionTreeNode.h"
#include <complex>

using namespace Lepton;
using namespace std;

Result Operation::Erf::evaluate(Args& args, const map<string, double>& variables) const {
    if (args.inputs[0].getImag() != 0) {
        throw Exception("Complex numbers not supported with Erf");
    }
    return erf(args.inputs[0].getReal());
}

Result Operation::Erfc::evaluate(Args& args, const map<string, double>& variables) const {
    if (args.inputs[0].getImag() != 0) {
        throw Exception("Complex numbers not supported with Erfc");
    }
    return erfc(args.inputs[0].getReal());
}

ExpressionTreeNode Operation::ComplexNumber::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Constant(0.0));
}

ExpressionTreeNode Operation::Constant::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Constant(0.0));
}

ExpressionTreeNode Operation::Variable::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    if (variable == name)
        return ExpressionTreeNode(new Operation::Constant(1.0));
    return ExpressionTreeNode(new Operation::Constant(0.0));
}

ExpressionTreeNode Operation::Matrix::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    // TO DO: modify this
    if (variable == name)
        return ExpressionTreeNode(new Operation::Constant(1.0));
    return ExpressionTreeNode(new Operation::Constant(0.0));
}

ExpressionTreeNode Operation::Transpose::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Transpose(), childDerivs[0]);
}

ExpressionTreeNode Operation::Det::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Det(), childDerivs[0]);
}

ExpressionTreeNode Operation::Dot::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Dot(), childDerivs[0], childDerivs[1]);
}

ExpressionTreeNode Operation::Cross::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Cross(), childDerivs[0], childDerivs[1]);
}

ExpressionTreeNode Operation::SolveSOLE::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::SolveSOLE(), childDerivs[0], childDerivs[1]);
}

ExpressionTreeNode Operation::Custom::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    if (function->getNumArguments() == 0)
        return ExpressionTreeNode(new Operation::Constant(0.0));
    ExpressionTreeNode result = ExpressionTreeNode(new Operation::Multiply(), ExpressionTreeNode(new Operation::Custom(*this, 0), children), childDerivs[0]);
    for (int i = 1; i < getNumArguments(); i++) {
        result = ExpressionTreeNode(new Operation::Add(),
                                    result,
                                    ExpressionTreeNode(new Operation::Multiply(), ExpressionTreeNode(new Operation::Custom(*this, i), children), childDerivs[i]));
    }
    return result;
}

ExpressionTreeNode Operation::And::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::And(), childDerivs[0], childDerivs[1]);
}

ExpressionTreeNode Operation::Or::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Or(), childDerivs[0], childDerivs[1]);
}

ExpressionTreeNode Operation::Not::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Not(), childDerivs[0]);
}

ExpressionTreeNode Operation::Xor::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Xor(), childDerivs[0], childDerivs[1]);
}

ExpressionTreeNode Operation::LogicalLeftShift::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::LogicalLeftShift(), childDerivs[0], childDerivs[1]);
}

ExpressionTreeNode Operation::LogicalRightShift::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::LogicalRightShift(), childDerivs[0], childDerivs[1]);
}

ExpressionTreeNode Operation::Add::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Add(), childDerivs[0], childDerivs[1]);
}

ExpressionTreeNode Operation::Subtract::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Subtract(), childDerivs[0], childDerivs[1]);
}

ExpressionTreeNode Operation::Multiply::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Add(),
                              ExpressionTreeNode(new Operation::Multiply(), children[0], childDerivs[1]),
                              ExpressionTreeNode(new Operation::Multiply(), children[1], childDerivs[0]));
}

ExpressionTreeNode Operation::Divide::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Divide(),
                              ExpressionTreeNode(new Operation::Subtract(),
                                                 ExpressionTreeNode(new Operation::Multiply(), children[1], childDerivs[0]),
                                                 ExpressionTreeNode(new Operation::Multiply(), children[0], childDerivs[1])),
                              ExpressionTreeNode(new Operation::Square(), children[1]));
}

ExpressionTreeNode Operation::Power::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Add(),
                              ExpressionTreeNode(new Operation::Multiply(),
                                                 ExpressionTreeNode(new Operation::Multiply(),
                                                                    children[1],
                                                                    ExpressionTreeNode(new Operation::Power(),
                                                                                       children[0], ExpressionTreeNode(new Operation::AddConstant(-1.0), children[1]))),
                                                 childDerivs[0]),
                              ExpressionTreeNode(new Operation::Multiply(),
                                                 ExpressionTreeNode(new Operation::Multiply(),
                                                                    ExpressionTreeNode(new Operation::Ln(), children[0]),
                                                                    ExpressionTreeNode(new Operation::Power(), children[0], children[1])),
                                                 childDerivs[1]));
}

ExpressionTreeNode Operation::Modulus::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Modulus(), childDerivs[0], childDerivs[1]);
}

ExpressionTreeNode Operation::Negate::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Negate(), childDerivs[0]);
}

ExpressionTreeNode Operation::Sqrt::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::MultiplyConstant(0.5),
                                                 ExpressionTreeNode(new Operation::Reciprocal(),
                                                                    ExpressionTreeNode(new Operation::Sqrt(), children[0]))),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Exp::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Exp(), children[0]),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Log::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Reciprocal(), children[0]),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Ln::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Reciprocal(), children[0]),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Factorial::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    // TO DO: compute the correct derivate of a factorial. Set arbitary derivative for now to speed up development.
    return ExpressionTreeNode(new Operation::Constant(0.0));

}

ExpressionTreeNode Operation::Gcd::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    // TO DO: compute the correct derivate of a gcd. Set arbitary derivative for now to speed up development.
    return ExpressionTreeNode(new Operation::Constant(0.0));

}

ExpressionTreeNode Operation::Lcm::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    // TO DO: compute the correct derivate of a lcm. Set arbitary derivative for now to speed up development.
    return ExpressionTreeNode(new Operation::Constant(0.0));

}


ExpressionTreeNode Operation::FnInt::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    // TO DO: compute the correct derivate of a FnInt. Set arbitary derivative for now to speed up development.
    return ExpressionTreeNode(new Operation::Constant(0.0));

}

ExpressionTreeNode Operation::Sigma::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    // TO DO: compute the correct derivate of a Sigma. Set arbitary derivative for now to speed up development.
    return ExpressionTreeNode(new Operation::Constant(0.0));

}

ExpressionTreeNode Operation::Prod::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    // TO DO: compute the correct derivate of a Prod. Set arbitary derivative for now to speed up development.
    return ExpressionTreeNode(new Operation::Constant(0.0));

}

ExpressionTreeNode Operation::Npr::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    // TO DO: compute the correct derivate of NPR. Set arbitary derivative for now to speed up development.
    return ExpressionTreeNode(new Operation::Constant(0.0));

}

ExpressionTreeNode Operation::Ncr::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    // TO DO: compute the correct derivate of NCR. Set arbitary derivative for now to speed up development.
    return ExpressionTreeNode(new Operation::Constant(0.0));

}

ExpressionTreeNode Operation::Sin::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Cos(), children[0]),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Cos::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Negate(),
                                                 ExpressionTreeNode(new Operation::Sin(), children[0])),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Sec::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Multiply(),
                                                 ExpressionTreeNode(new Operation::Sec(), children[0]),
                                                 ExpressionTreeNode(new Operation::Tan(), children[0])),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Csc::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Negate(),
                                                 ExpressionTreeNode(new Operation::Multiply(),
                                                                    ExpressionTreeNode(new Operation::Csc(), children[0]),
                                                                    ExpressionTreeNode(new Operation::Cot(), children[0]))),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Tan::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Square(),
                                                 ExpressionTreeNode(new Operation::Sec(), children[0])),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Cot::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Negate(),
                                                 ExpressionTreeNode(new Operation::Square(),
                                                                    ExpressionTreeNode(new Operation::Csc(), children[0]))),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Asin::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Reciprocal(),
                                                 ExpressionTreeNode(new Operation::Sqrt(),
                                                                    ExpressionTreeNode(new Operation::Subtract(),
                                                                                       ExpressionTreeNode(new Operation::Constant(1.0)),
                                                                                       ExpressionTreeNode(new Operation::Square(), children[0])))),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Acos::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Negate(),
                                                 ExpressionTreeNode(new Operation::Reciprocal(),
                                                                    ExpressionTreeNode(new Operation::Sqrt(),
                                                                                       ExpressionTreeNode(new Operation::Subtract(),
                                                                                                          ExpressionTreeNode(new Operation::Constant(1.0)),
                                                                                                          ExpressionTreeNode(new Operation::Square(), children[0]))))),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Atan::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Reciprocal(),
                                                 ExpressionTreeNode(new Operation::AddConstant(1.0),
                                                                    ExpressionTreeNode(new Operation::Square(), children[0]))),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Sinh::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Cosh(),
                                                 children[0]),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Cosh::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Sinh(),
                                                 children[0]),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Tanh::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Subtract(),
                                                 ExpressionTreeNode(new Operation::Constant(1.0)),
                                                 ExpressionTreeNode(new Operation::Square(),
                                                                    ExpressionTreeNode(new Operation::Tanh(), children[0]))),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Asinh::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Reciprocal(),
                                                  ExpressionTreeNode(new Operation::Sqrt(),
                                                                     ExpressionTreeNode(new Operation::AddConstant(1.0),
                                                                     ExpressionTreeNode(new Operation::Square(), children[0])))),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Acosh::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(), 
                              ExpressionTreeNode(new Operation::Reciprocal(),
                                                 ExpressionTreeNode(new Operation::Sqrt(),
                                                                    ExpressionTreeNode(new Operation::Subtract(),
                                                                                       ExpressionTreeNode(new Operation::Square(), children[0]),
                                                                                       ExpressionTreeNode(new Operation::Constant(1.0))))),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Atanh::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Reciprocal(),
                                                 ExpressionTreeNode(new Operation::Subtract(),
                                                                    ExpressionTreeNode(new Operation::Constant(1.0)),
                                                                    ExpressionTreeNode(new Operation::Square(), children[0]))),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Acsc::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(), 
                              ExpressionTreeNode(new Operation::Negate(),
                                                 ExpressionTreeNode(new Operation::Reciprocal(),
                                                                    ExpressionTreeNode(new Operation::Multiply(),
                                                                                       ExpressionTreeNode(new Operation::Square(), children[0]),
                                                                                       ExpressionTreeNode(new Operation::Sqrt(),
                                                                                                          ExpressionTreeNode(new Operation::Subtract(),
                                                                                                                             ExpressionTreeNode(new Operation::Constant(1.0)),
                                                                                                                             ExpressionTreeNode(new Operation::Reciprocal(),
                                                                                                                                                new Operation::Square(), children[0])))))),
                              childDerivs[0]);
}

// TO DO: correct this
ExpressionTreeNode Operation::Csch::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(), 
                              ExpressionTreeNode(new Operation::Negate(),
                                                 ExpressionTreeNode(new Operation::Reciprocal(),
                                                                    ExpressionTreeNode(new Operation::Multiply(),
                                                                                       ExpressionTreeNode(new Operation::Square(), children[0]),
                                                                                       ExpressionTreeNode(new Operation::Sqrt(),
                                                                                                          ExpressionTreeNode(new Operation::Subtract(),
                                                                                                                             ExpressionTreeNode(new Operation::Constant(1.0)),
                                                                                                                             ExpressionTreeNode(new Operation::Reciprocal(),
                                                                                                                                                new Operation::Square(), children[0])))))),
                              childDerivs[0]);
}

// TO DO: correct this
ExpressionTreeNode Operation::Coth::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(), 
                              ExpressionTreeNode(new Operation::Negate(),
                                                 ExpressionTreeNode(new Operation::Reciprocal(),
                                                                    ExpressionTreeNode(new Operation::Multiply(),
                                                                                       ExpressionTreeNode(new Operation::Square(), children[0]),
                                                                                       ExpressionTreeNode(new Operation::Sqrt(),
                                                                                                          ExpressionTreeNode(new Operation::Subtract(),
                                                                                                                             ExpressionTreeNode(new Operation::Constant(1.0)),
                                                                                                                             ExpressionTreeNode(new Operation::Reciprocal(),
                                                                                                                                                new Operation::Square(), children[0])))))),
                              childDerivs[0]);
}

// TO DO: correct this
ExpressionTreeNode Operation::Sech::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(), 
                              ExpressionTreeNode(new Operation::Negate(),
                                                 ExpressionTreeNode(new Operation::Reciprocal(),
                                                                    ExpressionTreeNode(new Operation::Multiply(),
                                                                                       ExpressionTreeNode(new Operation::Square(), children[0]),
                                                                                       ExpressionTreeNode(new Operation::Sqrt(),
                                                                                                          ExpressionTreeNode(new Operation::Subtract(),
                                                                                                                             ExpressionTreeNode(new Operation::Constant(1.0)),
                                                                                                                             ExpressionTreeNode(new Operation::Reciprocal(),
                                                                                                                                                new Operation::Square(), children[0])))))),
                              childDerivs[0]);
}

// TO DO: correct this
ExpressionTreeNode Operation::Asec::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(), 
                              ExpressionTreeNode(new Operation::Negate(),
                                                 ExpressionTreeNode(new Operation::Reciprocal(),
                                                                    ExpressionTreeNode(new Operation::Multiply(),
                                                                                       ExpressionTreeNode(new Operation::Square(), children[0]),
                                                                                       ExpressionTreeNode(new Operation::Sqrt(),
                                                                                                          ExpressionTreeNode(new Operation::Subtract(),
                                                                                                                             ExpressionTreeNode(new Operation::Constant(1.0)),
                                                                                                                             ExpressionTreeNode(new Operation::Reciprocal(),
                                                                                                                                                new Operation::Square(), children[0])))))),
                              childDerivs[0]);
}

// TO DO: correct this
ExpressionTreeNode Operation::Acot::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(), 
                              ExpressionTreeNode(new Operation::Negate(),
                                                 ExpressionTreeNode(new Operation::Reciprocal(),
                                                                    ExpressionTreeNode(new Operation::Multiply(),
                                                                                       ExpressionTreeNode(new Operation::Square(), children[0]),
                                                                                       ExpressionTreeNode(new Operation::Sqrt(),
                                                                                                          ExpressionTreeNode(new Operation::Subtract(),
                                                                                                                             ExpressionTreeNode(new Operation::Constant(1.0)),
                                                                                                                             ExpressionTreeNode(new Operation::Reciprocal(),
                                                                                                                                                new Operation::Square(), children[0])))))),
                              childDerivs[0]);
}

// TO DO: correct this
ExpressionTreeNode Operation::Acsch::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(), 
                              ExpressionTreeNode(new Operation::Negate(),
                                                 ExpressionTreeNode(new Operation::Reciprocal(),
                                                                    ExpressionTreeNode(new Operation::Multiply(),
                                                                                       ExpressionTreeNode(new Operation::Square(), children[0]),
                                                                                       ExpressionTreeNode(new Operation::Sqrt(),
                                                                                                          ExpressionTreeNode(new Operation::Subtract(),
                                                                                                                             ExpressionTreeNode(new Operation::Constant(1.0)),
                                                                                                                             ExpressionTreeNode(new Operation::Reciprocal(),
                                                                                                                                                new Operation::Square(), children[0])))))),
                              childDerivs[0]);
}

// TO DO: correct this
ExpressionTreeNode Operation::Asech::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(), 
                              ExpressionTreeNode(new Operation::Negate(),
                                                 ExpressionTreeNode(new Operation::Reciprocal(),
                                                                    ExpressionTreeNode(new Operation::Multiply(),
                                                                                       ExpressionTreeNode(new Operation::Square(), children[0]),
                                                                                       ExpressionTreeNode(new Operation::Sqrt(),
                                                                                                          ExpressionTreeNode(new Operation::Subtract(),
                                                                                                                             ExpressionTreeNode(new Operation::Constant(1.0)),
                                                                                                                             ExpressionTreeNode(new Operation::Reciprocal(),
                                                                                                                                                new Operation::Square(), children[0])))))),
                              childDerivs[0]);
}

// TO DO: correct this
ExpressionTreeNode Operation::Acoth::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(), 
                              ExpressionTreeNode(new Operation::Negate(),
                                                 ExpressionTreeNode(new Operation::Reciprocal(),
                                                                    ExpressionTreeNode(new Operation::Multiply(),
                                                                                       ExpressionTreeNode(new Operation::Square(), children[0]),
                                                                                       ExpressionTreeNode(new Operation::Sqrt(),
                                                                                                          ExpressionTreeNode(new Operation::Subtract(),
                                                                                                                             ExpressionTreeNode(new Operation::Constant(1.0)),
                                                                                                                             ExpressionTreeNode(new Operation::Reciprocal(),
                                                                                                                                                new Operation::Square(), children[0])))))),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Erf::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Multiply(),
                                                 ExpressionTreeNode(new Operation::Constant(2.0/sqrt(M_PI))),
                                                 ExpressionTreeNode(new Operation::Exp(),
                                                                    ExpressionTreeNode(new Operation::Negate(),
                                                                                       ExpressionTreeNode(new Operation::Square(), children[0])))),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Erfc::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Multiply(),
                                                 ExpressionTreeNode(new Operation::Constant(-2.0/sqrt(M_PI))),
                                                 ExpressionTreeNode(new Operation::Exp(),
                                                                    ExpressionTreeNode(new Operation::Negate(),
                                                                                       ExpressionTreeNode(new Operation::Square(), children[0])))),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Step::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Constant(0.0));
}

ExpressionTreeNode Operation::Delta::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Constant(0.0));
}

ExpressionTreeNode Operation::Square::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::MultiplyConstant(2.0),
                                                 children[0]),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Cube::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::MultiplyConstant(3.0),
                                                 ExpressionTreeNode(new Operation::Square(), children[0])),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Reciprocal::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::Negate(),
                                                 ExpressionTreeNode(new Operation::Reciprocal(),
                                                                    ExpressionTreeNode(new Operation::Square(), children[0]))),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::AddConstant::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return childDerivs[0];
}

ExpressionTreeNode Operation::MultiplyConstant::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::MultiplyConstant(value),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::PowerConstant::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Multiply(),
                              ExpressionTreeNode(new Operation::MultiplyConstant(value),
                                                 ExpressionTreeNode(new Operation::PowerConstant(value-1),
                                                                    children[0])),
                              childDerivs[0]);
}

ExpressionTreeNode Operation::Min::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    ExpressionTreeNode step(new Operation::Step(),
                            ExpressionTreeNode(new Operation::Subtract(), children[0], children[1]));
    return ExpressionTreeNode(new Operation::Subtract(),
                              ExpressionTreeNode(new Operation::Multiply(), childDerivs[1], step),
                              ExpressionTreeNode(new Operation::Multiply(), childDerivs[0],
                                                 ExpressionTreeNode(new Operation::AddConstant(-1), step)));
}

ExpressionTreeNode Operation::Max::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    ExpressionTreeNode step(new Operation::Step(),
                            ExpressionTreeNode(new Operation::Subtract(), children[0], children[1]));
    return ExpressionTreeNode(new Operation::Subtract(),
                              ExpressionTreeNode(new Operation::Multiply(), childDerivs[0], step),
                              ExpressionTreeNode(new Operation::Multiply(), childDerivs[1],
                                                 ExpressionTreeNode(new Operation::AddConstant(-1), step)));
}

ExpressionTreeNode Operation::Abs::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    ExpressionTreeNode step(new Operation::Step(), children[0]);
    return ExpressionTreeNode(new Operation::Multiply(),
                              childDerivs[0],
                              ExpressionTreeNode(new Operation::AddConstant(-1),
                                                 ExpressionTreeNode(new Operation::MultiplyConstant(2), step)));
}

ExpressionTreeNode Operation::Rand::differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs, const std::string& variable) const {
    return ExpressionTreeNode(new Operation::Rand(), childDerivs[0], childDerivs[1]);
}
