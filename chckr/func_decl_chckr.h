#pragma once 

#include "chckr_env.h"
#include "../prsr/func_decl.h"
#include "type_chckr.h"
#include "stmnt_chckr.h"

#define FUNC_DECL_CHCKR_ERROR_HEADER() \
  fprintf(stdout, "ERROR: did not pass function declaration analysis.\n  ");


bool func_decl_chckr_check(ASTCheckingAnalysisState *);

bool func_decl_same_name(FunctionDeclaration *f1, FunctionDeclaration *f2) {
  return idf_equal_identifiers(f1->name, f2->name);
}

bool func_decl_chckr_check_overloading(FunctionDeclarationList *functions) {
  // we do not support function overloading for the moment
  FunctionDeclaration *f1 = NULL, *f2 = NULL;
  FOR_EACH(FunctionDeclarationList, func1_it, functions) {
    FOR_EACH(FunctionDeclarationList, func2_it, func1_it->next) {
      if (func_decl_same_name(func1_it->node, func2_it->node)) {
        f1 = func1_it->node;
        f2 = func2_it->node;
        goto continue_check;
      }
    }
  }

continue_check:
  if (f1 != NULL && f2 != NULL) {
    FUNC_DECL_CHCKR_ERROR_HEADER();
    fprintf(stdout, "overloading is not supported, you can not have functions with same names:\n    ");
    func_decl_print_signature(f1, stdout); fprintf(stdout, "\n    ");
    func_decl_print_signature(f2, stdout); fprintf(stdout, "\n\n");
    single_line_file_info_print_context(f1->name->file_info, stdout); fprintf(stdout, "\n\n");
    single_line_file_info_print_context(f2->name->file_info, stdout); fprintf(stdout, "\n\n");
    return False;
  }
  return True;
}

bool func_decl_chckr_check(ASTCheckingAnalysisState *an_state) { 
  FunctionDeclarationList *functions = chckr_analysis_state_get_functions(an_state);
  StructDeclarationList *structs = chckr_analysis_state_get_structs(an_state);
  AvailableVariables *av_vars = chckr_analysis_state_get_av_vars(an_state);

  // check function signature 
  FOR_EACH(FunctionDeclarationList, func_it, functions) {
    // check return type is valid
    Type *ret_type = func_it->node->ret_type;
    if(!type_chckr_type_exists(structs, ret_type)) {
      FUNC_DECL_CHCKR_ERROR_HEADER();
      fprintf(stdout, "function declaration:\n    ");
      func_decl_print_signature(func_it->node, stdout);
      fprintf(stdout, "\n  has non existent return type: ");
      type_print(ret_type, stdout);
      fprintf(stdout, "\n\n");
      single_line_file_info_print_context(ret_type->file_info, stdout); fprintf(stdout, "\n\n");
      return False;
    }

    // check parameters' types are valid
    FOR_EACH(ParameterList, prmt_it, func_it->node->params) {
      Type *prmt_type = prmt_it->node->nt_bind->type;
      if(!type_chckr_type_exists(structs, prmt_type)) {
        FUNC_DECL_CHCKR_ERROR_HEADER();
        fprintf(stdout, "in function declaration:\n    ");
        func_decl_print_signature(func_it->node, stdout);
        fprintf(stdout, "\n  parameter: ");
        prmt_print(prmt_it->node, stdout);
        fprintf(stdout, " has non existent type: ");
        type_print(prmt_type, stdout);
        fprintf(stdout, "\n\n");
        single_line_file_info_print_context(prmt_it->node->file_info, stdout); fprintf(stdout, "\n\n");
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