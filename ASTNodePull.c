//
// Created by kir on 10.05.2025.
//
#include "ASTNodePull.h"

#include <string.h>

ErrorType ASTNodePull_init(ASTNodePull *node_pull, const size_t capacity) {
  node_pull->nodes = calloc(capacity, sizeof(ASTNode));
  if (node_pull->nodes == NULL) {
    return ERROR_TYPE_MEMORY_ALLOCATION_FAILED;
  }
  node_pull->capacity = capacity;
  node_pull->reserved = 0;

  return ERROR_TYPE_OK;
}

void ASTNodePull_free(const ASTNodePull *node_pull) {
  if (node_pull == NULL)
    return;
  free(node_pull->nodes);
}

ASTNode *ASTNodePull_find_free(const ASTNodePull *node_pull) {
  if (node_pull == NULL || node_pull->reserved >= node_pull->capacity)
    return NULL;
  for (size_t i = 0; i < node_pull->capacity; i++) {
    if (node_pull->nodes[i].used == AST_NODE_USED_FROM_FREE) {
      ASTNode *node = node_pull->nodes + i;
      return node;
    }
  }
  return NULL;
}

ErrorType ASTNodePull_pop(const ASTNodePull *node_pull, ASTNode **node,
                          const ASTNodeType node_type, Error *error) {
  if (node_pull == NULL) {
    if (error != NULL) {
      error->type = ERROR_TYPE_ARGUMENT;
      error->argument.where = strdup("ASTNodePull_pop");
      error->argument.argument_name = strdup("node_pull");
    }
    return ERROR_TYPE_ARGUMENT;
  }
  if (node == NULL) {
    if (error != NULL) {
      error->type = ERROR_TYPE_ARGUMENT;
      error->argument.where = strdup("ASTNodePull_pop");
      error->argument.argument_name = strdup("node");
    }
    return ERROR_TYPE_ARGUMENT;
  }

  if (node_pull->reserved >= node_pull->capacity) {
    ErrorType error_type;
    if ((error_type = ASTNode_allocate(node, node_type)) != ERROR_TYPE_OK) {
      return error_type;
    }
    (*node)->used = AST_NODE_USED_FROM_MALLOC;
    return ERROR_TYPE_OK;
  }

  *node = ASTNodePull_find_free(node_pull);
  if (*node == NULL)
    return ERROR_TYPE_UNKNOWN;

  (*node)->used = AST_NODE_USED_FROM_PULL;
  (*node)->type = node_type;
  return ERROR_TYPE_OK;
}

void ASTNodePull_push(ASTNodePull *node_pull, ASTNode *node) {
  if (node_pull == NULL)
    return;

  if (node->used == AST_NODE_USED_FROM_MALLOC) {
    ASTNode_free(node);
    return;
  }

  node_pull->reserved -= 1;
  node->used = AST_NODE_USED_FROM_FREE;
}
