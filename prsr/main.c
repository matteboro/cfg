#include "prsr.h"
#include <stdlib.h>
#include <stdio.h>

int main() {

  const char *data = "foo = 2 + bar * 5;";
  ASTNode *ast = prsr_parse(data);

  if (ast != NULL) 
    fprintf(stdout, "%s parsed succesfully\n", data);
  else 
    fprintf(stdout, "%s parsed unsuccesfully\n", data);

  ast_print_node(ast, stdout);

  return 0;
}