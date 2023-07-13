#include "prsr.h"
#include <stdlib.h>
#include <stdio.h>
#include "../utility/file_man.h"

int main() {

  char *code = file_to_cstring("/home/matteo/github/cfg/chckr/code.b");

  ASTNode *ast = prsr_parse(code);

  ast_print_node(ast, stdout);
  ast_dealloc_node(ast);

  munmap(code, 0);
  return 0;
}