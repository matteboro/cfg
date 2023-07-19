#pragma once

#include "expr_interface.h"
#include "idf.h"
#include "obj_drf.h"
#include "funccall.h"

#define OPRND_ERROR() { fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1); }
#define if_null_print(ptr, file) if (ptr == NULL) { fprintf(file, "NULL"); return; }

#define OPRND_GETTER(oprnd_prefix, obj_type, obj_name, data_type, oprnd_type)   \
obj_type *oprnd_## oprnd_prefix ## _get_ ## obj_name  (Operand* oprnd) {        \
  assert(oprnd->type == oprnd_type);                                            \
  casted_data(data_type, oprnd);                                                \
  return data->obj_name;                                                        \
}

typedef enum {
  INTEGER_OPERAND,
  STRING_OPERAND,
  FUNCCALL_OPERAND,
  OBJ_DEREF_OPERAND,
  COUNT_OPRND,
} OperandType;

typedef struct {
  OperandType type;
  void *data;
  FileInfo file_info;
} Operand;

Operand *oprnd_create(OperandType type, void *data, FileInfo file_info) {
  Operand *operand = (Operand *) malloc(sizeof(Operand));
  operand->type = type;
  operand->data = data;
  operand->file_info= file_info;
  return operand;
}

// OBJECT DEREF OPERAND

typedef struct {
  ObjectDerefList *derefs;
} ObjectDerefOperandData;

OPRND_GETTER(object_deref, ObjectDerefList, derefs, ObjectDerefOperandData, OBJ_DEREF_OPERAND)

Operand *oprnd_create_object_deref(ObjectDerefList *derefs);
void oprnd_dealloc_object_deref(Operand *obj_drf);
void oprnd_print_object_deref(Operand *obj_drf, FILE *file);

Operand *oprnd_create_object_deref(ObjectDerefList *derefs) {
  typed_data(ObjectDerefOperandData);
  data->derefs = derefs;
  return oprnd_create(OBJ_DEREF_OPERAND, data, obj_drf_list_merged_file_info(derefs));
}

void oprnd_dealloc_object_deref(Operand *operand) {
  casted_data(ObjectDerefOperandData, operand);
  obj_drf_list_dealloc(data->derefs);
  free(data);
}

void oprnd_print_object_deref(Operand *operand, FILE *file) {
  casted_data(ObjectDerefOperandData, operand);
  obj_drf_list_print(data->derefs, file);
}

// FUNCTION CALL OPERAND

typedef struct {
  FunctionCall *funccall;
} FunctionCallOperandData;

OPRND_GETTER(funccall, FunctionCall, funccall, FunctionCallOperandData, FUNCCALL_OPERAND)

Operand *oprnd_create_funccall(FunctionCall *funccall);
void oprnd_dealloc_funccall(Operand *obj_drf);
void oprnd_print_funccall(Operand *obj_drf, FILE *file);

Operand *oprnd_create_funccall(FunctionCall *funccall) {
  typed_data(FunctionCallOperandData);
  data->funccall = funccall;
  return oprnd_create(FUNCCALL_OPERAND, data, funccall->file_info);
}

void oprnd_dealloc_funccall(Operand *operand) {
  casted_data(FunctionCallOperandData, operand);
  funccall_dealloc(data->funccall);
  free(data);
}

void oprnd_print_funccall(Operand *operand, FILE *file) {
  casted_data(FunctionCallOperandData, operand);
  funccall_print(data->funccall, file);
}

// INTEGER OPERAND

typedef struct {
  int *integer;
} IntegerOperandData;

OPRND_GETTER(integer, int, integer, IntegerOperandData, INTEGER_OPERAND)

Operand *oprnd_create_integer(int *integer, FileInfo file_info);
void oprnd_dealloc_integer(Operand *obj_drf);
void oprnd_print_integer(Operand *obj_drf, FILE *file);

Operand *oprnd_create_integer(int *integer, FileInfo file_info) {
  typed_data(IntegerOperandData);
  data->integer = integer;
  return oprnd_create(INTEGER_OPERAND, data, file_info);
}

void oprnd_dealloc_integer(Operand *operand) {
  casted_data(IntegerOperandData, operand);
  free(data->integer);
  free(data);
}

void oprnd_print_integer(Operand *operand, FILE *file) {
  casted_data(IntegerOperandData, operand);
  fprintf(file, "%d", *data->integer);
}

// STRING OPERAND
 
typedef struct {
  char *string;
} StringOperandData;

OPRND_GETTER(string, char, string, StringOperandData, STRING_OPERAND)

Operand *oprnd_create_string(char *string, FileInfo file_info);
void oprnd_dealloc_string(Operand *obj_drf);
void oprnd_print_string(Operand *obj_drf, FILE *file);

Operand *oprnd_create_string(char *string, FileInfo file_info) {
  typed_data(StringOperandData);
  data->string = string;
  return oprnd_create(STRING_OPERAND, data, file_info);
}

void oprnd_dealloc_string(Operand *operand) {
  casted_data(StringOperandData, operand);
  free(data->string);
  free(data);
}

void oprnd_print_string(Operand *operand, FILE *file) {
  casted_data(StringOperandData, operand);
  fprintf(file, "\"%s\"", data->string);
}

//  GENERAL

#define OPRND_PRINT_SIGN   void (*)(Operand *, FILE *)
#define OPRND_DEALLOC_SIGN void (*)(Operand *)

enum {
  OPRND_PRINT_FUNC   = 0,
  OPRND_DEALLOC_FUNC = 1,
  OPRND_COUNT_FUNC,
};

void *oprnd_funcs_map[][OPRND_COUNT_FUNC] = {
  [INTEGER_OPERAND] = { oprnd_print_integer, oprnd_dealloc_integer},
  [STRING_OPERAND] = { oprnd_print_string, oprnd_dealloc_string},
  [FUNCCALL_OPERAND] = { oprnd_print_funccall, oprnd_dealloc_funccall},
  [OBJ_DEREF_OPERAND] = { oprnd_print_object_deref, oprnd_dealloc_object_deref},
};

#define OPRND_SIZE_OF_FUNCS_MAP (sizeof(oprnd_funcs_map)/sizeof(void *))/OPRND_COUNT_FUNC

void oprnd_dealloc(Operand *oprnd) {
  assert(COUNT_OPRND == OPRND_SIZE_OF_FUNCS_MAP);
  if (oprnd == NULL)
    return;
  ((OPRND_DEALLOC_SIGN)oprnd_funcs_map[oprnd->type][OPRND_DEALLOC_FUNC])(oprnd);
  free(oprnd);
}

void oprnd_print(Operand *oprnd, FILE *file) {
  assert(COUNT_OPRND == OPRND_SIZE_OF_FUNCS_MAP);
  ((OPRND_PRINT_SIGN)oprnd_funcs_map[oprnd->type][OPRND_PRINT_FUNC])(oprnd, file);
}