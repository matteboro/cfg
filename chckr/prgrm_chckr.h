#pragma once

#include "../prsr/prgrm.h"
#include "stmnt_chckr.h"
#include "strct_decl_chckr.h"
#include "func_decl_chckr.h"
#include "chckr_env.h"

extern size_t num_simplifications;

bool prgrm_chckr_check(ASTProgram *program, bool verbose) {

  ASTCheckingAnalysisState *an_state = chckr_anlysis_state_create(program->struct_declarations, program->func_declarations);

  if (!strct_decl_chckr_check(chckr_analysis_state_get_structs(an_state)))
    goto ret_false;

  if (verbose)
    fprintf(stdout, "structs declarationanalysis passed\n");

  if (!func_decl_chckr_check(an_state))
    goto ret_false;

  if (verbose)
    fprintf(stdout, "functions declaration analysis passed\n");

  if (!stmnt_chckr_check(program->global_statement, an_state)) {
    goto ret_false;
  } 

  if (verbose)
    fprintf(stdout, "global statements analysis passed\n");

  if (verbose)
    fprintf(stdout, "total expression simplifications: %lu: \n", num_simplifications);
  
  chckr_analysis_state_dealloc(an_state);
  return True;

ret_false:
  chckr_analysis_state_dealloc(an_state);
  return False;
}