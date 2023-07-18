#include "../prsr/prsr.h"
#include "prgrm_chckr.h"
#include "../utility/file_man.h"

int main() {

  char *code = file_to_cstring("/home/matteo/github/cfg/chckr/code.b");

  ASTProgram *ast = prsr_parse(code);

  prgrm_chckr_check(ast);

  prgrm_print(ast, stdout); fprintf(stdout, "\n");
  prgrm_dealloc(ast);

  munmap(code, 0);

  return 0;
}