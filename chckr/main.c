#include "../prsr/prsr.h"
#include "prgrm_chckr.h"
#include "../utility/file_man.h"

int main() {

  File *file = file_open("/home/matteo/github/cfg/chckr/code.b");

  ASTProgram *ast = prsr_parse(file);

  prgrm_chckr_check(ast);

  prgrm_print(ast, stdout); fprintf(stdout, "\n");
  prgrm_dealloc(ast);

  file_dealloc(file);

  return 0;
}