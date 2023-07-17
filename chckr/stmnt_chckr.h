#pragma once

#include "../prsr/stmnt.h"
#include "../prsr/func_decl.h"
#include "../prsr/strct_decl.h"
#include "funccall_chckr.h"
#include "avlb_vars.h"
#include "type_chckr.h"
#include "obj_drf_chckr.h"
#include "chckr_env.h"

#define STMNT_CHCKR_PARAMS                    \
  Statement *stmnt,                           \
  ASTCheckingAnalysisState *an_state          \

bool stmnt_chckr_check(STMNT_CHCKR_PARAMS);

bool stmnt_chckr_check_assignment(STMNT_CHCKR_PARAMS);
bool stmnt_chckr_check_declaration(STMNT_CHCKR_PARAMS);
bool stmnt_chckr_check_funccall(STMNT_CHCKR_PARAMS);
bool stmnt_chckr_check_return(STMNT_CHCKR_PARAMS);
bool stmnt_chckr_check_block(STMNT_CHCKR_PARAMS);
bool stmnt_chckr_check_if_else(STMNT_CHCKR_PARAMS);
bool stmnt_chckr_check_while(STMNT_CHCKR_PARAMS);


bool stmnt_chckr_check_assignment(STMNT_CHCKR_PARAMS) {
  AvailableVariables* av_vars = chckr_analysis_state_get_av_vars(an_state); 
  StructDeclarationList* structs = chckr_analysis_state_get_structs(an_state); 
  FunctionDeclarationList* functions = chckr_analysis_state_get_functions(an_state);
  (void) stmnt; (void) av_vars; (void) structs; (void) functions;

  AssignableElement *assgnbl = stmnt_assignment_get_assgnbl(stmnt);
  // Expression *value = stmnt_assignment_get_value(stmnt);

  // check dereference
  Type *last_deref_type = obj_drf_chckr_check(assgnbl->obj_derefs, av_vars, structs);
  if(last_deref_type == NULL)
    return False;

  // type_print(last_deref_type, stdout); fprintf(stdout, "\n");

  // TODO: check type of expression match type of dereference object
  //   get type of dereference object < DONE >
  //   get type of expression
  //     compare them

  return True;
}

bool stmnt_chckr_check_declaration(STMNT_CHCKR_PARAMS) {
  AvailableVariables* av_vars = chckr_analysis_state_get_av_vars(an_state); 
  StructDeclarationList* structs = chckr_analysis_state_get_structs(an_state); 
  FunctionDeclarationList* functions = chckr_analysis_state_get_functions(an_state);
  NameTypeBinding *nt_bind = stmnt_declaration_get_nt_bind(stmnt);
  (void) stmnt; (void) av_vars; (void) structs; (void) functions;

  // ExpressionList *init_values = stmnt_declaration_get_init_values(stmnt);

  // check type exists
  if (!type_chckr_type_exists(structs, nt_bind->type)) {
    fprintf(stdout, "ERROR, did not pass declaration statement analysis. The type of %s does not exists\n", nt_bind->name->name);
    return False;
  }

  // check name is available
  if (!avlb_vars_name_available(av_vars, nt_bind->name)) {
    fprintf(stdout, "ERROR, did not pass declaration statement analysis. The name %s is already taken\n", nt_bind->name->name);
    return False;
  }
  avlb_vars_add_var(av_vars, var_create(nt_bind));

  // TODO: check type of expression match type of variable

  return True;
} 

bool stmnt_chckr_check_funccall(STMNT_CHCKR_PARAMS) {
  AvailableVariables* av_vars = chckr_analysis_state_get_av_vars(an_state); 
  StructDeclarationList* structs = chckr_analysis_state_get_structs(an_state); 
  FunctionDeclarationList* functions = chckr_analysis_state_get_functions(an_state);
  (void) stmnt; (void) av_vars; (void) structs; (void) functions;

  FunctionCall *funccall = stmnt_funccall_get_funccall(stmnt);
  return funccall_chckr_check(funccall, av_vars, functions, structs); 
}

