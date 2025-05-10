//
// Created by kir on 09.05.2025.
//

#ifndef LEXER_H
#define LEXER_H
#include "Enums.h"
#include "Error.h"
#include "LexerToken.h"

#include <math.h>
#include <string.h>

typedef struct tag_Lexer {
  const char *source;
  char current_char;
  size_t current_position;
  size_t length;
} Lexer;

Lexer Lexer_init(const char *source);

void Lexer_reset(Lexer *lexer);

ErrorType Lexer_next_token(Lexer *lexer, LexerToken *token, Error *error);

#endif // LEXER_H
