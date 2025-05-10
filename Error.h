//
// Created by kir on 09.05.2025.
//

#ifndef ERROR_H
#define ERROR_H
#include "Enums.h"

typedef struct tag_Error {
  ErrorType type;
  char *what;

  union {
    struct {
      char symbol;
    } unexpected_symbol;

    struct {
      LexerTokenType expect;
      LexerTokenType got;
    } unexpected_token;

    struct {
      char *where;
      char *argument_name;
    } argument;

    struct {
      char *not_found_name;
    } name;

    struct {
      double argument;
    } not_defined_for_argument;
  };
} Error;

void Error_print(const Error *error);

#endif // ERROR_H
