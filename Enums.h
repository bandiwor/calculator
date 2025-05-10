//
// Created by kir on 09.05.2025.
//

#ifndef ENUMS_H
#define ENUMS_H
#include "EnumTypeMaps.h"

typedef enum {
#define X(name, value) name,
  ERROR_TYPE_MAP(X)
#undef X
      ERROR_TYPE_COUNT
} ErrorType;

typedef enum {
#define X(name, value) name,
  LEXER_TOKEN_MAP(X)
#undef X
      LEXER_TOKEN_COUNT
} LexerTokenType;

typedef enum {
#define X(name, text) name,
  AST_NODE_TYPE_MAP(X)
#undef X
      AST_NODE_COUNT
} ASTNodeType;

typedef enum {
#define X(name, text) name,
  AST_BINARY_NODE_TYPE_MAP(X)
#undef X
      AST_BINARY_NODE_COUNT
} ASTBinaryNodeType;

typedef enum {
#define X(name, text) name,
  AST_UNARY_NODE_TYPE_MAP(X)
#undef X
      AST_UNARY_NODE_COUNT
} ASTUnaryNodeType;

const char *ErrorType_to_string(ErrorType error_type);

const char *LexerTokenType_to_string(LexerTokenType token_type);

const char *ASTNodeType_to_string(ASTNodeType node_type);

const char *ASTBinaryNodeType_to_string(ASTBinaryNodeType node_type);

const char *ASTUnaryNodeType_to_string(ASTUnaryNodeType node_type);

#endif // ENUMS_H
