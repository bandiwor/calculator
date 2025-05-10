// #define CALC_REMOVE_DEBUG_INFO // Comment Line if not debug

#include "AST.h"
#include "ASTEval.h"
#include "Error.h"
#include "Globals.h"
#include "Lexer.h"
#include "Parser.h"
#include "RBT.h"
#include "Utils.h"

#include <stdio.h>
#include <stdlib.h>

int main(const int argc, const char **argv) {
  Error error = {};

  if (RBT_allocate(&g_variables, &error) != ERROR_TYPE_OK) {
    Error_print(&error);
    return EXIT_FAILURE;
  }

  static char input_expression[1024] = "";
  if (argc > 1) {
    build_expression_from_argv(input_expression, sizeof(input_expression),
                               argc - 1, argv + 1);
  }

  ASTNode *expression = NULL;
  Lexer lexer = Lexer_init(input_expression);
  Parser parser;
  if (Parser_init(&parser, &lexer, 64, &error) != ERROR_TYPE_OK) {
    Error_print(&error);
    return EXIT_FAILURE;
  }

  if (argc > 1) {
    if (Parser_parse(&parser, &expression, &error) != ERROR_TYPE_OK) {
      Error_print(&error);
      return EXIT_FAILURE;
    }
    const double value = ASTNode_evaluate(expression, &error);
    if (error.type != ERROR_TYPE_OK) {
      Error_print(&error);
      return EXIT_FAILURE;
    }
    printf("%.30f\n", value);
    ASTNodePull_push(&parser.node_pull, expression);
    goto exit_program;
  }

  while (1) {
    error.type = ERROR_TYPE_OK;

    if (input(input_expression, sizeof(input_expression), ">>> ") !=
        ERROR_TYPE_OK) {
      Error_print(&error);
      continue;
    }
    if (strcmp(input_expression, "") == 0) {
      goto exit_program;
    }

    lexer = Lexer_init(input_expression);
    if (Parser_reset(&parser, &lexer, &error) != ERROR_TYPE_OK) {
      Error_print(&error);
      continue;
    }

    if (Parser_parse(&parser, &expression, &error) != ERROR_TYPE_OK) {
      Error_print(&error);
      continue;
    }
    const double value = ASTNode_evaluate(expression, &error);
    if (error.type != ERROR_TYPE_OK) {
      Error_print(&error);
      continue;
    }

    printf("%.30f\n", value);
    ASTNodePull_push(&parser.node_pull, expression);
  }

exit_program:
  RBT_deallocate(g_variables);
  return EXIT_SUCCESS;
}
