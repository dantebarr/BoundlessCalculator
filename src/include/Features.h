#pragma once
#include <string>
#include <map>

#define SIN (1ULL << 0)
#define ASIN (1ULL << 1)
#define SINH (1ULL << 2)
#define ASINH (1ULL << 3)
#define COS (1ULL << 4)
#define ACOS (1ULL << 5)
#define COSH (1ULL << 6)
#define ACOSH (1ULL << 7)
#define TAN (1ULL << 8)
#define ATAN (1ULL << 9)
#define TANH (1ULL << 10)
#define ATANH (1ULL << 11)
#define CSC (1ULL << 12)
#define ACSC (1ULL << 13)
#define CSCH (1ULL << 14)
#define ACSCH (1ULL << 15)
#define SEC (1ULL << 16)
#define ASEC (1ULL << 17)
#define SECH (1ULL << 18)
#define ASECH (1ULL << 19)
#define COT (1ULL << 20)
#define ACOT (1ULL << 21)
#define COTH (1ULL << 22)
#define ACOTH (1ULL << 23)
#define LOG (1ULL << 24)
#define LN (1ULL << 25)
#define NPR (1ULL << 26)
#define NCR (1ULL << 27)
#define RAND (1ULL << 28)
#define GCD (1ULL << 29)
#define LCM (1ULL << 30)
#define FNINT (1ULL << 31)
#define SIGMA (1ULL << 32)
#define PROD (1ULL << 33)
#define AND (1ULL << 34)
#define OR (1ULL << 35)
#define NOT (1ULL << 36)
#define XOR (1ULL << 37)
#define LOGICAL_LEFT_SHIFT (1ULL << 38)
#define LOGICAL_RIGHT_SHIFT (1ULL << 39)
#define TRANSPOSE (1ULL << 40)
#define DET (1ULL << 41)
#define DOT (1ULL << 42)
#define CROSS (1ULL << 43)
#define SOLVE_SOLE (1ULL << 44)
#define MATRIX (1ULL << 45)
#define VARIABLE (1ULL << 46)
#define COMPLEX_NUMBER (1ULL << 47)
#define GRAPH (1ULL << 48)

const std::map<std::string, unsigned long long> featuresToBitMap = {
    {"sin", SIN},
    {"asin", ASIN},
    {"asinh", SINH},
    {"asinh", ASINH},
    {"cos", COS},
    {"acos", ACOS},
    {"cosh", COSH},
    {"acosh", ACOSH},
    {"tan", TAN},
    {"atan", ATAN},
    {"tanh", TANH},
    {"atanh", ATANH},
    {"csc", CSC},
    {"acsc", ACSC},
    {"csch", CSCH},
    {"acsch", ACSCH},
    {"sec", SEC},
    {"asec", ASEC},
    {"sech", SECH},
    {"asech", ASECH},
    {"cot", COT},
    {"acot", ACOT},
    {"coth", COTH},
    {"acoth", ACOTH},
    {"log", LOG},
    {"ln", LN},
    {"nPr", NPR},
    {"nCr", NCR},
    {"rand", RAND},
    {"gcd", GCD},
    {"lcm", LCM},
    {"fnInt", FNINT},
    {"sigma", SIGMA},
    {"prod", PROD},
    {"and", AND},
    {"or", OR},
    {"not", NOT},
    {"xor", XOR},
    {"lls", LOGICAL_LEFT_SHIFT},
    {"lrs", LOGICAL_RIGHT_SHIFT},
    {"transpose", TRANSPOSE},
    {"det", DET},
    {"dot", DOT},
    {"cross", CROSS},
    {"solveSOLE", SOLVE_SOLE},
    {"[A]", MATRIX},
    {"Disable variable", VARIABLE},
    {"Disable complex", COMPLEX_NUMBER},
    {"Disable graphing", GRAPH},
};

const std::map<unsigned long long, std::string> bitToFeaturesMap = {
    {SIN, "sin"},
    {ASIN, "asin"},
    {SINH, "sinh"},
    {ASINH, "asinh"},
    {COS, "cos"},
    {ACOS, "acos"},
    {COSH, "cos"},
    {ACOSH, "acosh"},
    {TAN, "tan"},
    {ATAN, "atan"},
    {TANH, "tanh"},
    {ATANH, "atanh"},
    {CSC, "csc"},
    {ACSC, "acsc"},
    {CSC, "csch"},
    {ACSCH, "acsch"},
    {SEC, "sec"},
    {ASEC, "asec"},
    {SECH, "sech"},
    {ASECH, "asech"},
    {COT, "cot"},
    {ACOT, "acot"},
    {COTH, "coth"},
    {ACOTH, "acoth"},
    {LOG, "log"},
    {LN, "ln"},
    {NPR, "nPr"},
    {NCR, "nCr"},
    {RAND, "rand"},
    {GCD, "gcd"},
    {LCM, "lcm"},
    {FNINT, "fnInt"},
    {SIGMA, "sigma"},
    {PROD, "prod"},
    {AND, "and"},
    {OR, "or"},
    {NOT, "not"},
    {XOR, "xor"},
    {LOGICAL_LEFT_SHIFT, "lls"},
    {LOGICAL_RIGHT_SHIFT, "lrs"},
    {TRANSPOSE, "transpose"},
    {DET, "det"},
    {DOT, "dot"},
    {CROSS, "cross"},
    {SOLVE_SOLE, "solveSOLE"},
    {MATRIX, "[A]"},
    {VARIABLE, "Disable variable"},
    {COMPLEX_NUMBER, "Disable complex"},
    {GRAPH, "Disable graphing"},
};
