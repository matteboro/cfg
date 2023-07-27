#pragma once

#include "CFGOperand.h"
#include "../expr/expr.h"

// NOTE: fast implementation --> no different types of expressions, every expr has two operands, if 
//       it is a unary expr then right = NULL

typedef struct {
  CFGOperand *left;
  CFGOperand *right; // can be NULL
  OperationType operation;
} CFGExpression;

CFGExpression *CFGExpression_Create_Unary_Expression(CFGOperand *oprnd, OperationType operation);
CFGExpression *CFGExpression_Create_Binary_Expression(CFGOperand *left, CFGOperand *right, OperationType operation);

void CFGExpression_Destroy(CFGExpression *expr);

// CREATE

CFGExpression *__CFGExpression_Create(CFGOperand *left, CFGOperand *right, OperationType operation) {
  assert(left != NULL);
  CFGExpression *expr = (CFGExpression *) malloc(sizeof(CFGExpression));
  expr->left = left;
  expr->right = right;
  expr->operation = operation;
  return expr;
}

CFGExpression *CFGExpression_Create_Unary_Expression(CFGOperand *oprnd, OperationType operation) {
  assert(oprnd != NULL);
  // TODO: assert(operation_is_unary(operation));
  return __CFGExpression_Create(oprnd, NULL, operation);
}

CFGExpression *CFGExpression_Create_Binary_Expression(CFGOperand *left, CFGOperand *right, OperationType operation) {
  assert(left != NULL);
  assert(right != NULL);
  // TODO: assert(operation_is_binary(operation));
  return __CFGExpression_Create(left, right, operation);
}

// DESTROY

void CFGExpression_Destroy(CFGExpression *expr) {
  assert(expr != NULL);
  CFGOperand_Destroy(expr->left);
  if (expr->right != NULL)
    CFGOperand_Destroy(expr->right);
  free(expr);
}
