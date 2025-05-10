//
// Created by kir on 10.05.2025.
//

#ifndef PARSER_H
#define PARSER_H
#include "ASTNodePull.h"
#include "Enums.h"
#include "Lexer.h"

typedef struct {
  Lexer *lexer;
  ASTNodePull node_pull;
  LexerToken current_token;
} Parser;

ErrorType Parser_init(Parser *parser, Lexer *lexer, size_t node_pull_capacity,
                      Error *error);

ErrorType Parser_reset(Parser *parser, Lexer *lexer, Error *error);

ErrorType Parser_parse(Parser *parser, ASTNode **node, Error *error);

#endif // PARSER_H
