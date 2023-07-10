#ifndef FUNCCALL_HEADER
#define FUNCCALL_HEADER

#include "expr_interface.h"

#define FUNCCALL_ERROR() { fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1); }
#define if_null_print(ptr, file) if (ptr == NULL) { fprintf(file, "NULL"); return; }


typedef struct {
  char *function_name;
  ExpressionList *params_values;
} FunctionCall;

FunctionCall *funccall_create(const char *name, ExpressionList *params_values){
  FunctionCall *func_call = (FunctionCall *)malloc(sizeof(FunctionCall));
  func_call->function_name = (char *)malloc(sizeof(strlen(name))+1);
  strcpy(func_call->function_name, name);
  func_call->params_values = params_values;
  return func_call;
}

void funccall_dealloc(FunctionCall *func_call) {
  if (func_call == NULL)
    return;
  free(func_call->function_name);
  expr_list_dealloc(func_call->params_values);
  free(func_call);
}

void funccall_print(FunctionCall *func_call, FILE *file) {
  if_null_print(func_call, file);
  fprintf(file, "func:%s(", func_call->function_name);
  expr_list_print(func_call->params_values, file);
  fprintf(file, ")");
}

#endif // end FUNCCALL_HEADER