//
// Created by kir on 10.05.2025.
//

#ifndef AST_H
#define AST_H
#include "Enums.h"

#include <stdlib.h>

typedef enum {
  AST_NODE_USED_FROM_FREE,
  AST_NODE_USED_FROM_PULL,
  AST_NODE_USED_FROM_MALLOC,
} ASTNodeUsed;

typedef struct tag_ASTNode {
  ASTNodeType type;
  ASTNodeUsed used;

  union {
    double number;

    struct {
      ASTBinaryNodeType op;
      struct tag_ASTNode *left;
      struct tag_ASTNode *right;
    } binary;

    struct {
      ASTUnaryNodeType op;
      struct tag_ASTNode *operand;
    } unary;

    struct {
      char *name;
    } var_ref;
  };
} ASTNode;

ErrorType ASTNode_allocate(ASTNode **node, ASTNodeType type);

void ASTNode_free(ASTNode *node);

void ASTNode_print(const ASTNode *node, int indent);


#endif // AST_H
