#include "prsr.h"
#include <stdlib.h>
#include <stdio.h>

int main() {

/*
  const char *data = "foo = 2 + bar * 5;";
  ASTNode *ast = prsr_parse(data);

  if (ast != NULL) 
    fprintf(stdout, "%s parsed succesfully\n", data);
  else 
    fprintf(stdout, "%s parsed unsuccesfully\n", data);

  ast_print_node(ast, stdout);
  ast_dealloc_node(ast);
*/

  ParameterList *list = prmt_list_create_empty();
  prmt_list_print(list, stdout); fprintf(stdout, "\n");
  prmt_list_append(list, prmt_create_integer_param(69));
  prmt_list_print(list, stdout); fprintf(stdout, "\n");

  Identifier *id = (Identifier *)malloc(sizeof(Identifier));
  *id = expr_create_identifier("foo");
  prmt_list_append(list, prmt_create_identifer_param(id));
  prmt_list_print(list, stdout); fprintf(stdout, "\n");

  Expression *expr = prsr_parse_expression_from_string("(foo - 6) * 7 + bar");
  prmt_list_append(list, prmt_create_expression_param(expr));

  prmt_list_print(list, stdout); fprintf(stdout, "\n");
  fprintf(stdout, "size: %zu\n", prmt_list_size(list));

  fprintf(stdout, "param 0: "); prmt_print_param(prmt_list_get_at(list, 0), stdout); fprintf(stdout, "\n");
  fprintf(stdout, "param 1: "); prmt_print_param(prmt_list_get_at(list, 1), stdout); fprintf(stdout, "\n");
  fprintf(stdout, "param 2: "); prmt_print_param(prmt_list_get_at(list, 2), stdout); fprintf(stdout, "\n");
  // fprintf(stdout, "param 3: "); prmt_print_param(prmt_list_get_at(list, 3), stdout); fprintf(stdout, "\n");
  prmt_list_dealloc(list);

  return 0;
}