#pragma once

#include "../expr/funccall.h"
#include "chckr_env.h"

#define FUNCCALL_CHCKR_PARAMS                 \
  FunctionCall *funccall,                     \
  ASTCheckingAnalysisState *an_state

#define FUNCCALL_CHCKR_ERROR_HEADER() \
  fprintf(stdout, "ERROR: did not pass declaration function call analysis.\n\n  In function call: \n    "); \
  funccall_print(funccall, stdout); fprintf(stdout, ",\n  ");


// FORWARD DECLARATIONS
Type *expr_chckr_get_returned_type(Expression *expr, ASTCheckingAnalysisState *an_state);


bool funccall_chckr_check(FUNCCALL_CHCKR_PARAMS);

bool funccall_chckr_check(FUNCCALL_CHCKR_PARAMS) {
  AvailableVariables* av_vars = chckr_analysis_state_get_av_vars(an_state); 
  StructDeclarationList* structs = chckr_analysis_state_get_structs(an_state); 
  FunctionDeclarationList* functions = chckr_analysis_state_get_functions(an_state);

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
    FUNCCALL_CHCKR_ERROR_HEADER();
    fprintf(stdout, "the function %s(...) is never declared\n", funccall->function_name->name);
    return False;
  }

  // check number of params is correct
  size_t func_decl_params_size = prmt_list_size(matched_function->params);
  size_t funccall_params_size = expr_list_size(funccall->params_values);
  if (func_decl_params_size != funccall_params_size) {
    FUNCCALL_CHCKR_ERROR_HEADER();
    fprintf(stdout, 
            "the function %s requires %lu parameters, but %lu where passed\n", 
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