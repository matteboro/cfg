#pragma once

#include "../expr/expr.h"
#include "oprnd_chckr.h"
#include "chckr_env.h"

Type *expr_chckr_get_returned_type(Expression *expr, ASTCheckingAnalysisState *an_state);

typedef struct {
  TypeType left;
  TypeType right;
} BinaryOpExpectedInput;

typedef struct {
  BinaryOpExpectedInput input;
  TypeType output;
} BinaryOpExpectedIO;

BinaryOpExpectedIO binary_ops_expected_io[] = {
  [SUM_OPERATION]        = {{INT_TYPE,    INT_TYPE},    INT_TYPE},
  [SUB_OPERATION]        = {{INT_TYPE,    INT_TYPE},    INT_TYPE},
  [MULT_OPERATION]       = {{INT_TYPE,    INT_TYPE},    INT_TYPE},
  [DIV_OPERATION]        = {{INT_TYPE,    INT_TYPE},    INT_TYPE},
  [LE_OPERATION]         = {{INT_TYPE,    INT_TYPE},    INT_TYPE},
  [LEQ_OPERATION]        = {{INT_TYPE,    INT_TYPE},    INT_TYPE},
  [GE_OPERATION]         = {{INT_TYPE,    INT_TYPE},    INT_TYPE},
  [GEQ_OPERATION]        = {{INT_TYPE,    INT_TYPE},    INT_TYPE},
  [EQ_OPERATION]         = {{INT_TYPE,    INT_TYPE},    INT_TYPE},
  [NEQ_OPERATION]        = {{INT_TYPE,    INT_TYPE},    INT_TYPE},
  [STR_CONCAT_OPERATION] = {{STRING_TYPE, STRING_TYPE}, STRING_TYPE},
};

typedef struct {
  TypeType type;
} UnaryOpExpectedInput;

typedef struct {
  UnaryOpExpectedInput input;
  TypeType output;
} UnaryOpExpectedIO;

UnaryOpExpectedIO unary_ops_expected_io[] = {
  [MINUS_UNARY_OPERATION] =   {{INT_TYPE},    INT_TYPE},
  [NOT_UNARY_OPERATION] =     {{INT_TYPE},    INT_TYPE},
  [STR_LEN_UNARY_OPERATION] = {{STRING_TYPE}, INT_TYPE},
};

Type *expr_chckr_get_returned_type(Expression *expr, ASTCheckingAnalysisState *an_state) {
  if (expr->type == BINARY_EXPRESSION_EXP_TYPE) {
    Expression *left = expr_binary_expression_get_left(expr);
    Expression *right = expr_binary_expression_get_right(expr);
    Type *left_type = expr_chckr_get_returned_type(left, an_state);
    Type *right_type = expr_chckr_get_returned_type(right, an_state);
    if (left_type == NULL || right_type == NULL)  {
      type_dealloc(left_type); type_dealloc(right_type);
      return NULL;
    }
    BinaryOpExpectedIO io = binary_ops_expected_io[expr_binary_expression_get_operation(expr)];
    BinaryOpExpectedInput exp_input = io.input;
    TypeType output = io.output;
    if (left_type->type == exp_input.left && right_type->type == exp_input.right) {
      type_dealloc(left_type); type_dealloc(right_type);
      Type *output_type = type_create_basic_type(output);
      expr_set_real_type(expr, type_copy(output_type));
      return output_type;
    }
    type_dealloc(left_type); type_dealloc(right_type);
  } 
  else if (expr->type == UNARY_EXPRESSION_EXP_TYPE) {
    Expression *operand = expr_unary_expression_get_operand(expr);
    Type *type = expr_chckr_get_returned_type(operand, an_state);
    if (type == NULL) 
      return NULL;
    UnaryOpExpectedIO io = unary_ops_expected_io[expr_unary_expression_get_operation(expr)];
    UnaryOpExpectedInput exp_input = io.input;
    TypeType output = io.output;
    if (type->type == exp_input.type) {
      type_dealloc(type);
      Type *output_type = type_create_basic_type(output);
      expr_set_real_type(expr, type_copy(output_type));
      return output_type;
    }
    type_dealloc(type);
  } 
  else if (expr->type == OPERAND_EXP_TYPE){
    Type *oprnd_type = oprnd_chckr_get_type(expr_operand_expression_get_operand(expr), an_state);
    expr_set_real_type(expr, type_copy(oprnd_type));
    return oprnd_type;
  }
  return NULL;
}