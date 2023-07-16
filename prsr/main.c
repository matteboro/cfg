#include "prsr.h"
#include <stdlib.h>
#include <stdio.h>
#include "../utility/file_man.h"

int main() {

  char *code = file_to_cstring("/home/matteo/github/cfg/chckr/code.b");

  ASTProgram *ast = prsr_parse(code);

  //strct_decl_list_print(ast->struct_declarations, stdout);

  prgrm_print(ast, stdout);
  prgrm_dealloc(ast);

  munmap(code, 0);
  return 0;
}