#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../lxr/lxr.h"


// #define EXPR_DEBUG
#ifdef EXPR_DEBUG
#define EXPR_DEBUG_PRINT() fprintf(stdout, "%s\n", __FUNCTION__);
#else
#define EXPR_DEBUG_PRINT() 
#endif

#define EXPR_ERROR() fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1);


typedef enum {
  BINARY_EXPRESSION_EXP_TYPE,
  OPERAND_EXP_TYPE,
} ExpressionType;

typedef enum {
  IDENTIFIER_OPERAND,
  INTEGER_OPERAND,
} OperandType;

typedef enum {
  SUM_OPERATION,
  SUB_OPERATION,
  MULT_OPERATION,
  DIV_OPERATION,
} OperationType;

static const char operation_to_char[] = {
  [SUM_OPERATION] = '+',
  [SUB_OPERATION] = '-',
  [MULT_OPERATION] = '*',
  [DIV_OPERATION] = '/',
};

typedef void EnclosedExpression;

typedef struct {
  ExpressionType type;
  EnclosedExpression *enclosed_expression;
} Expression;


typedef struct {
  OperandType type;
  void *data;
} Operand;

typedef struct {
  Expression *left, *right;
  OperationType operation;
} BinaryExpression;

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

#define expr_string_to_int(string) atoi(string)

Operand expr_create_operand(OperandType type, void *data) {
  Operand operand = {type, data};
  return operand;
}

Expression *expr_create_operand_expression(OperandType type, void *data) {
  void *my_data = NULL;

  if (type == IDENTIFIER_OPERAND) {
    int len = strlen(data);
    my_data = malloc(len+1);
    strcpy(my_data, data);
  } 
  else if (type == INTEGER_OPERAND) {
    my_data = malloc(sizeof(int));
    int *int_data = (int *)my_data;
    *int_data = expr_string_to_int(data);
  } 
  else {
    EXPR_ERROR();
  }

  Operand *operand = (Operand *) malloc(sizeof(Operand));
  *operand = expr_create_operand(type, my_data);
  return expr_create_expression(OPERAND_EXP_TYPE, operand);
}

Expression *expr_create_operand_expression_from_token(Token token) {
  OperandType type;
  void *data = NULL;
  char *token_data_string = lxr_get_token_data_as_cstring(token);
  switch (token.type) {
  case IDENTIFIER_TOKEN: 
    type = IDENTIFIER_OPERAND;
    data = malloc(token.data_length+1);
    strcpy(data, token_data_string);
  break;
  case INTEGER_TOKEN:
    type = INTEGER_OPERAND;
    data = malloc(sizeof(int)); 
    int *int_data = (int *)data;
    *int_data = expr_string_to_int(token_data_string);
  break;
  default:
    EXPR_ERROR();
  }
  free(token_data_string);
  Operand *operand = (Operand *) malloc(sizeof(Operand));
  *operand = expr_create_operand(type, data);
  return expr_create_expression(OPERAND_EXP_TYPE, operand);
}

// DEALLOC

void expr_dealloc_operand(Operand *operand) {
  EXPR_DEBUG_PRINT()
  if (operand == NULL) 
    return;
  free(operand->data);
  free(operand);
}

void expr_dealloc_binary_expression(BinaryExpression *expression) {
  EXPR_DEBUG_PRINT()
  if (expression == NULL) 
    return;
  free(expression);
}

void expr_dealloc_expression(Expression *expression) {
  EXPR_DEBUG_PRINT()
  if (expression == NULL) 
    return;
  switch (expression->type) {
  case OPERAND_EXP_TYPE:
    expr_dealloc_operand((Operand *) (expression->enclosed_expression));
  break;
  case BINARY_EXPRESSION_EXP_TYPE:
    // BinaryExpression *binary_expression = (BinaryExpression *) (expression->enclosed_expression);
    expr_dealloc_expression(((BinaryExpression *) (expression->enclosed_expression))->left);
    expr_dealloc_expression(((BinaryExpression *) (expression->enclosed_expression))->right);
    expr_dealloc_binary_expression((BinaryExpression *) (expression->enclosed_expression));
  break;
  default:
    EXPR_ERROR();
  }
  free(expression);
}

// PRINT

void expr_print_expression(Expression *expression, FILE *file);

void expr_print_operand(Operand *operand, FILE *file) {
  EXPR_DEBUG_PRINT()
  if (operand == NULL) 
    return;
  switch (operand->type) {
  case INTEGER_OPERAND:
    fprintf(file, "%d", *((int *)operand->data));
  break;
  case IDENTIFIER_OPERAND:
    fprintf(file, "%s", ((char *)operand->data));
  break;
  default:
    EXPR_ERROR();
  }
}

void expr_print_binary_expression(BinaryExpression *expression, FILE *file) {
  EXPR_DEBUG_PRINT()
  if (expression == NULL) 
    return;
  fprintf(file, "(");
  expr_print_expression(expression->left, file);
  fprintf(file, " %c ", operation_to_char[expression->operation]);
  expr_print_expression(expression->right, file);
  fprintf(file, ")");
}

void expr_print_expression(Expression *expression, FILE *file) {
  EXPR_DEBUG_PRINT()
  if (expression == NULL) 
    return;
  switch (expression->type) {
  case OPERAND_EXP_TYPE:
    expr_print_operand((Operand *) expression->enclosed_expression, file);
  break;
  case BINARY_EXPRESSION_EXP_TYPE:
    expr_print_binary_expression((BinaryExpression *) expression->enclosed_expression, file);
  break;
  default:
    EXPR_ERROR();
  }
}