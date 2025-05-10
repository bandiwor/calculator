//
// Created by kir on 10.05.2025.
//

#ifndef LEXERTOKEN_H
#define LEXERTOKEN_H
#include "Enums.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
  LexerTokenType type;

  union {
    double number;
    char *identifier;
  };
} LexerToken;

ErrorType LexerToken_create_identifier(LexerToken *token, const char *source);

void LexerToken_free(const LexerToken *token);

#endif // LEXERTOKEN_H
