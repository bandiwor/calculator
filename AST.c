//
// Created by kir on 10.05.2025.
//
#include "AST.h"

#include <stdio.h>

ErrorType ASTNode_allocate(ASTNode **node, const ASTNodeType type) {
  *node = (ASTNode *)malloc(sizeof(ASTNode));
  if (*node == NULL) {
    return ERROR_TYPE_MEMORY_ALLOCATION_FAILED;
  }
  (*node)->type = type;
  return ERROR_TYPE_OK;
}

void ASTNode_free(ASTNode *node) {
  if (node == NULL) {
    return;
  }

  switch (node->type) {
  case AST_NODE_BINARY_OP:
    ASTNode_free(node->binary.left);
    ASTNode_free(node->binary.right);
    break;
  case AST_NODE_UNARY_OP:
    ASTNode_free(node->unary.operand);
    break;
  default:
    break;
  }

  free(node);
}

static void indent_print(const int level) {
#ifndef CALC_REMOVE_DEBUG_INFO
  for (int i = 0; i < level; ++i)
    printf("|    ");
#endif
}

void ASTNode_print(const ASTNode *node, const int indent) {
#ifndef CALC_REMOVE_DEBUG_INFO
  if (node == NULL)
    return;

  indent_print(indent);
  const char *node_type_text = ASTNodeType_to_string(node->type);

  switch (node->type) {
  case AST_NODE_NUMBER:
    printf("%s: %f\n", node_type_text, node->number);
    break;
  case AST_NODE_UNARY_OP:
    const char *unary_operator_type_text =
        ASTUnaryNodeType_to_string(node->unary.op);
    printf("%s: (%s):\n", node_type_text, unary_operator_type_text);
    indent_print(indent + 1);
    printf("Operand:\n");
    ASTNode_print(node->unary.operand, indent + 2);
    break;
  case AST_NODE_BINARY_OP:
    const char *binary_operator_type_text =
        ASTBinaryNodeType_to_string(node->binary.op);
    printf("%s (%s):\n", node_type_text, binary_operator_type_text);
    indent_print(indent + 1);
    printf("Left:\n");
    ASTNode_print(node->binary.left, indent + 2);
    indent_print(indent + 1);
    printf("Right:\n");
    ASTNode_print(node->binary.right, indent + 2);
    break;
  case AST_NODE_VAR_REF:
    printf("%s: (%s):\n", node_type_text, node->var_ref.name);
    indent_print(indent + 1);
    printf("Value:\n");
    indent_print(indent + 2);
    printf("<value>\n");
    break;
  default:
    printf("%s", node_type_text);
    break;
  }
#endif
}
