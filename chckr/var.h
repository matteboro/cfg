#pragma once

#include "../expr/nt_bind.h"

struct Var_s;
typedef struct Var_s Var;

Var *var_create(NameTypeBinding *);
void var_print(Var *, FILE *);
void var_dealloc(Var *);

struct Var_s {
  NameTypeBinding *nt_bind;
};

Var *var_create(NameTypeBinding *nt_bind) {
  Var *var = (Var *) malloc(sizeof(Var));
  var->nt_bind = nt_bind_copy(nt_bind);
  return var;
}

void var_print(Var *var, FILE *file){ 
  nt_bind_print(var->nt_bind, file);
}

void var_dealloc(Var *var){
  nt_bind_dealloc(var->nt_bind);
  free(var);
}

LIST(var, Var, var_dealloc, var_print)
LIST(var_list, VarList, var_list_dealloc, var_list_print)