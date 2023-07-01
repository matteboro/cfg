#ifndef EXPR_HEADER
#define EXPR_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../lxr/lxr.h"


// #define EXPR_DEBUG
#ifdef EXPR_DEBUG
#define EXPR_DEBUG_PRINT() fprintf(stdout, "%s\n", __FUNCTION__);
#else
#define EXPR_DEBUG_PRINT() 
#endif

#define EXPR_ERROR() fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1);

// FORWARD DECLARATIONS

typedef enum {
  BINARY_EXPRESSION_EXP_TYPE,
  OPERAND_EXP_TYPE,
} ExpressionType;

typedef void EnclosedExpression;

typedef struct {
  ExpressionType type;
  EnclosedExpression *enclosed_expression;
} Expression;

void expr_dealloc_expression(Expression *expression);
void expr_print_expression(Expression *expression, FILE *file);

// IDENTIFIER

typedef struct {
  char *name;
} Identifier;

Identifier expr_create_identifier(const char *name) {
  Identifier id;
  id.name = malloc(strlen(name)+1);
  strcpy(id.name, name);
  return id;
}

void expr_dealloc_identifier(Identifier *id) {
  free(id->name);
  free(id);
}

void expr_print_identifier(Identifier *id, FILE *file) {
  fprintf(file, "id.%s", id->name);
} 

Identifier *expr_create_identifier_from_token(Token token) {
  // TODO : error handling, assume (token.type == IDENTIFIER_TOKEN)

  Identifier *id = (Identifier *)malloc(sizeof(Identifier));
  id->name = malloc(token.data_length+1);
  id->name = lxr_get_token_data_as_cstring(token);
  return id;
}

// END IDENTIFIER

// PARAMETER

typedef enum {
  IDENTIFIER_PARAM,
  INTEGER_PARAM,
  EXPRESSION_PARAM,
  FUNCCALL_PARAM,
} ParameterType;

typedef struct {
  ParameterType type;
  void* param;
} Parameter;

//// CREATE

Parameter *prmt_create_identifer_param(Identifier *id) {
  Parameter *param = (Parameter *) malloc(sizeof(Parameter));
  param->type = IDENTIFIER_PARAM;
  param->param = id;
  return param;
}

Parameter *prmt_create_expression_param(Expression *expr) {
  Parameter *param = (Parameter *) malloc(sizeof(Parameter));
  param->type = EXPRESSION_PARAM;
  param->param = expr;
  return param;
}

Parameter *prmt_create_integer_param(int i) {
  Parameter *param = (Parameter *) malloc(sizeof(Parameter));
  param->type = INTEGER_PARAM;
  param->param = malloc(sizeof(int));
  *((int *)param->param) = i;
  return param;
}

//// DEALLOC

void prmt_dealloc_identifer_param(Parameter *param) {
  Identifier *id = (Identifier *) param->param;
  expr_dealloc_identifier(id);
}

void prmt_dealloc_expression_param(Parameter *param) {
  Expression *expr = (Expression *) param->param;
  expr_dealloc_expression(expr);
}

void prmt_dealloc_integer_param(Parameter *param) {
  free(param->param);
}

void prmt_dealloc_param(Parameter *param) {
  switch (param->type) {
  case IDENTIFIER_PARAM:
    prmt_dealloc_identifer_param(param); break;
  case EXPRESSION_PARAM:
    prmt_dealloc_expression_param(param); break;
  case INTEGER_PARAM:
    prmt_dealloc_integer_param(param); break;
  default:
    EXPR_ERROR();
  }
  free(param);
}

//// PRINT

void prmt_print_identifer_param(Parameter *param, FILE *file) {
  Identifier *id = (Identifier *) param->param;
  expr_print_identifier(id, file);
}

void prmt_print_expression_param(Parameter *param, FILE *file) {
  Expression *expr = (Expression *) param->param;
  expr_print_expression(expr, file);
}

void prmt_print_integer_param(Parameter *param, FILE *file) {
  fprintf(file, "%d", *((int *)param->param));
}

void prmt_print_param(Parameter *param, FILE *file) {
  if (param == NULL) {
    fprintf(file, "NULL");
    return;
  }
  switch (param->type) {
  case IDENTIFIER_PARAM:
    prmt_print_identifer_param(param, file); break;
  case EXPRESSION_PARAM:
    prmt_print_expression_param(param, file); break;
  case INTEGER_PARAM:
    prmt_print_integer_param(param, file); break;
  default:
    EXPR_ERROR();
  }
}

