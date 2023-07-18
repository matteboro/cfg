#ifndef EXPR_HEADER
#define EXPR_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../lxr/lxr.h"
#include "idf.h"
#include "nt_bind.h"
#include "expr_interface.h"
#include "obj_drf.h"
#include "prmt.h"
#include "oprnd.h"


// #define EXPR_DEBUG
#ifdef EXPR_DEBUG
#define EXPR_DEBUG_PRINT() fprintf(stdout, "%s\n", __FUNCTION__);
#else
#define EXPR_DEBUG_PRINT() 
#endif

#define EXPR_ERROR() fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1);
#define if_null_print(ptr, file) if (ptr == NULL) { fprintf(file, "NULL"); return; }
#define expr_string_to_int(string) atoi(string)

#define expr_typed_data(type) type *data = (type *) malloc(sizeof(type))
#define expr_casted_data(type, elem) type *data = (type *) elem->enclosed_expression

#define EXPR_GETTER(expr_prefix, obj_type, obj_name, data_type, expr_type)                 \
obj_type *expr_## expr_prefix ## _expression_get_ ## obj_name  (Expression* expr) {        \
  assert(expr->type == expr_type);                                                         \
  expr_casted_data(data_type, expr);                                                       \
  return data->obj_name;                                                                   \
}

#define EXPR_NON_PTR_GETTER(expr_prefix, obj_type, obj_name, data_type, expr_type)         \
obj_type expr_## expr_prefix ## _expression_get_ ## obj_name  (Expression* expr) {         \
  assert(expr->type == expr_type);                                                         \
  expr_casted_data(data_type, expr);                                                       \
  return data->obj_name;                                                                   \
}

#define EXPR_SETTER(expr_prefix, obj_type, obj_name, data_type, expr_type)                            \
void expr_## expr_prefix ## _expression_set_ ## obj_name  (Expression* expr, obj_type *obj) {         \
  assert(expr->type == expr_type);                                                                    \
  expr_casted_data(data_type, expr);                                                                  \
  data->obj_name = obj;                                                                               \
  return;                                                                                             \
} 

#define EXPR_SET_GET(expr_prefix, obj_type, obj_name, data_type, expr_type)  \
  EXPR_SETTER(expr_prefix, obj_type, obj_name, data_type, expr_type)         \
  EXPR_GETTER(expr_prefix, obj_type, obj_name, data_type, expr_type)         \


// DEFINITIONS

typedef enum {
  SUM_OPERATION,
  SUB_OPERATION,
  MULT_OPERATION,
  DIV_OPERATION,
  LE_OPERATION,
  LEQ_OPERATION,
  GE_OPERATION,
  GEQ_OPERATION,
  EQ_OPERATION,
  NEQ_OPERATION,
  MINUS_UNARY_OPERATION,
  NOT_UNARY_OPERATION,
  STR_LEN_UNARY_OPERATION,
  STR_CONCAT_OPERATION,
} OperationType;

static const char *operation_to_char[] = {
  [SUM_OPERATION] = "+",
  [SUB_OPERATION] = "-",
  [MULT_OPERATION] = "*",
  [DIV_OPERATION] = "/",
  [LE_OPERATION] = "<",
  [LEQ_OPERATION] = "<=",
  [GE_OPERATION] = ">",
  [GEQ_OPERATION] = ">=",
  [EQ_OPERATION] = "==",
  [NEQ_OPERATION] = "!=",
  [MINUS_UNARY_OPERATION] = "-",
  [NOT_UNARY_OPERATION] = "!",
  [STR_LEN_UNARY_OPERATION] = "<>",
  [STR_CONCAT_OPERATION] = "|",
};

typedef struct {
  Expression *left, *right;
  OperationType operation;
} BinaryExpression;

typedef struct {
  Expression *operand;
  OperationType operation;
} UnaryExpression;

typedef struct {
  Operand *operand;
} OperandExpression;

EXPR_SET_GET(operand, Operand, operand, OperandExpression, OPERAND_EXP_TYPE)

EXPR_SET_GET(binary, Expression, left, BinaryExpression, BINARY_EXPRESSION_EXP_TYPE)
EXPR_SET_GET(binary, Expression, right, BinaryExpression, BINARY_EXPRESSION_EXP_TYPE)
EXPR_NON_PTR_GETTER(binary, OperationType, operation, BinaryExpression, BINARY_EXPRESSION_EXP_TYPE)

EXPR_SET_GET(unary, Expression, operand, UnaryExpression, UNARY_EXPRESSION_EXP_TYPE)
EXPR_NON_PTR_GETTER(unary, OperationType, operation, UnaryExpression, UNARY_EXPRESSION_EXP_TYPE)

// CREATE

Expression *expr_create_expression(ExpressionType type, void *enclosed_expression) {
  Expression *expression = (Expression *)malloc(sizeof(Expression));
  expression->type = type;
  expression->enclosed_expression = (EnclosedExpression *) enclosed_expression;
  return expression;
}

Expression *expr_create_binary_expression(Expression *left, OperationType op_type, Expression *right) {
  BinaryExpression *binary_expression = (BinaryExpression *) malloc(sizeof(BinaryExpression));
  binary_expression->left = left;
  binary_expression->right = right;
  binary_expression->operation = op_type;
  return expr_create_expression(BINARY_EXPRESSION_EXP_TYPE, binary_expression);
}

