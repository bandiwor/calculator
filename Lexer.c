//
// Created by kir on 09.05.2025.
//
#include "Lexer.h"

#include <ctype.h>
#include <stdbool.h>

Lexer Lexer_init(const char *source) {
  if (source == NULL) {
    return (Lexer){
        .source = NULL,
        .current_char = 0,
        .current_position = 0,
        .length = 0,
    };
  }
  const size_t length = strlen(source);

  return (Lexer){
      .source = source,
      .current_char = length > 0 ? source[0] : '\0',
      .current_position = 0,
      .length = length,
  };
}

void Lexer_reset(Lexer *lexer) {
  lexer->current_position = 0;
  lexer->current_char = lexer->length > 0 ? lexer->source[0] : '\0';
}

void Lexer_advance(Lexer *lexer) {
  if (lexer->current_position + 1 <= lexer->length) {
    ++lexer->current_position;
    lexer->current_char = lexer->source[lexer->current_position];
  } else {
    lexer->current_char = '\0';
  }
}

void Lexer_parse_number(Lexer *lexer, LexerToken *token) {
  static char buffer[128];
#define MAX_BUFFER_SIZE (sizeof(buffer) - 1)

  buffer[0] = lexer->current_char;
  size_t buffer_length = 1;
  Lexer_advance(lexer);
  bool is_int = true;

  while (buffer_length < MAX_BUFFER_SIZE) {
    if (lexer->current_char >= '0' && lexer->current_char <= '9') {
    } else if (lexer->current_char == '.' && is_int) {
      is_int = false;
    } else {
      break;
    }

    buffer[buffer_length++] = lexer->current_char;
    Lexer_advance(lexer);
  }
  buffer[buffer_length] = '\0';

  token->type = LEXER_TOKEN_NUMBER;
  token->number = strtod(buffer, NULL);
#undef MAX_BUFFER_SIZE
}

ErrorType Lexer_parse_identifier(Lexer *lexer, LexerToken *token) {
  static char buffer[16];
#define MAX_BUFFER_SIZE (sizeof(buffer) - 1)

  buffer[0] = lexer->current_char;
  size_t buffer_length = 1;
  Lexer_advance(lexer);

  while (isalnum(lexer->current_char) && buffer_length < MAX_BUFFER_SIZE) {
    buffer[buffer_length++] = lexer->current_char;
    Lexer_advance(lexer);
  }

  buffer[buffer_length] = '\0';

#define IDENTIFIER_COMPARE(name, matched_type)                                 \
  if (strcasecmp(buffer, name) == 0) {                                         \
    token->type = matched_type;                                                \
    return ERROR_TYPE_OK;                                                      \
  }

  IDENTIFIER_COMPARE("sqrt", LEXER_TOKEN_SQRT)
  IDENTIFIER_COMPARE("lg", LEXER_TOKEN_LG)
  IDENTIFIER_COMPARE("ln", LEXER_TOKEN_LN)
  IDENTIFIER_COMPARE("log", LEXER_TOKEN_LOG)
  IDENTIFIER_COMPARE("sin", LEXER_TOKEN_SIN)
  IDENTIFIER_COMPARE("cos", LEXER_TOKEN_COS)
  IDENTIFIER_COMPARE("tan", LEXER_TOKEN_TAN)
  IDENTIFIER_COMPARE("tg", LEXER_TOKEN_TAN)
  IDENTIFIER_COMPARE("cot", LEXER_TOKEN_COT)
  IDENTIFIER_COMPARE("ctg", LEXER_TOKEN_COT)
  IDENTIFIER_COMPARE("abs", LEXER_TOKEN_ABS)
  IDENTIFIER_COMPARE("exp", LEXER_TOKEN_EXP)
  IDENTIFIER_COMPARE("gamma", LEXER_TOKEN_GAMMA)
  IDENTIFIER_COMPARE("factorial", LEXER_TOKEN_FACTORIAL)
  IDENTIFIER_COMPARE("fac", LEXER_TOKEN_FACTORIAL)
  IDENTIFIER_COMPARE("arcsin", LEXER_TOKEN_ARC_SIN)
  IDENTIFIER_COMPARE("asin", LEXER_TOKEN_ARC_SIN)
  IDENTIFIER_COMPARE("arccos", LEXER_TOKEN_ARC_COS)
  IDENTIFIER_COMPARE("acos", LEXER_TOKEN_ARC_COS)
  IDENTIFIER_COMPARE("arctan", LEXER_TOKEN_ARC_TAN)
  IDENTIFIER_COMPARE("arctg", LEXER_TOKEN_ARC_TAN)
  IDENTIFIER_COMPARE("arccot", LEXER_TOKEN_ARC_COT)
  IDENTIFIER_COMPARE("arcctg", LEXER_TOKEN_ARC_COT)
  IDENTIFIER_COMPARE("arcgamma", LEXER_TOKEN_ARC_GAMMA)
  IDENTIFIER_COMPARE("arcfactorial", LEXER_TOKEN_ARC_FACTORIAL)
  IDENTIFIER_COMPARE("arcfac", LEXER_TOKEN_ARC_FACTORIAL)
#undef IDENTIFIER_COMPARE

  if (strcasecmp(buffer, "pi") == 0) {
    token->type = LEXER_TOKEN_NUMBER;
    token->number = M_PI;
    return ERROR_TYPE_OK;
  }
  if (strcasecmp(buffer, "p") == 0) {
    token->type = LEXER_TOKEN_NUMBER;
    token->number = M_PI;
    return ERROR_TYPE_OK;
  }
  if (strcasecmp(buffer, "e") == 0) {
    token->type = LEXER_TOKEN_NUMBER;
    token->number = M_E;
    return ERROR_TYPE_OK;
  }

  ErrorType error_type;
  if ((error_type = LexerToken_create_identifier(token, buffer)) !=
      ERROR_TYPE_OK) {
    return error_type;
  }

  return ERROR_TYPE_OK;
#undef MAX_BUFFER_SIZE
}

