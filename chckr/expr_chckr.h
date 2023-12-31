#pragma once

#include "../expr/expr.h"
#include "oprnd_chckr.h"
#include "chckr_env.h"

#define EXPR_CHCKR_ERROR_HEADER(expr_type, expr) \
  fprintf(stdout, "ERROR: did not pass " expr_type "expression analysis.\n\n  In expression: \n    "); \
  expr_print_expression(expr, stdout); fprintf(stdout, ",\n  ");


Type *expr_chckr_get_returned_type(Expression *expr, ASTCheckingAnalysisState *an_state);
Expression *expr_chckr_simplify(Expression *expr);

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

bool expr_chckr_check_array_create_expression(Expression *expr, ASTCheckingAnalysisState *an_state) {
  Expression *size_expr = expr_create_expression_get_size(expr);
  Type *size_expr_type = expr_chckr_get_returned_type(size_expr, an_state);

  if (size_expr_type == NULL) {
    EXPR_CHCKR_ERROR_HEADER("create", expr);
    fprintf(stdout, "the expression controlling the size: ");
    expr_print_expression(size_expr, stdout);
    fprintf(stdout, " does not yield a valid return type\n\n");
    single_line_file_info_print_context(size_expr->file_info, stdout);
    fprintf(stdout, "\n\n");
    return False;
  }

  if (!type_is_integer(size_expr_type)) {
    EXPR_CHCKR_ERROR_HEADER("create", expr);
    fprintf(stdout, "the expression controlling the size: ");
    expr_print_expression(size_expr, stdout);
    fprintf(stdout, " is not of integer type as expected but: ");
    type_print(size_expr_type, stdout); fprintf(stdout, "\n\n"); 
    single_line_file_info_print_context(size_expr->file_info, stdout);
    fprintf(stdout, "\n\n");
    type_dealloc(size_expr_type);
    return False;
  }

  size_expr = expr_chckr_simplify(size_expr);
  expr_create_expression_set_size(expr, size_expr);

  type_dealloc(size_expr_type);
  return True;
}

Type *expr_chckr_get_returned_type(Expression *expr, ASTCheckingAnalysisState *an_state) {
  assert(expr != NULL);
  assert(an_state != NULL);
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
  else if (expr->type == CREATE_EXP_TYPE) {
    // I check in the case of allocating an array that the expression is valid
    if (!expr_create_expression_is_single_element(expr)) {
      if(!expr_chckr_check_array_create_expression(expr, an_state))
        return NULL;
    }
    // I contruct the Type to be returned
    Type *ptr_type = expr_create_expression_get_type(expr);
    // TODO: think why we say False to is_strong here
    ptr_type = type_create_pointer_type(type_copy(ptr_type), False, file_info_create_null());
    expr_set_real_type(expr, type_copy(ptr_type));
    return ptr_type;
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