//
// Created by kir on 09.05.2025.
//
#include "Enums.h"

const char *ErrorType_to_string(const ErrorType error_type) {
  static const char *error_type_string[] = {
#define X(name, value) [name] = (value),
    ERROR_TYPE_MAP(X)
#undef X
};

  if (error_type >= ERROR_TYPE_COUNT) {
    return error_type_string[ERROR_TYPE_UNKNOWN];
  }

  return error_type_string[error_type];
}

const char *LexerTokenType_to_string(const LexerTokenType token_type) {
#ifndef CALC_REMOVE_DEBUG_INFO
  static const char *token_type_string[] = {
#define X(name, value) [name] = (value),
    LEXER_TOKEN_MAP(X)
#undef X
};

  if (token_type >= LEXER_TOKEN_COUNT) {
    return token_type_string[LEXER_TOKEN_UNKNOWN];
  }

  return token_type_string[token_type];
#else
  return "";
#endif
}

const char *ASTNodeType_to_string(const ASTNodeType node_type) {
#ifndef CALC_REMOVE_DEBUG_INFO
  static const char *node_type_string[] = {
#define X(name, value) [name] = (value),
    AST_NODE_TYPE_MAP(X)
#undef X
};
  if (node_type >= AST_NODE_COUNT) {
    return node_type_string[AST_NODE_UNKNOWN];
  }

  return node_type_string[node_type];
#else
  return "";
#endif
}

const char *ASTBinaryNodeType_to_string(const ASTBinaryNodeType node_type) {
#ifndef CALC_REMOVE_DEBUG_INFO
  static const char *node_type_string[] = {
#define X(name, value) [name] = (value),
    AST_BINARY_NODE_TYPE_MAP(X)
#undef X
};

  if (node_type >= AST_BINARY_NODE_COUNT) {
    return node_type_string[AST_BINARY_UNKNOWN];
  }

  return node_type_string[node_type];
#else
  return "";
#endif
}

const char *ASTUnaryNodeType_to_string(const ASTUnaryNodeType node_type) {
#ifndef CALC_REMOVE_DEBUG_INFO
  static const char *node_type_string[] = {
#define X(name, value) [name] = (value),
    AST_UNARY_NODE_TYPE_MAP(X)
#undef X
};

  if (node_type >= AST_UNARY_NODE_COUNT) {
    return node_type_string[AST_UNARY_UNKNOWN];
  }

  return node_type_string[node_type];
#else
  return "";
#endif
}
