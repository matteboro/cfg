#pragma once

#include "avlb_vars.h"
#include "../prsr/strct_decl.h"
#include "../prsr/func_decl.h"

struct ASTCheckingAnalysisState_s;
typedef struct ASTCheckingAnalysisState_s ASTCheckingAnalysisState;

ASTCheckingAnalysisState *chckr_anlysis_state_create(StructDeclarationList *, FunctionDeclarationList *);
void chckr_analysis_state_dealloc(ASTCheckingAnalysisState *);

AvailableVariables *chckr_analysis_state_get_av_vars(ASTCheckingAnalysisState *);
StructDeclarationList *chckr_analysis_state_get_structs(ASTCheckingAnalysisState *);
FunctionDeclarationList *chckr_analysis_state_get_functions(ASTCheckingAnalysisState *);

struct ASTCheckingAnalysisState_s {
  AvailableVariables *av_vars;
  StructDeclarationList *structs;
  FunctionDeclarationList *functions;
};

ASTCheckingAnalysisState *chckr_anlysis_state_create(StructDeclarationList *structs, FunctionDeclarationList *functions) {
  ASTCheckingAnalysisState *an_state = (ASTCheckingAnalysisState *) malloc(sizeof(ASTCheckingAnalysisState));
  an_state->functions = functions;
  an_state->structs = structs;
  an_state->av_vars = avlb_vars_create();
  return an_state;
}

void chckr_analysis_state_dealloc(ASTCheckingAnalysisState *an_state) {
  avlb_vars_dealloc(an_state->av_vars);
  free(an_state);
}

AvailableVariables *chckr_analysis_state_get_av_vars(ASTCheckingAnalysisState *an_state){
  if (an_state == NULL) return NULL;
  return an_state->av_vars;
}
StructDeclarationList *chckr_analysis_state_get_structs(ASTCheckingAnalysisState *an_state){
  if (an_state == NULL) return NULL;
  return an_state->structs;
}
FunctionDeclarationList *chckr_analysis_state_get_functions(ASTCheckingAnalysisState *an_state){
  if (an_state == NULL) return NULL;
  return an_state->functions;
}