char Lexer_next_char(const Lexer *lexer) {
  if (lexer->current_position + 1 < lexer->length) {
    return lexer->source[lexer->current_position + 1];
  }
  return '\0';
}

inline int is_space(const int ch) {
  return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t' || ch == '\f' ||
         ch == '\v';
}

ErrorType Lexer_next_token(Lexer *lexer, LexerToken *token, Error *error) {
  if (lexer == NULL) {
    token->type = LEXER_TOKEN_EOF;
    return ERROR_TYPE_OK;
  }
  if (lexer->current_position + 1 > lexer->length) {
    token->type = LEXER_TOKEN_EOF;
    return ERROR_TYPE_OK;
  }
  while (is_space(lexer->current_char)) {
    Lexer_advance(lexer);
  }

  if (lexer->current_char >= '0' && lexer->current_char <= '9') {
    Lexer_parse_number(lexer, token);
    return ERROR_TYPE_OK;
  }

  if (isalpha(lexer->current_char)) {
    return Lexer_parse_identifier(lexer, token);
  }

  const char current_char = lexer->current_char;
  const char next_char = Lexer_next_char(lexer);
  Lexer_advance(lexer);
  LexerTokenType type;

  switch (current_char) {
  case '+':
    type = LEXER_TOKEN_PLUS;
    break;
  case '-':
    type = LEXER_TOKEN_MINUS;
    break;
  case '%':
    type = LEXER_TOKEN_MOD;
    break;
  case '=':
    type = LEXER_TOKEN_ASSIGN;
    break;
  case '(':
  case '[':
    type = LEXER_TOKEN_LPAREN;
    break;
  case ')':
  case ']':
    type = LEXER_TOKEN_RPAREN;
    break;
  case '*':
    if (next_char == '*') {
      type = LEXER_TOKEN_POWER;
      Lexer_advance(lexer);
      break;
    }
    type = LEXER_TOKEN_MUL;
    break;
  case '/':
    if (next_char == '/') {
      type = LEXER_TOKEN_INT_DIV;
      Lexer_advance(lexer);
      break;
    }
    type = LEXER_TOKEN_DIV;
    break;
  case '\0':
    token->type = LEXER_TOKEN_EOF;
    return ERROR_TYPE_OK;
  default:
    if (error != NULL) {
      error->type = ERROR_TYPE_UNEXPECTED_SYMBOL;
      error->unexpected_symbol.symbol = current_char;
    }
    return ERROR_TYPE_UNEXPECTED_SYMBOL;
  }

  token->type = type;
  return ERROR_TYPE_OK;
}
