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

// EXPRESSION LIST

struct ExpressionList_s{
  struct ExpressionList_s *next;
  Expression *expression;
};

//// CREATE EMPTY

ExpressionList *expr_list_create_empty() {
  ExpressionList *list = (ExpressionList *)malloc(sizeof(ExpressionList));
  list->next = NULL;
  list->expression = NULL;
  return list;
  // return NULL;
}

ExpressionList *expr_list_create(Expression *expression) {
  ExpressionList *list = (ExpressionList *)malloc(sizeof(ExpressionList));
  list->next = NULL;
  list->expression = expression;
  return list;
}

//// APPEND

void expr_list_append(ExpressionList *list, Expression *expression) {
  if (list->next == NULL) {
    if (list->expression == NULL) {
      list->expression = expression;
    } else {
      list->next = expr_list_create(expression);
    }
    return;
  }

  ExpressionList *n = list->next;
  while(n->next != NULL)
    n = n->next;

  n->next = expr_list_create(expression);
  return;
}

//// DEALLOC

void expr_list_dealloc(ExpressionList *list) {
  if (list == NULL)
    return;
  if (list->next != NULL)
    expr_list_dealloc(list->next);
  if (list->expression != NULL)
    expr_dealloc_expression(list->expression);
  free(list);
}

//// SIZE

size_t expr_list_size(ExpressionList *list) {
  if (list->next != NULL)
    return expr_list_size(list->next) + 1;
  
  if (list->expression != NULL)
    return 1;
  
  return 0;
}

//// GET AT

Expression *expr_list_get_at(ExpressionList *list, size_t index) {
  if (list->next == NULL && list->expression == NULL && index == 0)
    return NULL;
  assert(index < expr_list_size(list));
  ExpressionList *n = list;
  for (size_t i=0; i<index; ++i)
    n = n->next;
  return n->expression;
}

//// PRINT

void expr_list_print(ExpressionList *list, FILE *file) {
  if_null_print(list, file);
  if (list->expression != NULL)
    expr_print_expression(list->expression, file);
  
  if (list->next != NULL){
    fprintf(file, ", ");
    expr_list_print(list->next, file);
  }
}

// END EXPRESSION LIST

/*
  - a Parameter can be a Integer, Identifier, FuncCall, Expression,
  - a FuncCall has a list of Parameter,
  - an Operand can be a FuncCall, Integer, Identifier
*/

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

// CREATE

Expression *expr_create_expression(ExpressionType type, void *enclosed_expression) {
  Expression *expression = (Expression *)malloc(sizeof(Expression));
  expression->type = type;
  expression->enclosed_expression = (EnclosedExpression *) enclosed_expression;
  return expression;
}

Expression *expr_create_binary_expression(Expression *left, OperationType op_type, Expression *right) {
  BinaryExpression *binary_expression = (BinaryExpression *)malloc(sizeof(BinaryExpression));
  binary_expression->left = left;
  binary_expression->right = right;
  binary_expression->operation = op_type;
  return expr_create_expression(BINARY_EXPRESSION_EXP_TYPE, binary_expression);
}

Expression *expr_create_unary_expression(Expression *operand, OperationType op_type) {
  UnaryExpression *unary_expression = (UnaryExpression *)malloc(sizeof(UnaryExpression));
  unary_expression->operand = operand;
  unary_expression->operation = op_type;
  return expr_create_expression(UNARY_EXPRESSION_EXP_TYPE, unary_expression);
}

Expression *expr_create_funccall_operand_expression(FunctionCall *func_call) {
  Operand *operand = oprnd_create_operand(FUNCCALL_OPERAND, func_call);
  return expr_create_expression(OPERAND_EXP_TYPE, operand);
}

Expression *expr_create_array_deref_operand_expression(Identifier *id, Expression *index) {
  Operand *operand = 
    oprnd_create_operand(
      ARRAY_DEREF_OPERAND, 
      oprnd_create_array_deref_operand_data(id, index));
  return expr_create_expression(OPERAND_EXP_TYPE, operand);
}

Expression *expr_create_object_deref_operand_expression(ObjectDerefList *derefs) {
  Operand *operand = 
    oprnd_create_operand(
      OBJ_DEREF_OPERAND, 
      oprnd_create_object_deref_operand_data(derefs));
  return expr_create_expression(OPERAND_EXP_TYPE, operand);
}

Expression *expr_create_operand_expression(OperandType type, const char *data) {
  void *my_data = NULL;

  if (type == IDENTIFIER_OPERAND) {
    my_data = idf_create_identifier(data);
  } 
  else if (type == INTEGER_OPERAND) {
    my_data = malloc(sizeof(int));
    int *int_data = (int *)my_data;
    *int_data = expr_string_to_int(data);
  } 
  else if (type == STRING_OPERAND) {
    my_data = malloc(strlen(data)+1);
    strcpy((char *) my_data, data);
  } 
  else {
    EXPR_ERROR();
  }

  Operand *operand = oprnd_create_operand(type, my_data);
  return expr_create_expression(OPERAND_EXP_TYPE, operand);
}

Expression *expr_create_operand_expression_from_token(Token token) {
  OperandType type;
  char *token_data_string = lxr_get_token_data_as_cstring(token);

  switch (token.type) {
  case IDENTIFIER_TOKEN: type = IDENTIFIER_OPERAND; break;
  case INTEGER_TOKEN: type = INTEGER_OPERAND; break;
  case STRING_TOKEN: type = STRING_OPERAND; break;
  default: EXPR_ERROR();
  }

  Expression* result = expr_create_operand_expression(type, token_data_string);
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

void expr_dealloc_expression(Expression *expression) {
  EXPR_DEBUG_PRINT()
  if (expression == NULL) 
    return;
  switch (expression->type) {
  case OPERAND_EXP_TYPE:
    oprnd_dealloc_operand((Operand *) (expression->enclosed_expression));
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
  fprintf(file, "(");
  if (expression->operation == STR_LEN_UNARY_OPERATION) {
    fprintf(file, "< ");
    expr_print_expression(expression->operand, file);
    fprintf(file, " >");
  } else {
    fprintf(file, "%s ", operation_to_char[expression->operation]);
    expr_print_expression(expression->operand, file);
  }
  fprintf(file, ")");
}

void expr_print_expression(Expression *expression, FILE *file) {
  EXPR_DEBUG_PRINT()
  if (expression == NULL) 
    return;
  switch (expression->type) {
  case OPERAND_EXP_TYPE:
    oprnd_print_operand((Operand *) expression->enclosed_expression, file);
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