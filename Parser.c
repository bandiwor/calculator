//
// Created by kir on 10.05.2025.
//
#include "Parser.h"

ErrorType Parser_init(Parser *parser, Lexer *lexer,
                      const size_t node_pull_capacity, Error *error) {
  ErrorType error_type;

  parser->lexer = lexer;
  if ((error_type = Lexer_next_token(lexer, &parser->current_token, error)) !=
      ERROR_TYPE_OK) {
    return error_type;
  }

  if ((error_type = ASTNodePull_init(&parser->node_pull, node_pull_capacity)) !=
      ERROR_TYPE_OK) {
    return error_type;
  }

  return ERROR_TYPE_OK;
}

ErrorType Parser_reset(Parser *parser, Lexer *lexer, Error *error) {
  if (parser == NULL) {
    if (error != NULL) {
      error->type = ERROR_TYPE_ARGUMENT;
      error->argument.where = strdup("Parser_reset");
      error->argument.argument_name = strdup("parser");
    }
    return ERROR_TYPE_ARGUMENT;
  }

  parser->lexer = lexer;
  Lexer_reset(parser->lexer);

  ErrorType error_type;
  if ((error_type = Lexer_next_token(lexer, &parser->current_token, error)) !=
      ERROR_TYPE_OK) {
    return error_type;
  }

  return ERROR_TYPE_OK;
}

static ErrorType Parser_next_token(Parser *parser, Error *error) {
  LexerToken_free(&parser->current_token);
  ErrorType error_type;
  if ((error_type = Lexer_next_token(parser->lexer, &parser->current_token,
                                     error)) != ERROR_TYPE_OK) {
    return error_type;
  }
  return ERROR_TYPE_OK;
}

static ErrorType Parser_parse_expression(Parser *parser, ASTNode **node,
                                         const int min_precedence,
                                         Error *error);

static ErrorType Parser_parse_primary(Parser *parser, ASTNode **node,
                                      Error *error) {
  ErrorType error_type;

  switch (parser->current_token.type) {
  case LEXER_TOKEN_IDENTIFIER:
    if ((error_type = ASTNodePull_pop(&parser->node_pull, node,
                                      AST_NODE_VAR_REF, error)) !=
        ERROR_TYPE_OK) {
      return error_type;
    }
    (*node)->var_ref.name = strdup(parser->current_token.identifier);
    if ((*node)->var_ref.name == NULL) {
      return ERROR_TYPE_MEMORY_ALLOCATION_FAILED;
    }
    if ((error_type = Parser_next_token(parser, error)) != ERROR_TYPE_OK) {
      return error_type;
    }
    return ERROR_TYPE_OK;
  case LEXER_TOKEN_NUMBER:
    if ((error_type = ASTNodePull_pop(&parser->node_pull, node, AST_NODE_NUMBER,
                                      error)) != ERROR_TYPE_OK) {
      return error_type;
    }
    (*node)->number = parser->current_token.number;
    if ((error_type = Parser_next_token(parser, error)) != ERROR_TYPE_OK) {
      return error_type;
    }
    return ERROR_TYPE_OK;
  case LEXER_TOKEN_LPAREN:
    if ((error_type = Parser_next_token(parser, error)) != ERROR_TYPE_OK) {
      return error_type;
    }
    if ((error_type = Parser_parse_expression(parser, node, 0, error)) !=
        ERROR_TYPE_OK) {
      return error_type;
    }
    if (parser->current_token.type != LEXER_TOKEN_RPAREN) {
      if (error != NULL) {
        error->type = ERROR_TYPE_UNEXPECTED_TOKEN;
        error->unexpected_token.expect = LEXER_TOKEN_RPAREN;
        error->unexpected_token.got = parser->current_token.type;
      }
      return ERROR_TYPE_UNEXPECTED_TOKEN;
    }
    if ((error_type = Parser_next_token(parser, error)) != ERROR_TYPE_OK) {
      return error_type;
    }
    return ERROR_TYPE_OK;
  default:
    if (error != NULL) {
      error->type = ERROR_TYPE_UNEXPECTED_TOKEN;
      error->unexpected_token.got = parser->current_token.type;
      error->unexpected_token.expect = LEXER_TOKEN_IGNORE;
    }
    return ERROR_TYPE_UNEXPECTED_TOKEN;
  }
}

