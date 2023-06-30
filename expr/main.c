#include <stdlib.h>
#include <stdio.h>
#include "expr.h"

int main() {

  Expression *left_left = expr_create_operand_expression(IDENTIFIER_OPERAND, "foo");
  Expression *left_right = expr_create_operand_expression(INTEGER_OPERAND, "42");
  Expression *left_bin_expr = expr_create_binary_expression(left_left, MULT_OPERATION, left_right);

  Expression *right_right = expr_create_operand_expression(IDENTIFIER_OPERAND, "bar");
  Expression *right_left = expr_create_operand_expression(INTEGER_OPERAND, "69");
  Expression *right_bin_expr = expr_create_binary_expression(right_left, SUM_OPERATION, right_right);

  Expression *root_bin_expr = expr_create_binary_expression(left_bin_expr, DIV_OPERATION, right_bin_expr);

  expr_print_expression(root_bin_expr, stdout);
  fprintf(stdout, "\n");
  expr_dealloc_expression(root_bin_expr);

  return 0;
}