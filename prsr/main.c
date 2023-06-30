#include "prsr.h"
#include <stdlib.h>
#include <stdio.h>

int main() {

  const char *data = "2 + 4 * 5 * 6 - 7";
  Expression *expr = prsr_parse_only_expression(data);

  if (expr != NULL) 
    fprintf(stdout, "%s parsed succesfully\n", data);
  else 
    fprintf(stdout, "%s parsed unsuccesfully\n", data);

  expr_print_expression(expr, stdout);
  fprintf(stdout, "\n");
  expr_dealloc_expression(expr);

  return 0;
}