static ErrorType Parser_parse_unary(Parser *parser, ASTNode **node,
                                    Error *error) {
  ErrorType error_type;
  ASTUnaryNodeType uop;

  switch (parser->current_token.type) {
  case LEXER_TOKEN_MINUS:
    uop = AST_UNARY_NEGATIVE;

    if ((error_type = Parser_next_token(parser, error)) != ERROR_TYPE_OK) {
      return error_type;
    }
    {
      ASTNode *operand;
      if ((error_type = Parser_parse_unary(parser, &operand, error)) !=
          ERROR_TYPE_OK) {
        return error_type;
      }
      if ((error_type = ASTNodePull_pop(&parser->node_pull, node,
                                        AST_NODE_UNARY_OP, error)) !=
          ERROR_TYPE_OK) {
        return error_type;
      }
      (*node)->unary.op = uop;
      (*node)->unary.operand = operand;
      return ERROR_TYPE_OK;
    }
  case LEXER_TOKEN_SQRT:
    uop = AST_UNARY_SQRT;
    break;
  case LEXER_TOKEN_SIN:
    uop = AST_UNARY_SIN;
    break;
  case LEXER_TOKEN_COS:
    uop = AST_UNARY_COS;
    break;
  case LEXER_TOKEN_LG:
    uop = AST_UNARY_LG;
    break;
  case LEXER_TOKEN_LN:
    uop = AST_UNARY_LN;
    break;
  case LEXER_TOKEN_TAN:
    uop = AST_UNARY_TAN;
    break;
  case LEXER_TOKEN_COT:
    uop = AST_UNARY_COT;
    break;
  case LEXER_TOKEN_EXP:
    uop = AST_UNARY_EXP;
    break;
  case LEXER_TOKEN_GAMMA:
    uop = AST_UNARY_GAMMA;
    break;
  case LEXER_TOKEN_FACTORIAL:
    uop = AST_UNARY_FACTORIAL;
    break;
  case LEXER_TOKEN_ABS:
    uop = AST_UNARY_ABS;
    break;
  case LEXER_TOKEN_ARC_SIN:
    uop = AST_UNARY_ARC_SIN;
    break;
  case LEXER_TOKEN_ARC_COS:
    uop = AST_UNARY_ARC_COS;
    break;
  case LEXER_TOKEN_ARC_TAN:
    uop = AST_UNARY_ARC_TAN;
    break;
  case LEXER_TOKEN_ARC_COT:
    uop = AST_UNARY_ARC_COT;
    break;
  case LEXER_TOKEN_ARC_GAMMA:
    uop = AST_UNARY_ARC_GAMMA;
    break;
  case LEXER_TOKEN_ARC_FACTORIAL:
    uop = AST_UNARY_ARC_FACTORIAL;
    break;
  default:
    return Parser_parse_primary(parser, node, error);
  }

  if ((error_type = Parser_next_token(parser, error)) != ERROR_TYPE_OK) {
    return error_type;
  }

  if (parser->current_token.type != LEXER_TOKEN_LPAREN) {
    if (error != NULL) {
      error->type = ERROR_TYPE_UNEXPECTED_TOKEN;
      error->unexpected_token.expect = LEXER_TOKEN_LPAREN;
      error->unexpected_token.got = parser->current_token.type;
    }
    return ERROR_TYPE_UNEXPECTED_TOKEN;
  }

  if ((error_type = Parser_next_token(parser, error)) != ERROR_TYPE_OK) {
    return error_type;
  }

  ASTNode *operand;
  if ((error_type = Parser_parse_expression(parser, &operand, 0, error)) !=
      ERROR_TYPE_OK) {
    return error_type;
  }

  if (parser->current_token.type != LEXER_TOKEN_RPAREN) {
    if (error != NULL) {
      error->type = ERROR_TYPE_UNEXPECTED_TOKEN;
      error->unexpected_token.expect = LEXER_TOKEN_RPAREN;
      error->unexpected_token.got = parser->current_token.type;
    }
    return ERROR_TYPE_UNEXPECTED_TOKEN;
  }

  if ((error_type = Parser_next_token(parser, error)) != ERROR_TYPE_OK) {
    return error_type;
  }

  if ((error_type = ASTNodePull_pop(&parser->node_pull, node, AST_NODE_UNARY_OP,
                                    error)) != ERROR_TYPE_OK) {
    return error_type;
  }
  (*node)->unary.op = uop;
  (*node)->unary.operand = operand;
  return ERROR_TYPE_OK;
}

