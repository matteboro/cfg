#pragma once

#include "var.h"

struct AvailableVariables_s;
typedef struct AvailableVariables_s AvailableVariables;

AvailableVariables *avlb_vars_create();
void avlb_vars_print(AvailableVariables *, FILE *);
void avlb_vars_dealloc(AvailableVariables *);

bool avlb_vars_name_available(AvailableVariables *, Identifier *);
void avlb_vars_add_var(AvailableVariables *, Var *);
void avlb_vars_enter_block(AvailableVariables *);
void avlb_vars_exit_block(AvailableVariables *);
Var *avlb_vars_get_var_from_identifier(AvailableVariables *, Identifier *);

struct AvailableVariables_s {
  VarListList *vars;
};

AvailableVariables *avlb_vars_create() {
  AvailableVariables *av_vars = (AvailableVariables *) malloc(sizeof(AvailableVariables));
  av_vars->vars = var_list_list_create_empty();
  return av_vars;
}

void avlb_vars_print(AvailableVariables *av_vars, FILE *file) {
  var_list_list_print(av_vars->vars, file);
}
void avlb_vars_dealloc(AvailableVariables *av_vars) {
  var_list_list_dealloc(av_vars->vars);
  free(av_vars);
}

bool avlb_vars_name_available(AvailableVariables *av_vars, Identifier *idf) {
  FOR_EACH(VarListList, var_ll_it, av_vars->vars) {
    FOR_EACH(VarList, var_list, var_ll_it->node) {
      if (idf_equal_identifiers(var_list->node->nt_bind->name, idf))
        return False;
    }
  }
  return True;
}

void avlb_vars_add_var(AvailableVariables *av_vars, Var *var) {
  size_t size = var_list_list_size(av_vars->vars);
  if (size > 0) {
    VarList *last_list = var_list_list_get_at(av_vars->vars, size-1);
    var_list_append(last_list, var);
    return;
  }
  fprintf(stdout, "ERROR in %s, tried to add a variable to an empty stack of available variables\n", __FUNCTION__);
}

void avlb_vars_enter_block(AvailableVariables *av_vars) {
  VarList *new_list = var_list_create_empty();
  var_list_list_append(av_vars->vars, new_list);
}

void avlb_vars_exit_block(AvailableVariables *av_vars) {
  VarList *new_list = var_list_list_pop_last(av_vars->vars);
  var_list_dealloc(new_list);
}

Var *avlb_vars_get_var_from_identifier(AvailableVariables *av_vars, Identifier *idf) {
  FOR_EACH(VarListList, var_ll_it, av_vars->vars) {
    FOR_EACH(VarList, var_list_it, var_ll_it->node) {
      if (idf_equal_identifiers(var_list_it->node->nt_bind->name, idf))
        return var_list_it->node;
    }
  }
  return NULL;
}