#pragma once

#include "CFGOperand.h"
#include "../expr/expr.h"

// NOTE: fast implementation --> no different types of expressions, every expr has two operands, if 
//       it is a unary expr then right = NULL

typedef struct {
  CFGOperand *left;
  CFGOperand *right; // can be NULL
  OperationType operation;
  bool is_operand;   // if True operation does not mean anything
} CFGExpression;

CFGExpression *CFGExpression_Create_UnaryExpression(CFGOperand *oprnd, OperationType operation);
CFGExpression *CFGExpression_Create_BinaryExpression(CFGOperand *left, CFGOperand *right, OperationType operation);

void CFGExpression_Destroy(CFGExpression *expr);

bool CFGExpression_IsOperand(CFGExpression *cfg_expr) {
  assert(cfg_expr != NULL);
  return cfg_expr->is_operand;
}

// CREATE

CFGExpression *__CFGExpression_Create(CFGOperand *left, CFGOperand *right, OperationType operation, bool is_operand) {
  assert(left != NULL);
  CFGExpression *expr = (CFGExpression *) malloc(sizeof(CFGExpression));
  expr->left = left;
  expr->right = right;
  expr->operation = operation;
  expr->is_operand = is_operand;
  return expr;
}

CFGExpression *CFGExpression_Create_UnaryExpression(CFGOperand *oprnd, OperationType operation) {
  assert(oprnd != NULL);
  // TODO: assert(operation_is_unary(operation));
  return __CFGExpression_Create(oprnd, NULL, operation, False);
}

CFGExpression *CFGExpression_Create_BinaryExpression(CFGOperand *left, CFGOperand *right, OperationType operation) {
  assert(left != NULL);
  assert(right != NULL);
  // TODO: assert(operation_is_binary(operation));
  return __CFGExpression_Create(left, right, operation, False);
}

CFGExpression *CFGExpression_Create_OperandExpression(CFGOperand *operand) {
  assert(operand != NULL);
  // TODO: assert(operation_is_unary(operation));
  return __CFGExpression_Create(operand, NULL, NULL_OPERATION, True);
}

// DESTROY

void CFGExpression_Destroy(CFGExpression *expr) {
  assert(expr != NULL);
  CFGOperand_Destroy(expr->left);
  if (expr->right != NULL)
    CFGOperand_Destroy(expr->right);
  free(expr);
}

// PRINT

void CFGExpression_Print(CFGExpression *expr, FILE *file) {
  assert(expr != NULL);

  OperationType operation = expr->operation;
  if (CFGExpression_IsOperand(expr)) {
    CFGOperand_Print(expr->left, file);
  }
  else if (expr->right == NULL) {
    if (operation == STR_LEN_UNARY_OPERATION) {
      fprintf(file, "< ");
      CFGOperand_Print(expr->left, file);
      fprintf(file, " >");
    } 
    else {
      fprintf(file, "%s ", operation_to_char[operation]);
      CFGOperand_Print(expr->left, file);
    }
    return;
  } 
  else {
    CFGOperand_Print(expr->left, file);
    fprintf(file, " %s ", operation_to_char[operation]);
    CFGOperand_Print(expr->right, file);
  }
}
