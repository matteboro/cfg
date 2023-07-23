#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

// FORWARD DECLARATIONS

typedef enum {
  BINARY_EXPRESSION_EXP_TYPE,
  UNARY_EXPRESSION_EXP_TYPE,
  OPERAND_EXP_TYPE,
  CREATE_EXP_TYPE,
} ExpressionType;

typedef void EnclosedExpression;

typedef struct {
  ExpressionType type;
  EnclosedExpression *enclosed_expression;
  FileInfo file_info;
  Type *real_type;
} Expression;

void expr_set_real_type(Expression *expression, Type *real_type) {
  expression->real_type = real_type;
}

void expr_dealloc_expression(Expression *expression);
void expr_print_expression(Expression *expression, FILE *file);
Expression *expr_create_expression(ExpressionType type, void *enclosed_expression, FileInfo file_info);

LIST(expr, Expression, expr_dealloc_expression, expr_print_expression)
DEFAULT_LIST_MERGED_FILE_INFO(expr, Expression)
