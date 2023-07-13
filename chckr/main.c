#include "../prsr/prsr.h"
#include "strct_decl_chckr.h"
#include "../utility/file_man.h"

int main() {

  char *code = file_to_cstring("/home/matteo/github/cfg/chckr/code.b");

  ASTNode *ast = prsr_parse(code);

  if (strct_decl_chckr_check(ast))
    fprintf(stdout, "all checks passed!\n");

  ast_print_node(ast, stdout); fprintf(stdout, "\n");
  ast_dealloc_node(ast);

  munmap(code, 0);

  return 0;
}