//
// Created by kir on 09.05.2025.
//
#include "Error.h"

#include <stdio.h>

void Error_print(const Error *error) {
  const char *error_type = ErrorType_to_string(error->type);

  fprintf_s(stderr, "%s", error_type);
  if (error->what) {
    fprintf_s(stderr, ": %s\n", error->what);
  } else {
    fprintf_s(stderr, "\n");
  }

  switch (error->type) {
  case ERROR_TYPE_UNEXPECTED_SYMBOL:
    if (error->unexpected_symbol.symbol != '\0') {
      fprintf_s(stderr, "Symbol '%c'\n", error->unexpected_symbol.symbol);
    }
    break;
  case ERROR_TYPE_UNEXPECTED_TOKEN:
    if (error->unexpected_token.got != LEXER_TOKEN_IGNORE &&
        error->unexpected_token.expect) {
      fprintf_s(stderr, "Given token: '%s', but expected '%s'\n",
                LexerTokenType_to_string(error->unexpected_token.got),
                LexerTokenType_to_string(error->unexpected_token.expect));
      return;
        }
    if (error->unexpected_token.got != LEXER_TOKEN_IGNORE) {
      fprintf_s(stderr, "Given token: '%s'\n",
                LexerTokenType_to_string(error->unexpected_token.got));
      return;
    }
    break;
  default:
    break;
  }
}

