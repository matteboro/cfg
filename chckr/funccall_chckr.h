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


// TODO: here is where I should modify the code to permit overloading of functions
bool funccall_chckr_check(FUNCCALL_CHCKR_PARAMS) {
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
  ExpressionList *params_values = funccall->params_values;
  size_t func_decl_params_size = prmt_list_size(matched_function->params);
  size_t funccall_params_size = expr_list_size(params_values);
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

  if (funccall_params_size == 0)
    return True;

  FOR_EACH_ENUM(ParameterList, prmt_it, matched_function->params, counter) {
    Type *param_type = prmt_it->node->nt_bind->type;
    Expression *init_expr = expr_list_get_at(params_values, counter);
    Type *init_expr_type = expr_chckr_get_returned_type(init_expr, an_state);

    if (init_expr_type == NULL) {
      FUNCCALL_CHCKR_ERROR_HEADER();
      fprintf(stdout, "initialize expression : ");
      expr_print_expression(init_expr, stdout);
      fprintf(stdout, ", for paramter n.%lu: ", counter);
      prmt_print(prmt_it->node, stdout);
      fprintf(stdout, ", does not yield a valid return type\n\n");

      return False;
    }
    if (!type_equal(param_type, init_expr_type)) {
      FUNCCALL_CHCKR_ERROR_HEADER();
      fprintf(stdout, "initialize expression : ");
      expr_print_expression(init_expr, stdout);
      fprintf(stdout, ", for paramter n.%lu: ", counter);
      prmt_print(prmt_it->node, stdout);
      fprintf(stdout, " does not yield a correct return type: expected -> ");
      type_print(param_type, stdout);
      fprintf(stdout,", received -> ");
      type_print(init_expr_type, stdout);
      fprintf(stdout, "\n\n");

      type_dealloc(init_expr_type);
      return False;
    }
    type_dealloc(init_expr_type);
  }

  return True;
}