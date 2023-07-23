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
    Operand *oprnd = expr_operand_expression_get_operand(expr);
    Type *oprnd_type = oprnd_chckr_get_type(oprnd, an_state);
    oprnd_set_real_type(oprnd, type_copy(oprnd_type));
    expr_set_real_type(expr, type_copy(oprnd_type));
    return oprnd_type;
  }
  return NULL;
}

static size_t num_simplifications = 0;

// TODO: multipliy divide by 1, add subtract 0 could be simplified, multiply by 0
// TODO: divide by 0 --> instant error
Expression *expr_chckr_simplify(Expression *expr) {
  if (expr->type == BINARY_EXPRESSION_EXP_TYPE) {
    // recursively simplify sub-expression
    Expression *left = expr_binary_expression_get_left(expr);
    Expression *right = expr_binary_expression_get_right(expr);
    left = expr_chckr_simplify(left);
    right = expr_chckr_simplify(right);
    expr_binary_expression_set_left(expr, left);
    expr_binary_expression_set_right(expr, right);

    // fprintf(stdout, "left is operand: %s\n", expr_is_operand(left) ? "yes" : "no");
    // fprintf(stdout, "right is operand: %s\n", expr_is_operand(right) ? "yes" : "no");
    
    // simplify this expression
    // NOTE: could use better interface: is basic, is simplifyable type ...
    if (expr_is_operand(left) && expr_is_operand(right)) {
      Operand *left_oprnd = expr_operand_expression_get_operand(left);
      Operand *right_oprnd = expr_operand_expression_get_operand(right);
      Type *left_type = oprnd_get_real_type(left_oprnd);
      Type *right_type = oprnd_get_real_type(right_oprnd);
      OperationType op = expr_binary_expression_get_operation(expr);
      BinaryOpExpectedIO opIO = binary_ops_expected_io[op];
      BinaryOpExpectedInput input = opIO.input;
      // this check is actually performed before, we do it again without printing any error
      // fprintf(stdout, "left: ");  type_print(left_type, stdout);  fprintf(stdout, "\n");
      // fprintf(stdout, "right: "); type_print(right_type, stdout); fprintf(stdout, "\n");
      // fprintf(stdout, "left: ");  oprnd_print_operand_type(left_oprnd->type, stdout);  fprintf(stdout, "\n");
      // fprintf(stdout, "right: "); oprnd_print_operand_type(right_oprnd->type, stdout); fprintf(stdout, "\n");
      if (type_is_of_type(left_type, input.left) && 
          type_is_of_type(right_type, input.right) &&
          oprnd_is_literal(left_oprnd) && 
          oprnd_is_literal(right_oprnd)) {

        if (op != STR_CONCAT_OPERATION) { 
          // assume oprnd are both integer
          int *left_val = oprnd_integer_get_integer(left_oprnd);
          int *right_val = oprnd_integer_get_integer(right_oprnd);
          int *res = (int *) malloc(sizeof(int));
          switch (op) {
            case SUM_OPERATION:  (*res) = (*left_val) +  (*right_val);  break;
            case SUB_OPERATION:  (*res) = (*left_val) -  (*right_val);  break;
            case MULT_OPERATION: (*res) = (*left_val) *  (*right_val);  break;
            case DIV_OPERATION:  (*res) = (*left_val) /  (*right_val);  break;
            case LE_OPERATION:   (*res) = (*left_val) <  (*right_val);  break;
            case LEQ_OPERATION:  (*res) = (*left_val) <= (*right_val);  break;
            case GE_OPERATION:   (*res) = (*left_val) >  (*right_val);  break;
            case GEQ_OPERATION:  (*res) = (*left_val) >= (*right_val);  break;
            case EQ_OPERATION:   (*res) = (*left_val) == (*right_val);  break;
            case NEQ_OPERATION:  (*res) = (*left_val) != (*right_val);  break;
            default: return expr;
          }
          Operand *new_operand = oprnd_create_integer(res, expr->file_info);
          Expression *new_expression = expr_create_operand_expression(new_operand);
          oprnd_set_real_type(new_operand, type_copy(expr->real_type));
          expr_set_real_type(new_expression, type_copy(expr->real_type));
          expr_dealloc_expression(expr);
          ++num_simplifications;
          return new_expression;
        } 
        else if (op == STR_CONCAT_OPERATION) {
          // assume oprnd are both integer
          char *left_val = oprnd_string_get_string(left_oprnd);
          char *right_val = oprnd_string_get_string(right_oprnd);
          size_t left_len = strlen(left_val);
          size_t right_len = strlen(right_val);
          char *res = (char *) malloc(sizeof(char)*(left_len + right_len + 1));
          for (size_t i=0; i<left_len; ++i) {
            res[i] = left_val[i];
          }
          for (size_t i=0; i<right_len; ++i) {
            res[i + left_len] = right_val[i];
          }
          res[left_len + right_len] = '\0';
          Operand *new_operand = oprnd_create_string(res, expr->file_info);
          Expression *new_expression = expr_create_operand_expression(new_operand);
          oprnd_set_real_type(new_operand, type_copy(expr->real_type));
          expr_set_real_type(new_expression, type_copy(expr->real_type));
          expr_dealloc_expression(expr);
          ++num_simplifications;
          return new_expression;
        }
      }
    }
  } 
  else if (expr->type == UNARY_EXPRESSION_EXP_TYPE) {
    Expression *operand_expr = expr_unary_expression_get_operand(expr);
    operand_expr = expr_chckr_simplify(operand_expr);
    expr_unary_expression_set_operand(expr, operand_expr);
    if (expr_is_operand(operand_expr)) {
      Operand *oprnd = expr_operand_expression_get_operand(operand_expr);
      Type *oprnd_type = oprnd_get_real_type(oprnd);
      OperationType op = expr_unary_expression_get_operation(expr);
      UnaryOpExpectedIO opIO = unary_ops_expected_io[op];
      UnaryOpExpectedInput input = opIO.input;
      if (type_is_of_type(oprnd_type, input.type) && oprnd_is_literal(oprnd)) {
        if (op != STR_LEN_UNARY_OPERATION) {
          int *val = oprnd_integer_get_integer(oprnd);
          int *res = (int *) malloc(sizeof(int));          
          switch (op) {
            case MINUS_UNARY_OPERATION: (*res) = -(*val); break;
            case NOT_UNARY_OPERATION: (*res) = !(*val); break;
            default: return expr;
          }
          Operand *new_operand = oprnd_create_integer(res, expr->file_info);
          Expression *new_expression = expr_create_operand_expression(new_operand);
          oprnd_set_real_type(new_operand, type_copy(expr->real_type));
          expr_set_real_type(new_expression, type_copy(expr->real_type));
          expr_dealloc_expression(expr);
          ++num_simplifications;
          return new_expression;          
        } 
        else if (op == STR_LEN_UNARY_OPERATION) {
          char *val = oprnd_string_get_string(oprnd);
          int *res = (int *) malloc(sizeof(int));
          (*res) = strlen(val);
          Operand *new_operand = oprnd_create_integer(res, expr->file_info);
          Expression *new_expression = expr_create_operand_expression(new_operand);
          oprnd_set_real_type(new_operand, type_copy(expr->real_type));
          expr_set_real_type(new_expression, type_copy(expr->real_type));
          expr_dealloc_expression(expr);
          ++num_simplifications;
          return new_expression; 
        }
      }
    }
  } 
  return expr;
}