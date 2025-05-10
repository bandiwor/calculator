//
// Created by kir on 10.05.2025.
//
#include "ASTEval.h"

#include "ArcGamma.h"
#include "Globals.h"
#include "MyMath.h"
#include "RBT.h"

#include <float.h>
#include <math.h>
#include <string.h>

double ASTNode_evaluate(const ASTNode *node, Error *error) {
  switch (node->type) {
  case AST_NODE_NUMBER:
    return node->number;
  case AST_NODE_VAR_REF:
    const RBTNode *value_node = RBT_search(g_variables, node->var_ref.name);
    if (value_node == NULL) {
      if (error != NULL) {
        error->type = ERROR_TYPE_NAME;
        error->name.not_found_name = strdup(node->var_ref.name);
      }
      return NAN;
    }
    return value_node->value.f64;
  case AST_NODE_UNARY_OP:
    const double v = ASTNode_evaluate(node->unary.operand, error);
    switch (node->unary.op) {
    case AST_UNARY_NEGATIVE:
      return -v;
    case AST_UNARY_SQRT:
      return sqrt(v);
    case AST_UNARY_SIN:
      return sin(v);
    case AST_UNARY_COS:
      return cos(v);
    case AST_UNARY_LG:
      return log10(v);
    case AST_UNARY_LN:
      return log(v);
    case AST_UNARY_TAN:
      return tan(v);
    case AST_UNARY_COT: {
      const double sin_v = sin(v);
      if (fabs(sin_v) < DBL_EPSILON) {
        if (error) {
          error->type = ERROR_TYPE_COTANGENS_IS_UNDEFINED;
          error->not_defined_for_argument.argument = v;
        }
        return NAN;
      }
      return cos(v) / sin_v;
    }
    case AST_UNARY_ABS:
      return fabs(v);
    case AST_UNARY_GAMMA:
      return tgamma(v);
    case AST_UNARY_FACTORIAL:
      return tgamma(v + 1.0f);
    case AST_UNARY_EXP:
      return exp(v);
    case AST_UNARY_ARC_SIN:
      return asin(v);
    case AST_UNARY_ARC_COS:
      return acos(v);
    case AST_UNARY_ARC_TAN:
      return atan(v);
    case AST_UNARY_ARC_COT:
      return arccot(v);
    case AST_UNARY_ARC_GAMMA: {
      bool success;
      const double result = arcgamma(v, &success);
      if (!success) {
        if (error) {
          error->type = ERROR_TYPE_ARCGAMMA;
          error->not_defined_for_argument.argument = v;
        }
        return NAN;
      }
      return result;
    }
    case AST_UNARY_ARC_FACTORIAL: {
      bool success = true;
      const double result = arcfactorial(v, &success);
      if (!success) {
        if (error != NULL) {
          error->type = ERROR_TYPE_ARCFACTORIAL;
          error->not_defined_for_argument.argument = v;
        }
        return NAN;
      }
      return result;
    }
    default:
      break;
    }
  case AST_NODE_BINARY_OP:
    const double R = ASTNode_evaluate(node->binary.right, error);
    if (node->binary.op == AST_BINARY_ASSIGN) {
      if (node->binary.left->type != AST_NODE_VAR_REF) {
        if (error != NULL) {
          error->type = ERROR_TYPE_ASSIGN;
        }
        return NAN;
      }
      RBT_insert(g_variables, node->binary.left->var_ref.name,
                 (RBTNodeValue){.f64 = R});
      return R;
    }

    const double L = ASTNode_evaluate(node->binary.left, error);
    switch (node->binary.op) {
    case AST_BINARY_ADD:
      return L + R;
    case AST_BINARY_SUB:
      return L - R;
    case AST_BINARY_MUL:
      return L * R;
    case AST_BINARY_DIV:
      return L / R;
    case AST_BINARY_INT_DIV:
      return floor(L / R);
    case AST_BINARY_MOD:
      return fmod(L, R);
    case AST_BINARY_POWER:
      return pow(L, R);
    case AST_BINARY_LOG:
      return log(R) / log(L);
    default:
      break;
    }
  default:
    break;
  }

  return NAN;
}