// END PARAMETER

// PARAMTER LIST

typedef struct ParameterList_s{
  struct ParameterList_s *next;
  Parameter *param;
} ParameterList;

//// CREATE EMPTY

ParameterList *prmt_list_create_empty() {
  ParameterList *list = (ParameterList *)malloc(sizeof(ParameterList));
  list->next = NULL;
  list->param = NULL;
  return list;
  // return NULL;
}

ParameterList *prmt_list_create(Parameter *param) {
  ParameterList *list = (ParameterList *)malloc(sizeof(ParameterList));
  list->next = NULL;
  list->param = param;
  return list;
}

//// APPEND

void prmt_list_append(ParameterList *list, Parameter *param) {
  if (list->next == NULL) {
    if (list->param == NULL) {
      list->param = param;
    } else {
      list->next = prmt_list_create(param);
    }
    return;
  }

  ParameterList *n = list->next;
  while(n->next != NULL)
    n = n->next;

  n->next = prmt_list_create(param);
  return;
}

//// DEALLOC

void prmt_list_dealloc(ParameterList *list) {
  if (list->next != NULL)
    prmt_list_dealloc(list->next);
  
  if (list->param != NULL)
    prmt_dealloc_param(list->param);
  
  free(list);
}

//// SIZE

size_t prmt_list_size(ParameterList *list) {
  if (list->next != NULL)
    return prmt_list_size(list->next) + 1;
  
  if (list->param != NULL)
    return 1;
  
  return 0;
}

//// GET AT

Parameter *prmt_list_get_at(ParameterList *list, size_t index) {
  if (list->next == NULL && list->param == NULL && index == 0)
    return NULL;
  assert(index < prmt_list_size(list));
  ParameterList *n = list;
  for (size_t i=0; i<index; ++i)
    n = n->next;
  return n->param;
}

//// PRINT

void prmt_list_print(ParameterList *list, FILE *file) {
  if (list->param != NULL)
    prmt_print_param(list->param, file);
  
  if (list->next != NULL){
    fprintf(file, ", ");
    prmt_list_print(list->next, file);
  }
}

// END PARAMETER LIST

// FUNCCALL

/*
  - a Parameter can be a Integer, Identifier, FuncCall, Expression,
  - a FuncCall has a list of Parameter,
  - an Operand can be a FuncCall, Integer, Identifier
*/

// END FUNCCALL

// DEFINITIONS

typedef enum {
  IDENTIFIER_OPERAND,
  INTEGER_OPERAND,
  FUNCCALL_OPERAND,
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

typedef struct {
  OperandType type;
  void *data;
} Operand;

typedef struct {
  Expression *left, *right;
  OperationType operation;
} BinaryExpression;

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

#define expr_string_to_int(string) atoi(string)

Operand expr_create_operand(OperandType type, void *data) {
  Operand operand = {type, data};
  return operand;
}

Expression *expr_create_operand_expression(OperandType type, const char *data) {
  void *my_data = NULL;

  if (type == IDENTIFIER_OPERAND) {
    my_data = malloc(sizeof(Identifier));
    Identifier *id_data = (Identifier *)my_data;
    *id_data = expr_create_identifier(data);
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
  char *token_data_string = lxr_get_token_data_as_cstring(token);

  switch (token.type) {
  case IDENTIFIER_TOKEN: type = IDENTIFIER_OPERAND; break;
  case INTEGER_TOKEN: type = INTEGER_OPERAND; break;
  default: EXPR_ERROR();
  }

  Expression* result = expr_create_operand_expression(type, token_data_string);
  free(token_data_string);
  return result;
}

// DEALLOC

void expr_dealloc_operand(Operand *operand) {
  EXPR_DEBUG_PRINT()
  if (operand == NULL) 
    return;
  
  if (operand->type == IDENTIFIER_OPERAND) 
    expr_dealloc_identifier((Identifier *) operand->data);
  else
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
  case BINARY_EXPRESSION_EXP_TYPE: {
    BinaryExpression *binary_expression = (BinaryExpression *) (expression->enclosed_expression);
    expr_dealloc_expression(binary_expression->left);
    expr_dealloc_expression(binary_expression->right);
    expr_dealloc_binary_expression((BinaryExpression *) (expression->enclosed_expression));
  } break;
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
    expr_print_identifier((Identifier *)operand->data, file);
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

#endif // end EXPR_HEADER