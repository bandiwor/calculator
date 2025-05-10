//
// Created by kir on 09.05.2025.
//

#ifndef ENUMITEMSDEFINITIONS_H
#define ENUMITEMSDEFINITIONS_H

#define LEXER_TOKEN_MAP(X)                                                     \
  X(LEXER_TOKEN_IGNORE, "LEXER_TOKEN_IGNORE")                                  \
  X(LEXER_TOKEN_EOF, "<eof>")                                                  \
  X(LEXER_TOKEN_UNKNOWN, "LEXER_TOKEN_UNKNOWN")                                \
  X(LEXER_TOKEN_NUMBER, "number")                                              \
  X(LEXER_TOKEN_IDENTIFIER, "identifier")                                      \
  X(LEXER_TOKEN_LPAREN, "(")                                                   \
  X(LEXER_TOKEN_RPAREN, ")")                                                   \
  X(LEXER_TOKEN_ASSIGN, "=")                                                   \
  X(LEXER_TOKEN_PLUS, "+")                                                     \
  X(LEXER_TOKEN_MINUS, "-")                                                    \
  X(LEXER_TOKEN_MUL, "*")                                                      \
  X(LEXER_TOKEN_DIV, "/")                                                      \
  X(LEXER_TOKEN_INT_DIV, "//")                                                 \
  X(LEXER_TOKEN_MOD, "%")                                                      \
  X(LEXER_TOKEN_SQRT, "sqrt")                                                  \
  X(LEXER_TOKEN_POWER, "**")                                                   \
  X(LEXER_TOKEN_LG, "lg")                                                      \
  X(LEXER_TOKEN_LN, "ln")                                                      \
  X(LEXER_TOKEN_LOG, "log")                                                    \
  X(LEXER_TOKEN_ARC_SIN, "arcsin")                                             \
  X(LEXER_TOKEN_SIN, "sin")                                                    \
  X(LEXER_TOKEN_ARC_COS, "arccos")                                             \
  X(LEXER_TOKEN_COS, "cos")                                                    \
  X(LEXER_TOKEN_ARC_TAN, "arctan")                                             \
  X(LEXER_TOKEN_TAN, "tan")                                                    \
  X(LEXER_TOKEN_ARC_COT, "arccot")                                             \
  X(LEXER_TOKEN_COT, "cot")                                                    \
  X(LEXER_TOKEN_ABS, "abs")                                                    \
  X(LEXER_TOKEN_EXP, "exp")                                                    \
  X(LEXER_TOKEN_ARC_GAMMA, "arcgamma")                                         \
  X(LEXER_TOKEN_GAMMA, "gamma")                                                \
  X(LEXER_TOKEN_ARC_FACTORIAL, "arcfactorial")                                 \
  X(LEXER_TOKEN_FACTORIAL, "factorial")

#define AST_NODE_TYPE_MAP(X)                                                   \
  X(AST_NODE_UNKNOWN, "AST_NODE_UNKNOWN")                                      \
  X(AST_NODE_NUMBER, "number")                                                 \
  X(AST_NODE_VAR_REF, "var reference")                                         \
  X(AST_NODE_BINARY_OP, "binary op")                                           \
  X(AST_NODE_UNARY_OP, "unary op")

#define AST_UNARY_NODE_TYPE_MAP(X)                                             \
  X(AST_UNARY_UNKNOWN, "AST_UNARY_UNKNOWN")                                    \
  X(AST_UNARY_NEGATIVE, "negative")                                            \
  X(AST_UNARY_ABS, "abs")                                                      \
  X(AST_UNARY_SQRT, "sqrt")                                                    \
  X(AST_UNARY_LG, "lg")                                                        \
  X(AST_UNARY_ARC_SIN, "asin")                                                 \
  X(AST_UNARY_SIN, "sin")                                                      \
  X(AST_UNARY_ARC_COS, "acos")                                                 \
  X(AST_UNARY_COS, "cos")                                                      \
  X(AST_UNARY_ARC_TAN, "atan")                                                 \
  X(AST_UNARY_TAN, "tan")                                                      \
  X(AST_UNARY_ARC_COT, "cot")                                                  \
  X(AST_UNARY_COT, "cot")                                                      \
  X(AST_UNARY_LN, "ln")                                                        \
  X(AST_UNARY_EXP, "exp")                                                      \
  X(AST_UNARY_ARC_GAMMA, "arcgamma")                                           \
  X(AST_UNARY_GAMMA, "gamma")                                                  \
  X(AST_UNARY_ARC_FACTORIAL, "arcfactorial")                                   \
  X(AST_UNARY_FACTORIAL, "factorial")

#define AST_BINARY_NODE_TYPE_MAP(X)                                            \
  X(AST_BINARY_UNKNOWN, "AST_BINARY_UNKNOWN")                                  \
  X(AST_BINARY_ASSIGN, "assign")                                               \
  X(AST_BINARY_ADD, "add")                                                     \
  X(AST_BINARY_SUB, "sub")                                                     \
  X(AST_BINARY_MUL, "mul")                                                     \
  X(AST_BINARY_DIV, "div")                                                     \
  X(AST_BINARY_INT_DIV, "int div")                                             \
  X(AST_BINARY_MOD, "mod")                                                     \
  X(AST_BINARY_POWER, "power")                                                 \
  X(AST_BINARY_LOG, "log")

#define ERROR_TYPE_MAP(X)                                                      \
  X(ERROR_TYPE_OK, "No error")                                                 \
  X(ERROR_TYPE_UNKNOWN, "UnknownError")                                        \
  X(ERROR_TYPE_MEMORY_ALLOCATION_FAILED, "MemoryAllocationFailedError")        \
  X(ERROR_TYPE_INPUT_FAILED, "InputFailedError")                               \
  X(ERROR_TYPE_ARGUMENT, "ArgumentError")                                      \
  X(ERROR_TYPE_UNEXPECTED_TOKEN, "UnexpectedTokenError")                       \
  X(ERROR_TYPE_UNEXPECTED_SYMBOL, "UnexpectedSymbolError")                     \
  X(ERROR_TYPE_NAME, "NameError")                                              \
  X(ERROR_TYPE_ASSIGN, "AssignError")                                          \
  X(ERROR_TYPE_COTANGENS_IS_UNDEFINED, "CotangensIsUndefinedError")            \
  X(ERROR_TYPE_ARCGAMMA, "ArcgammaIsUndefinedError")                           \
  X(ERROR_TYPE_ARCFACTORIAL, "ArcfactorialIsUndefinedError")


#endif //ENUMITEMSDEFINITIONS_H
