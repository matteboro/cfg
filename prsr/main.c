#include "prsr.h"
#include <stdlib.h>
#include <stdio.h>
#include "../utility/file_man.h"

int main() {

  File *file = file_open("/home/matteo/github/cfg/chckr/code.b");

  ASTProgram *ast = prsr_parse(file);

  //strct_decl_list_print(ast->struct_declarations, stdout);

  prgrm_print(ast, stdout);
  prgrm_dealloc(ast);

  file_dealloc(file);
  return 0;
}