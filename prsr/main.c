#include "prsr.h"
#include <stdlib.h>
#include <stdio.h>

int main() {

/*
  ASTNode *ass1 = 
    ast_create_assignment_node(
      idf_create_identifier("bar"), 
      ast_create_expression_node(
        prsr_parse_expression_from_string("bar * 69")
      ));

  ASTNode *ass2 = 
    ast_create_assignment_node(
      idf_create_identifier("foo"), 
      ast_create_expression_node(
        prsr_parse_expression_from_string("(baz - 420) * 69")
      ));

  ASTNodeList *node_list = ast_list_create_empty();
  ast_list_append(node_list, ass1);
  ast_list_append(node_list, ass2);

  ASTNode *list_node = ast_create_node_list_node(node_list);
  
  ast_print_node(list_node, stdout); fprintf(stdout, "\n");
  ast_dealloc_node(list_node);
*/
  // ast_list_print(node_list, stdout);
  // ast_list_dealloc(node_list);

  /*
  const char *data = "foo = 2 + bar(4, (boo * 3 + z), 5) * 5;";
  ASTNode *ast = prsr_parse(data);

  if (ast != NULL) 
    fprintf(stdout, "%s parsed succesfully\n", data);
  else 
    fprintf(stdout, "%s parsed unsuccesfully\n", data);

  ast_print_node(ast, stdout);
  ast_dealloc_node(ast);
*/
/*
  ParameterList *list = prmt_list_create_empty();
  prmt_list_print(list, stdout); fprintf(stdout, "\n");

  prmt_list_append(list, prmt_create_integer_param(69));
  prmt_list_print(list, stdout); fprintf(stdout, "\n");

  prmt_list_append(list, prmt_create_identifer_param(idf_create_identifier("foo")));
  prmt_list_print(list, stdout); fprintf(stdout, "\n");

  prmt_list_append(list, prmt_create_expression_param(prsr_parse_expression_from_string("(foo - 6) * 7 + bar")));
  prmt_list_print(list, stdout); fprintf(stdout, "\n");

  fprintf(stdout, "size: %zu\n", prmt_list_size(list));

  fprintf(stdout, "param 0: "); prmt_print_param(prmt_list_get_at(list, 0), stdout); fprintf(stdout, "\n");
  fprintf(stdout, "param 1: "); prmt_print_param(prmt_list_get_at(list, 1), stdout); fprintf(stdout, "\n");
  fprintf(stdout, "param 2: "); prmt_print_param(prmt_list_get_at(list, 2), stdout); fprintf(stdout, "\n");

  FunctionCall *func_call = funccall_create("baz", list);

  Expression *expr = expr_create_binary_expression(
    expr_create_funccall_operand_expression(func_call),
    MULT_OPERATION,
    expr_create_operand_expression(INTEGER_OPERAND, "69"));

  fprintf(stdout, "expr: "); expr_print_expression(expr, stdout); fprintf(stdout, "\n");

  expr_dealloc_expression(expr);

  // funccall_print(func_call, stdout); fprintf(stdout, "\n");
  // funccall_dealloc(func_call);
*/

  ASTNode *ast = prsr_parse(" \
  func baz(a, b, c) { \
    var boo = (foo + 69) * 420; \
    while boo + 7 { \
      var z = 10; \
    } \
    boo = 420 * 69; \
    if j * t - 420 { \
      var doodoo; \
      bich = 33 * tree; \
    } \
  }");

  ast_print_node(ast, stdout); fprintf(stdout, "\n");
  ast_dealloc_node(ast);

  return 0;
}