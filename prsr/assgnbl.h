#pragma once

#include "../expr/expr.h"
#include "../expr/obj_drf.h"

// ASSIGNABLE ELEMENT

#define ASSGNBL_ERROR() { fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1); }
#define typed_data(type) type *data = (type *) malloc(sizeof(type))
#define casted_data(type, elem) type *data = (type *) elem->data

//// DEFINITIONS

typedef enum {
  VAR_ASSGNBL,
  ARR_DEREF_ASSGNBL,
  DEREF_LIST_ASSGNBL,
} AssignableElementType;

typedef struct {
  AssignableElementType type;
  void *data;
} AssignableElement;

typedef struct {
  Identifier *var_name;
} VariableAssignableData;

typedef struct {
  Identifier *arr_name;
  Expression *index;
} ArrayDerefAssignableData;

typedef struct {
  ObjectDerefList *derefs;
} ObjectDerefListAssignableData;

//// CREATE

AssignableElement *assgnbl_create(AssignableElementType type, void *data){
  AssignableElement *assgnbl = (AssignableElement *) malloc(sizeof(AssignableElement));
  assgnbl->type = type;
  assgnbl->data = data;
  return assgnbl;
}

AssignableElement *assgnbl_create_deref_list_assignable(ObjectDerefList *derefs) {
  typed_data(ObjectDerefListAssignableData);
  data->derefs = derefs;
  return assgnbl_create(DEREF_LIST_ASSGNBL, data);
}

AssignableElement *assgnbl_create_var_assignable(Identifier *var_name) {
  typed_data(VariableAssignableData);
  data->var_name = var_name;
  return assgnbl_create(VAR_ASSGNBL, data);
}

AssignableElement *assgnbl_create_arr_deref_assignable(Identifier *arr_name, Expression *index) {
  typed_data(ArrayDerefAssignableData);
  data->arr_name = arr_name;
  data->index = index;
  return assgnbl_create(ARR_DEREF_ASSGNBL, data);
}

//// PRINT

void assgnbl_print_var_assignable(AssignableElement *element, FILE *file) {
  casted_data(VariableAssignableData, element);
  idf_print_identifier(data->var_name, file);
}

void assgnbl_print_arr_deref_assignable(AssignableElement *element, FILE *file) {
  casted_data(ArrayDerefAssignableData, element);
  idf_print_identifier(data->arr_name, file);
  fprintf(file, "[");
  expr_print_expression(data->index, file);
  fprintf(file, "]");
}

void assgnbl_print_deref_list_assignable(AssignableElement *element, FILE *file) {
  casted_data(ObjectDerefListAssignableData, element);
  obj_drf_list_print(data->derefs, file);
}

void assgnbl_print(AssignableElement *element, FILE *file) {
  if (element->type == VAR_ASSGNBL) 
    assgnbl_print_var_assignable(element, file);
  else if (element->type == ARR_DEREF_ASSGNBL)
    assgnbl_print_arr_deref_assignable(element, file);
  else if (element->type == DEREF_LIST_ASSGNBL)
    assgnbl_print_deref_list_assignable(element, file);
  else {
    ASSGNBL_ERROR();
  }
}

//// DEALLOC

void assgnbl_dealloc_var_assignable(AssignableElement *element) {
  casted_data(VariableAssignableData, element);
  idf_dealloc_identifier(data->var_name);
  free(data);
}

void assgnbl_dealloc_deref_list_assignable(AssignableElement *element) {
  casted_data(ObjectDerefListAssignableData, element);
  obj_drf_list_dealloc(data->derefs);
  free(data);
}


void assgnbl_dealloc_arr_deref_assignable(AssignableElement *element) {
  casted_data(ArrayDerefAssignableData, element);
  idf_dealloc_identifier(data->arr_name);
  expr_dealloc_expression(data->index);
  free(data);
}

void assgnbl_dealloc(AssignableElement *element) {
  if (element->type == VAR_ASSGNBL) 
    assgnbl_dealloc_var_assignable(element);
  else if (element->type == ARR_DEREF_ASSGNBL)
    assgnbl_dealloc_arr_deref_assignable(element);
  else if (element->type == DEREF_LIST_ASSGNBL)
    assgnbl_dealloc_deref_list_assignable(element);
  else {
    ASSGNBL_ERROR();
  }
  free(element);
}
