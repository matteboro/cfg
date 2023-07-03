#include "lxr.h"
#include <stdlib.h>
#include <stdio.h>

int main() {

  Lexer lexer = lxr_init(" \
  func baz(a, b, c) { \
    var boo = (foo + \"hello\") * 420; \
    while boo + 7 { \
      var z = 10; \
      arr[10] z; \
    } \
  }\
  var string = \"hello, world\"; \
  ");

  lxr_dump_lexer(&lexer);

  Token next_token = lxr_next_token(&lexer); 

  while(next_token.type != END_TOKEN) {
    lxr_print_token(next_token);
    fprintf(stdout, "\n");
    next_token = lxr_next_token(&lexer); 
  }

  return 0;
}