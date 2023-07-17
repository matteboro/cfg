#pragma once

#include "../prsr/prgrm.h"
#include "stmnt_chckr.h"
#include "strct_decl_chckr.h"
#include "chckr_env.h"

bool prgrm_chckr_check(ASTProgram *program) {

  ASTCheckingAnalysisState *an_state = chckr_anlysis_state_create(program->struct_declarations, program->func_declarations);

  //strct_decl_list_print(program->struct_declarations, stdout);
  if (!strct_decl_chckr_check(chckr_analysis_state_get_structs(an_state)))
    goto ret_false;

  fprintf(stdout, "structs analysis passed\n");

  AvailableVariables *av_vars = avlb_vars_create();
  if (!stmnt_chckr_check(program->global_statement, an_state)) {
    avlb_vars_dealloc(av_vars);
    goto ret_false;
  }
  avlb_vars_dealloc(av_vars);  

  fprintf(stdout, "global statements analysis passed\n");

  chckr_analysis_state_dealloc(an_state);
  return True;

ret_false:
  chckr_analysis_state_dealloc(an_state);
  return False;
}