static int Parser_get_operator_precedence(const ASTBinaryNodeType type) {
  switch (type) {
  case AST_BINARY_ASSIGN:
    return 0;
  case AST_BINARY_ADD:
  case AST_BINARY_SUB:
    return 9;

  case AST_BINARY_MUL:
  case AST_BINARY_DIV:
  case AST_BINARY_MOD:
  case AST_BINARY_INT_DIV:
    return 10;

  case AST_BINARY_POWER:
    return 12;
  default:
    return 0;
  }
}

static ErrorType Parser_parse_expression(Parser *parser, ASTNode **node,
                                         const int min_precedence,
                                         Error *error) {
  ErrorType error_type;
  ASTNode *left = NULL;
  if ((error_type = Parser_parse_unary(parser, &left, error)) !=
      ERROR_TYPE_OK) {
    return error_type;
  }

  while (1) {
    ASTBinaryNodeType op;
    const LexerTokenType token_type = parser->current_token.type;
    int precedence = 0;
    int is_right_assoc = 0;

    switch (token_type) {
    case LEXER_TOKEN_POWER:
      op = AST_BINARY_POWER;
      is_right_assoc = 1;
      break;
    case LEXER_TOKEN_ASSIGN:
      op = AST_BINARY_ASSIGN;
      is_right_assoc = 1;
      break;
    case LEXER_TOKEN_PLUS:
      op = AST_BINARY_ADD;
      break;
    case LEXER_TOKEN_MINUS:
      op = AST_BINARY_SUB;
      break;
    case LEXER_TOKEN_MUL:
      op = AST_BINARY_MUL;
      break;
    case LEXER_TOKEN_DIV:
      op = AST_BINARY_DIV;
      break;
    case LEXER_TOKEN_INT_DIV:
      op = AST_BINARY_INT_DIV;
      break;
    case LEXER_TOKEN_MOD:
      op = AST_BINARY_MOD;
      break;
    default:
      *node = left;
      return ERROR_TYPE_OK;
    }

    precedence = Parser_get_operator_precedence(op);
    if (precedence < min_precedence)
      break;

    if ((error_type = Parser_next_token(parser, error)) != ERROR_TYPE_OK) {
      return error_type;
    }

    const int next_min_precedence =
        is_right_assoc ? precedence : precedence + 1;

    ASTNode *right;
    if ((error_type = Parser_parse_expression(
             parser, &right, next_min_precedence, error)) != ERROR_TYPE_OK) {
      return error_type;
    }

    ASTNode *binary_node;
    if ((error_type = ASTNodePull_pop(&parser->node_pull, &binary_node,
                                      AST_NODE_BINARY_OP, error)) !=
        ERROR_TYPE_OK) {
      return error_type;
    }

    binary_node->binary.left = left;
    binary_node->binary.right = right;
    binary_node->binary.op = op;

    left = binary_node;
  }

  *node = left;
  return ERROR_TYPE_OK;
}

ErrorType Parser_parse(Parser *parser, ASTNode **node, Error *error) {
  return Parser_parse_expression(parser, node, 0, error);
}
