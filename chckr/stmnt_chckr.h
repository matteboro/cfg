#pragma once

#include "../prsr/stmnt.h"
#include "../prsr/func_decl.h"
#include "../prsr/strct_decl.h"
#include "funccall_chckr.h"
#include "avlb_vars.h"
#include "type_chckr.h"
#include "obj_drf_chckr.h"

#define STMNT_CHCKR_PARAMS                    \
  Statement *stmnt,                           \
  AvailableVariables *av_vars,                \
  FunctionDeclarationList *functions,         \
  StructDeclarationList *structs

bool stmnt_chckr_check(STMNT_CHCKR_PARAMS);

bool stmnt_chckr_check_assignment(STMNT_CHCKR_PARAMS);
bool stmnt_chckr_check_declaration(STMNT_CHCKR_PARAMS);
bool stmnt_chckr_check_funccall(STMNT_CHCKR_PARAMS);
bool stmnt_chckr_check_return(STMNT_CHCKR_PARAMS);
bool stmnt_chckr_check_block(STMNT_CHCKR_PARAMS);
bool stmnt_chckr_check_if_else(STMNT_CHCKR_PARAMS);
bool stmnt_chckr_check_while(STMNT_CHCKR_PARAMS);


bool stmnt_chckr_check_assignment(STMNT_CHCKR_PARAMS) {
  (void) stmnt; (void) av_vars; (void) structs; (void) functions;
  AssignableElement *assgnbl = stmnt_assignment_get_assgnbl(stmnt);
  // Expression *value = stmnt_assignment_get_value(stmnt);
  if(!obj_drf_chckr_check(assgnbl->obj_derefs, av_vars, structs))
    return False;

  return True;
}

bool stmnt_chckr_check_declaration(STMNT_CHCKR_PARAMS) {
  (void) stmnt; (void) av_vars; (void) structs; (void) functions;
  NameTypeBinding *nt_bind = stmnt_declaration_get_nt_bind(stmnt);
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
  // (void) stmnt; (void) av_vars; (void) structs; (void) functions;
  FunctionCall *funccall = stmnt_funccall_get_funccall(stmnt);
  return funccall_chckr_check(funccall, av_vars, functions, structs); 
}

bool stmnt_chckr_check_return(STMNT_CHCKR_PARAMS) {
  (void) stmnt; (void) av_vars; (void) structs; (void) functions;
  // Expression *ret_val = stmnt_return_get_ret_value(stmnt);
  return True;
}

bool stmnt_chckr_check_block(STMNT_CHCKR_PARAMS) {
  (void) stmnt; (void) av_vars; (void) structs; (void) functions;
  StatementList* body = stmnt_block_get_body(stmnt);
  avlb_vars_enter_block(av_vars);
  FOR_EACH(StatementList, stmnt_it, body) {
    if(!stmnt_chckr_check(stmnt_it->node, av_vars, functions, structs))
      return False;
  }
  avlb_vars_exit_block(av_vars);
  return True;
}

bool stmnt_chckr_check_if_else(STMNT_CHCKR_PARAMS) {
  (void) stmnt; (void) av_vars; (void) structs; (void) functions;
  // Expression *condition = stmnt_while_get_condition(stmnt);
  // Statement *body = stmnt_while_get_body(stmnt);
  return True;
}

bool stmnt_chckr_check_while(STMNT_CHCKR_PARAMS) {
  (void) stmnt; (void) av_vars; (void) structs; (void) functions;
  // Expression *condition = stmnt_if_else_get_condition(stmnt);
  // Statement *if_body = stmnt_if_else_get_if_body(stmnt);
  // Statement *else_body = stmnt_if_else_get_else_body(stmnt);
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
  return stmnt_chckr_func_maps[stmnt->type](stmnt, av_vars, functions, structs);
}
