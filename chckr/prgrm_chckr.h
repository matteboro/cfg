#pragma once

#include "../prsr/prgrm.h"
#include "stmnt_chckr.h"
#include "strct_decl_chckr.h"

bool prgrm_chckr_check(ASTProgram *program) {

  //strct_decl_list_print(program->struct_declarations, stdout);
  if (!strct_decl_chckr_check(program->struct_declarations))
    return False;

  fprintf(stdout, "structs analysis passed\n");

  AvailableVariables *av_vars = avlb_vars_create();
  if (!stmnt_chckr_check(program->global_statement, av_vars, program->func_declarations, program->struct_declarations)) {
    avlb_vars_dealloc(av_vars);
    return False;
  }
  avlb_vars_dealloc(av_vars);  

  fprintf(stdout, "global statements analysis passed\n");

  return True;
}