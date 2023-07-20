#pragma once 

#include "chckr_env.h"
#include "../prsr/func_decl.h"
#include "type_chckr.h"
#include "stmnt_chckr.h"

bool func_decl_chckr_check(ASTCheckingAnalysisState *);

bool func_decl_same_name(FunctionDeclaration *f1, FunctionDeclaration *f2) {
  return idf_equal_identifiers(f1->name, f2->name);
}

bool func_decl_chckr_check_overloading(FunctionDeclarationList *functions) {
  // we do not support function overloading for the moment
  if (func_decl_list_check_binary_predicate(functions, func_decl_same_name)) {
    // TODO: error

    return False;
  }
  return True;
}

bool func_decl_chckr_check(ASTCheckingAnalysisState *an_state) { 
  FunctionDeclarationList *functions = chckr_analysis_state_get_functions(an_state);
  StructDeclarationList *structs = chckr_analysis_state_get_structs(an_state);
  AvailableVariables *av_vars = chckr_analysis_state_get_av_vars(an_state);

  // check parameter types  
  FOR_EACH(FunctionDeclarationList, func_it, functions) {
    // check return type is valid
    Type *ret_type = func_it->node->ret_type;
    if(!type_chckr_type_exists(structs, ret_type)) {
      // TODO: error

      return False;
    }

    FOR_EACH(ParameterList, prmt_it, func_it->node->params) {
      // check parameter's type is valid
      Type *prmt_type = prmt_it->node->nt_bind->type;
      if(!type_chckr_type_exists(structs, prmt_type)) {
        // TODO: error

        return False;
      }
    }
  }

  // check for overloading
  if (!func_decl_chckr_check_overloading(functions)) {
    return False;
  }

  // check functions bodys
  FOR_EACH(FunctionDeclarationList, func_it, functions) {
    avlb_vars_enter_block(av_vars);
    FOR_EACH(ParameterList, prmt_it, func_it->node->params) {
      if (!avlb_vars_name_available(av_vars, prmt_it->node->nt_bind->name)) {
        // TODO: error

        return False;
      }
      avlb_vars_add_var(av_vars, var_create(prmt_it->node->nt_bind));
    }
    if (!stmnt_chckr_check(func_it->node->body, an_state)) {
      return False;
    }
    avlb_vars_exit_block(av_vars);
  }

  return True;
}