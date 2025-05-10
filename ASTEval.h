//
// Created by kir on 10.05.2025.
//

#ifndef ASTEVAL_H
#define ASTEVAL_H
#include "AST.h"
#include "Error.h"

double ASTNode_evaluate(const ASTNode *node, Error *error);

#endif //ASTEVAL_H
