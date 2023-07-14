#include "lxr.h"
#include <stdlib.h>
#include <stdio.h>
#include "../utility/file_man.h"

int main() {

  char *code = file_to_cstring("/home/matteo/github/cfg/chckr/code.b");

  Lexer lexer = lxr_init(code);

  lxr_dump_lexer(&lexer);

  Token next_token = lxr_next_token(&lexer); 

  while(next_token.type != END_TOKEN) {
    lxr_print_token(next_token);
    fprintf(stdout, "\n");
    next_token = lxr_next_token(&lexer); 
  }

  munmap(code, 0);
  return 0;
}