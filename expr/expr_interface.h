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
} ExpressionType;

typedef void EnclosedExpression;

typedef struct {
  ExpressionType type;
  EnclosedExpression *enclosed_expression;
  FileInfo file_info;
} Expression;

void expr_dealloc_expression(Expression *expression);
void expr_print_expression(Expression *expression, FILE *file);

LIST(expr, Expression, expr_dealloc_expression, expr_print_expression)
DEFAULT_LIST_MERGED_FILE_INFO(expr, Expression)