bool stmnt_chckr_check_return(STMNT_CHCKR_PARAMS) {
  AvailableVariables* av_vars = chckr_analysis_state_get_av_vars(an_state); 
  StructDeclarationList* structs = chckr_analysis_state_get_structs(an_state); 
  FunctionDeclarationList* functions = chckr_analysis_state_get_functions(an_state);
  (void) stmnt; (void) av_vars; (void) structs; (void) functions;

  Expression *ret_val = stmnt_return_get_ret_value(stmnt);

  // check if the function return type and ret_val type are equal
  Type *func_return_type = stmnt_return_get_func_decl(stmnt)->ret_type;

  (void) ret_val;
  (void) func_return_type;

  // type_equal(expr_get_return_type(ret_val, func_return_type))

  return True;
}

bool stmnt_chckr_check_block(STMNT_CHCKR_PARAMS) {
  AvailableVariables* av_vars = chckr_analysis_state_get_av_vars(an_state); 
  StructDeclarationList* structs = chckr_analysis_state_get_structs(an_state); 
  FunctionDeclarationList* functions = chckr_analysis_state_get_functions(an_state);
  (void) stmnt; (void) av_vars; (void) structs; (void) functions;

  StatementList* body = stmnt_block_get_body(stmnt);
  avlb_vars_enter_block(av_vars);
  FOR_EACH(StatementList, stmnt_it, body) {
    if(!stmnt_chckr_check(stmnt_it->node, an_state))
      return False;
  }
  avlb_vars_exit_block(av_vars);
  return True;
}

bool stmnt_chckr_check_if_else(STMNT_CHCKR_PARAMS) {
  AvailableVariables* av_vars = chckr_analysis_state_get_av_vars(an_state); 
  StructDeclarationList* structs = chckr_analysis_state_get_structs(an_state); 
  FunctionDeclarationList* functions = chckr_analysis_state_get_functions(an_state);
  (void) stmnt; (void) av_vars; (void) structs; (void) functions;

  // Expression *condition = stmnt_if_else_get_condition(stmnt);
  // Statement *if_body = stmnt_if_else_get_if_body(stmnt);
  // Statement *else_body = stmnt_if_else_get_else_body(stmnt);

  // check condition is of type int
  // check if_body
  // check else_body (if not NULL)

  return True;
}

bool stmnt_chckr_check_while(STMNT_CHCKR_PARAMS) {
  AvailableVariables* av_vars = chckr_analysis_state_get_av_vars(an_state); 
  StructDeclarationList* structs = chckr_analysis_state_get_structs(an_state); 
  FunctionDeclarationList* functions = chckr_analysis_state_get_functions(an_state);
  (void) stmnt; (void) av_vars; (void) structs; (void) functions;

  // Expression *condition = stmnt_while_get_condition(stmnt);
  // Statement *body = stmnt_while_get_body(stmnt);

  // check condition is of type int
  // check body

  return True;
}

#define STMNT_CHCKR_FUNC_SIGN bool (*)(STMNT_CHCKR_PARAMS)

bool (*stmnt_chckr_func_maps[])(STMNT_CHCKR_PARAMS) = {
  [ASSIGNMENT_STMNT] = stmnt_chckr_check_assignment,
  [DECLARATION_STMNT] = stmnt_chckr_check_declaration,
  [BLOCK_STMNT] = stmnt_chckr_check_block,
  [IF_ELSE_STMNT] = stmnt_chckr_check_if_else,
  [WHILE_STMNT] = stmnt_chckr_check_while,
  [RETURN_STMNT] = stmnt_chckr_check_return,
  [FUNCCALL_STMNT] = stmnt_chckr_check_funccall,
};

#define STMNT_CHCKR_SIZE_FUNCTION_MAP (sizeof(stmnt_chckr_func_maps)/sizeof(STMNT_CHCKR_FUNC_SIGN))

bool stmnt_chckr_check(STMNT_CHCKR_PARAMS) {
  assert(COUNT_STMNT == STMNT_CHCKR_SIZE_FUNCTION_MAP);
  assert(stmnt->type >= 0 && stmnt->type < COUNT_STMNT);
  // fprintf(stdout, "checking statement: ");
  // stmnt_print(stmnt, stdout); fprintf(stdout, "\n");
  return stmnt_chckr_func_maps[stmnt->type](stmnt, an_state);
}
