//
// Created by kir on 10.05.2025.
//

#ifndef ASTNODEPULL_H
#define ASTNODEPULL_H
#include "AST.h"
#include "Error.h"

typedef struct {
  ASTNode *nodes;
  size_t capacity;
  size_t reserved;
} ASTNodePull;

ErrorType ASTNodePull_init(ASTNodePull *node_pull, size_t capacity);

void ASTNodePull_free(const ASTNodePull *node_pull);

ASTNode *ASTNodePull_find_free(const ASTNodePull *node_pull);

ErrorType ASTNodePull_pop(const ASTNodePull *node_pull, ASTNode **node,
                          ASTNodeType node_type, Error *error);

void ASTNodePull_push(ASTNodePull *node_pull, ASTNode *node);

#endif // ASTNODEPULL_H
