#include "lxr.h"
#include <stdlib.h>
#include <stdio.h>

int main() {

  lxr_t lexer = lxr_init("{\n  if bar <= 7; \n  # this is a comment\n}");

  lxr_dump_lexer(&lexer);

  token_t next_token = lxr_next_token(&lexer); 

  while(next_token.type != END_TOKEN) {
    lxr_print_token(next_token);
    fprintf(stdout, "\n");
    next_token = lxr_next_token(&lexer); 
  }

  return 0;
}