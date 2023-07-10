#ifndef OPRND_HEADER
#define OPRND_HEADER

#include "expr_interface.h"
#include "idf.h"
#include "obj_drf.h"
#include "funccall.h"

#define OPRND_ERROR() { fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1); }
#define if_null_print(ptr, file) if (ptr == NULL) { fprintf(file, "NULL"); return; }


// OPERAND

typedef enum {
  IDENTIFIER_OPERAND,
  INTEGER_OPERAND,
  STRING_OPERAND,
  FUNCCALL_OPERAND,
  ARRAY_DEREF_OPERAND,
  OBJ_DEREF_OPERAND,
} OperandType;

typedef struct {
  OperandType type;
  void *data;
} Operand;

typedef struct {
  Identifier *array_name;
  Expression *index;
} ArrayDereferenceOperandData;

typedef struct {
  ObjectDerefList *derefs;
} ObjectDerefOperandData;

ObjectDerefOperandData *oprnd_create_object_deref_operand_data(ObjectDerefList *derefs){
  ObjectDerefOperandData *data = (ObjectDerefOperandData *) malloc(sizeof(ObjectDerefOperandData));
  data->derefs = derefs;
  return data;
}

ArrayDereferenceOperandData *oprnd_create_array_deref_operand_data(Identifier *array_name, Expression *index) {
  ArrayDereferenceOperandData *data = (ArrayDereferenceOperandData *) malloc(sizeof(ArrayDereferenceOperandData));
  data->array_name = array_name;
  data->index = index;
  return data;
}

void oprnd_dealloc_array_deref_operand_data(ArrayDereferenceOperandData *data) {
  idf_dealloc_identifier(data->array_name);
  expr_dealloc_expression(data->index);
  free(data);
}

void oprnd_dealloc_object_deref_operand_data(ObjectDerefOperandData *data) {
  obj_drf_list_dealloc(data->derefs);
  free(data);
}

void oprnd_print_array_deref_operand_data(ArrayDereferenceOperandData *data, FILE *file) {
  idf_print_identifier(data->array_name, file);
  fprintf(file, "[");
  expr_print_expression(data->index, file);
  fprintf(file, "]");
}

void oprnd_print_object_deref_operand_data(ObjectDerefOperandData *data, FILE *file) {
  obj_drf_list_print(data->derefs, file);
}
 
Operand *oprnd_create_operand(OperandType type, void *data) {
  Operand *operand = (Operand *)malloc(sizeof(Operand));
  operand->type = type;
  operand->data = data;
  return operand;
}

void oprnd_dealloc_operand(Operand *operand) {
  if (operand == NULL) 
    return;
  if (operand->type == IDENTIFIER_OPERAND) 
    idf_dealloc_identifier((Identifier *) operand->data);
  else if (operand->type == FUNCCALL_OPERAND)
    funccall_dealloc((FunctionCall *) operand->data);
  else if (operand->type == ARRAY_DEREF_OPERAND)
    oprnd_dealloc_array_deref_operand_data((ArrayDereferenceOperandData *) operand->data);
  else if (operand->type == OBJ_DEREF_OPERAND)
    oprnd_dealloc_object_deref_operand_data((ObjectDerefOperandData *) operand->data);
  else
    free(operand->data);
  free(operand);
}

void oprnd_print_operand(Operand *operand, FILE *file) {
  if (operand == NULL) 
    return;
  switch (operand->type) {
  case INTEGER_OPERAND:
    fprintf(file, "%d", *((int *) operand->data)); break;
  case IDENTIFIER_OPERAND:
    idf_print_identifier((Identifier *) operand->data, file); break;
  case FUNCCALL_OPERAND:
    funccall_print((FunctionCall *) operand->data, file); break;
  case STRING_OPERAND:
    fprintf(file, "\"%s\"", (char * )operand->data); break;
  case ARRAY_DEREF_OPERAND:
    oprnd_print_array_deref_operand_data((ArrayDereferenceOperandData *) operand->data, file); break;
  case OBJ_DEREF_OPERAND:
    oprnd_print_object_deref_operand_data((ObjectDerefOperandData *) operand->data, file); break;
  default:
    OPRND_ERROR();
  }
}

#endif // end OPRND_HEADER