#pragma once

#include "../expr/funccall.h"
#include "avlb_vars.h"
#include "../prsr/func_decl.h"
#include "../prsr/strct_decl.h"

#define FUNCCALL_CHCKR_PARAMS                 \
  FunctionCall *funccall,                     \
  AvailableVariables *av_vars,                \
  FunctionDeclarationList *functions,         \
  StructDeclarationList *structs


bool funccall_chckr_check(FUNCCALL_CHCKR_PARAMS);

bool funccall_chckr_check(FUNCCALL_CHCKR_PARAMS) {

  // check existance of function
  bool function_found = False;
  FunctionDeclaration *matched_function = NULL;
  FOR_EACH(FunctionDeclarationList, func_it, functions) {
    if (idf_equal_identifiers(func_it->node->name, funccall->function_name)) {
      function_found = True;
      matched_function = func_it->node;
      break;
    }
  }
  if (!function_found) {
    fprintf(stdout, "ERROR, did not pass function call analysis. The function %s(...) is never declared\n", funccall->function_name->name);
    return False;
  }

  // check number of params is correct
  size_t func_decl_params_size = prmt_list_size(matched_function->params);
  size_t funccall_params_size = expr_list_size(funccall->params_values);
  if (func_decl_params_size != funccall_params_size) {
    fprintf(stdout, 
            "ERROR, did not pass function call analysis. The function %s requires %lu parameters, but %lu where passed\n", 
            matched_function->name->name, 
            func_decl_params_size, 
            funccall_params_size
            );
    return False;
  }

  // TODO: check types of params passed are correct
  (void) structs;
  (void) av_vars;

  return True;
}