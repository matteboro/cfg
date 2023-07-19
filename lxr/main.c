#include "lxr.h"
#include <stdlib.h>
#include <stdio.h>
#include "../utility/file_man.h"

int main() {

  File *file = file_open("/home/matteo/github/cfg/chckr/code.b");

  Lexer lexer = lxr_init(file);

  lxr_dump_lexer(&lexer);

  Token next_token = lxr_next_token(&lexer); 

  while(next_token.type != END_TOKEN) {
    lxr_print_token(next_token);
    fprintf(stdout, "\n");
    next_token = lxr_next_token(&lexer); 
  }

  file_dealloc(file);
  return 0;
}