#pragma once

#include "../expr/oprnd.h"
#include "chckr_env.h"

// FORWARD DEFINITIONS
bool funccall_chckr_check(FunctionCall *, ASTCheckingAnalysisState *);
Type *obj_drf_chckr_check(ObjectDerefList *, ASTCheckingAnalysisState *);

Type *oprnd_chckr_get_type(Operand *operand, ASTCheckingAnalysisState *an_state);
FunctionDeclaration *oprnd_chckr_get_func_decl_from_identifier(FunctionDeclarationList *functions, Identifier *name);

FunctionDeclaration *oprnd_chckr_get_func_decl_from_identifier(FunctionDeclarationList *functions, Identifier *name) {
  FOR_EACH(FunctionDeclarationList, func_it, functions) {
    if (idf_equal_identifiers(func_it->node->name, name))
      return func_it->node;
  }
  return NULL;
}

Type *oprnd_chckr_get_type(Operand *operand, ASTCheckingAnalysisState *an_state) {
  if (operand->type == INTEGER_OPERAND) {
    return type_create_int_type();
  } else if (operand->type == STRING_OPERAND) {
    return type_create_string_type();
  } else if (operand->type == FUNCCALL_OPERAND) {
    FunctionCall *funccall = oprnd_funccall_get_funccall(operand);
    if (!funccall_chckr_check(funccall, an_state))
      return NULL;
    FunctionDeclaration *function = oprnd_chckr_get_func_decl_from_identifier(an_state->functions, funccall->function_name);
    assert(function != NULL);
    return type_copy(function->ret_type);
  } else if (operand->type == OBJ_DEREF_OPERAND) {
    ObjectDerefList *derefs = oprnd_object_deref_get_derefs(operand);
    Type *deref_type = obj_drf_chckr_check(derefs, an_state);
    if (deref_type != NULL)
      return type_copy(deref_type);
  } else {
    fprintf(stdout, "UNREACHABLE in %s\n", __FUNCTION__);
    exit(1);
  }
  return NULL;
}