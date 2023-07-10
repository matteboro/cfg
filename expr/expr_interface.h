#ifndef EXPR_INTERFACE_HEADER
#define EXPR_INTERFACE_HEADER

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
} Expression;

struct ExpressionList_s;
typedef struct ExpressionList_s ExpressionList;

ExpressionList *expr_list_create_empty();
ExpressionList *expr_list_create(Expression *expression);
void expr_list_append(ExpressionList *list, Expression *expression);
void expr_list_dealloc(ExpressionList *list);
size_t expr_list_size(ExpressionList *list);
Expression *expr_list_get_at(ExpressionList *list, size_t index);
void expr_list_print(ExpressionList *list, FILE *file);

void expr_dealloc_expression(Expression *expression);
void expr_print_expression(Expression *expression, FILE *file);

#endif // end EXPR_INTERFACE_HEADER