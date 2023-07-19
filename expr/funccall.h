#pragma once

#include "expr_interface.h"
#include "idf.h"

#define FUNCCALL_ERROR() { fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1); }
#define if_null_print(ptr, file) if (ptr == NULL) { fprintf(file, "NULL"); return; }

typedef struct {
  Identifier *function_name;
  ExpressionList *params_values;
  FileInfo file_info;
} FunctionCall;

FunctionCall *funccall_create(Identifier *func_name, ExpressionList *params_values, FileInfo file_info){
  FunctionCall *func_call = (FunctionCall *)malloc(sizeof(FunctionCall));
  func_call->function_name = func_name;
  func_call->params_values = params_values;
  func_call->file_info = file_info;
  return func_call;
}

void funccall_dealloc(FunctionCall *func_call) {
  if (func_call == NULL)
    return;
  idf_dealloc_identifier(func_call->function_name);
  expr_list_dealloc(func_call->params_values);
  free(func_call);
}

void funccall_print(FunctionCall *func_call, FILE *file) {
  if_null_print(func_call, file);
  fprintf(file, "func:%s(", func_call->function_name->name);
  expr_list_print(func_call->params_values, file);
  fprintf(file, ")");
}