Expression *expr_create_unary_expression(Expression *operand, OperationType op_type) {
  UnaryExpression *unary_expression = (UnaryExpression *) malloc(sizeof(UnaryExpression));
  unary_expression->operand = operand;
  unary_expression->operation = op_type;
  return expr_create_expression(UNARY_EXPRESSION_EXP_TYPE, unary_expression);
}

Expression *expr_create_operand_expression(Operand *operand) {
  OperandExpression *operand_expression = (OperandExpression *) malloc(sizeof(OperandExpression));
  operand_expression->operand = operand;
  return expr_create_expression(OPERAND_EXP_TYPE, operand_expression);
}

Expression *expr_create_funccall_operand_expression(FunctionCall *func_call) {
  Operand *operand = oprnd_create_funccall(func_call);
  return expr_create_operand_expression(operand);
}

Expression *expr_create_object_deref_operand_expression(ObjectDerefList *derefs) {
  Operand *operand = oprnd_create_object_deref(derefs);
  return expr_create_operand_expression(operand);
}

Expression *expr_create_operand_expression_from_operand_type_and_data(OperandType type, const char *data) {
  void *my_data = NULL;

  if (type == INTEGER_OPERAND) {
    my_data = malloc(sizeof(int));
    int *int_data = (int *)my_data;
    *int_data = expr_string_to_int(data);
    Operand *operand = oprnd_create_integer(my_data);
    return expr_create_operand_expression(operand);
  } 
  else if (type == STRING_OPERAND) {
    my_data = malloc(strlen(data)+1);
    strcpy((char *) my_data, data);
    Operand *operand = oprnd_create_string(my_data);
    return expr_create_operand_expression(operand);
  } 
  else {
    EXPR_ERROR();
  }
}

Expression *expr_create_operand_expression_from_token(Token token) {
  OperandType type;
  char *token_data_string = lxr_get_token_data_as_cstring(token);

  switch (token.type) {
  case INTEGER_TOKEN: type = INTEGER_OPERAND; break;
  case STRING_TOKEN: type = STRING_OPERAND; break;
  default: EXPR_ERROR();
  }

  Expression* result = expr_create_operand_expression_from_operand_type_and_data(type, token_data_string);
  free(token_data_string);
  return result;
}

// DEALLOC

void expr_dealloc_binary_expression(BinaryExpression *expression) {
  EXPR_DEBUG_PRINT()
  if (expression == NULL) 
    return;
  expr_dealloc_expression(expression->left);
  expr_dealloc_expression(expression->right);
  free(expression);
}

void expr_dealloc_unary_expression(UnaryExpression *expression) {
  EXPR_DEBUG_PRINT()
  if (expression == NULL) 
    return;
  expr_dealloc_expression(expression->operand);
  free(expression);
}

void expr_dealloc_operand_expression(OperandExpression *expression) {
  EXPR_DEBUG_PRINT()
  if (expression == NULL) 
    return;
  oprnd_dealloc(expression->operand);
  free(expression);
}

void expr_dealloc_expression(Expression *expression) {
  EXPR_DEBUG_PRINT()
  if (expression == NULL) 
    return;
  switch (expression->type) {
  case OPERAND_EXP_TYPE:
    expr_dealloc_operand_expression((OperandExpression *) (expression->enclosed_expression));
  break;
  case BINARY_EXPRESSION_EXP_TYPE: {
    expr_dealloc_binary_expression((BinaryExpression *) (expression->enclosed_expression));
  } break;
  case UNARY_EXPRESSION_EXP_TYPE: {
    expr_dealloc_unary_expression((UnaryExpression *) (expression->enclosed_expression));
  } break;
  default:
    EXPR_ERROR();
  }
  free(expression);
}

// PRINT

void expr_print_binary_expression(BinaryExpression *expression, FILE *file) {
  EXPR_DEBUG_PRINT()
  if (expression == NULL) 
    return;
  fprintf(file, "(");
  expr_print_expression(expression->left, file);
  fprintf(file, " %s ", operation_to_char[expression->operation]);
  expr_print_expression(expression->right, file);
  fprintf(file, ")");
}

void expr_print_unary_expression(UnaryExpression *expression, FILE *file) {
  EXPR_DEBUG_PRINT()
  if (expression == NULL) 
    return;
  
  if (expression->operation == STR_LEN_UNARY_OPERATION) {
    fprintf(file, "< ");
    expr_print_expression(expression->operand, file);
    fprintf(file, " >");
  } else {
    fprintf(file, "(");
    fprintf(file, "%s ", operation_to_char[expression->operation]);
    expr_print_expression(expression->operand, file);
    fprintf(file, ")");
  }
}

void expr_print_operand_expression(OperandExpression *expression, FILE *file) {
  EXPR_DEBUG_PRINT()
  if (expression == NULL) 
    return;
  oprnd_print(expression->operand, file);
}

void expr_print_expression(Expression *expression, FILE *file) {
  EXPR_DEBUG_PRINT()
  if (expression == NULL) 
    return;
  switch (expression->type) {
  case OPERAND_EXP_TYPE:
    expr_print_operand_expression((OperandExpression *) expression->enclosed_expression, file);
  break;
  case BINARY_EXPRESSION_EXP_TYPE:
    expr_print_binary_expression((BinaryExpression *) expression->enclosed_expression, file);
  break;
  case UNARY_EXPRESSION_EXP_TYPE:
    expr_print_unary_expression((UnaryExpression *) expression->enclosed_expression, file);
  break;
  default:
    EXPR_ERROR();
  }
}

#endif // end EXPR_HEADER