#pragma once

#include <stdint.h>
#include "../../expr/idf.h"
#include "../../expr/type.h"

typedef uint64_t VariableIndex;
typedef struct {
  VariableIndex var_idx;
  Identifier *name;
  Type *type;
} Variable;

Variable Variable_Create(VariableIndex idx, Identifier *name, Type *type);
void Variable_Destroy(Variable var);
void Variable_Print(Variable var, FILE *file);

// IMPLEMENTATIONS

Variable Variable_Create(VariableIndex idx, Identifier *name, Type *type){
  assert(name != NULL);
  assert(type != NULL);
  Variable var = {
    .var_idx = idx,
    .name = name,
    .type = type,
  };
  return var;
}

Variable *Variable_Create_Pointer(VariableIndex var_idx, Identifier *name, Type *type) {
  Variable *var = (Variable *) malloc(sizeof(Variable));
  var->name = name;
  var->type = type;
  var->var_idx = var_idx;
  return var;
}

void Variable_Destroy(Variable var) {
  idf_dealloc_identifier(var.name);
  type_dealloc(var.type);
}

void Variable_Print(Variable var, FILE *file) {
  fprintf(file, "{");
  idf_print_identifier(var.name, file);
  fprintf(file, ":%lu:", var.var_idx);
  type_print(var.type, file);
  fprintf(file, "}");
}

// GLOBAL VARIABLES TABLE

void __Variable_Destroy_List_Version(Variable* var) {
  Variable_Destroy(*var);
  free(var);
}

void __Variable_Destroy_Print_Version(Variable* var, FILE *file) {
  Variable_Print(*var, file); 
}

LIST(Variable, Variable, __Variable_Destroy_List_Version, __Variable_Destroy_Print_Version);

#define NULL_VARIABLE_IDX 0

typedef struct {
  VariableList *variables;
} GlobalVariablesTable;

GlobalVariablesTable *GlobalVariablesTable_Create();
void GlobalVariablesTable_Destroy(GlobalVariablesTable* table);

void GlobalVariablesTable_Add_Variable(GlobalVariablesTable* table, Variable *var);
Variable GlobalVariablesTable_Get_Variable(GlobalVariablesTable* table, VariableIndex idx);

Variable GlobalVariablesTable_GetFromName(GlobalVariablesTable *table, Identifier *name);

void GlobalVariablesTable_Print(GlobalVariablesTable *table, FILE *file);

// IMPLEMENTATION

GlobalVariablesTable *GlobalVariablesTable_Create() {
  GlobalVariablesTable *table = (GlobalVariablesTable *) malloc(sizeof(GlobalVariablesTable));
  
  Variable *null_var = 
    Variable_Create_Pointer(
      NULL_VARIABLE_IDX, 
      idf_create_identifier("NULL.NULL", file_info_create_null()),
      type_create_generic_int_type());
  table->variables = Variable_list_create(null_var);

  return table;
}
void GlobalVariablesTable_Destroy(GlobalVariablesTable* table) {
  assert(table != NULL);
  Variable_list_dealloc(table->variables);
  free(table);
}

void GlobalVariablesTable_Add_Variable(GlobalVariablesTable* table, Variable *var) {
  assert(table != NULL);
  assert(var != NULL);
  assert(var->var_idx > 0);
  Variable_list_append(table->variables, var);
}

Variable GlobalVariablesTable_Get_Variable(GlobalVariablesTable* table, VariableIndex idx) {
  assert(table != NULL);
  FOR_EACH(VariableList, var_it, table->variables) {
    if (var_it->node->var_idx == idx)
      return *(var_it->node);
  }
  return GlobalVariablesTable_Get_Variable(table, NULL_VARIABLE_IDX);
}

void GlobalVariablesTable_Print(GlobalVariablesTable *table, FILE *file) {
  assert(table != NULL);
  fprintf(file, "Global Variables Table:\n");
  FOR_EACH(VariableList, var_it, table->variables) {
    fprintf(file, "  ");
    __Variable_Destroy_Print_Version(var_it->node, file);
    fprintf(file, "\n");
  }
}

Variable GlobalVariablesTable_GetFromName(GlobalVariablesTable *table, Identifier *name) {
  assert(table != NULL);
  FOR_EACH(VariableList, var_it, table->variables) {
    if (idf_equal_identifiers(var_it->node->name, name))
      return *(var_it->node);
  }
  return GlobalVariablesTable_Get_Variable(table, NULL_VARIABLE_IDX);
}


bool Variable_IsNull(Variable var) {
  return var.var_idx == NULL_VARIABLE_IDX;
}