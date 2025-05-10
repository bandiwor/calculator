//
// Created by kir on 10.05.2025.
//
#include "LexerToken.h"

ErrorType LexerToken_create_identifier(LexerToken *token, const char *source) {
  token->type = LEXER_TOKEN_IDENTIFIER;
  token->identifier = strdup(source);
  if (token->identifier == NULL) {
    return ERROR_TYPE_MEMORY_ALLOCATION_FAILED;
  }

  return ERROR_TYPE_OK;
}

void LexerToken_free(const LexerToken *token) {
  if (token == NULL) {
    return;
  }
  if (token->type == LEXER_TOKEN_IDENTIFIER) {
    free(token->identifier);
  }
}
