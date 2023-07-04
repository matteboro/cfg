
#include "idf.h"

#define TYPE_ERROR() { fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1); }
#define typed_data(type) type *data = (type *) malloc(sizeof(type))
#define casted_data(type, elem) type *data = (type *) elem->data

#define TYPE_DEALLOC( infix, data_type, dealloc_code) \
  void type_dealloc_##infix##_type(Type *type) \
    { if (!type->data) return; data_type *data = (data_type *)type->data; dealloc_code; free(data); }

typedef enum {
  INT_TYPE,
  STRING_TYPE,
  ARR_TYPE,
  STRUCT_TYPE,
} TypeType;

typedef struct {
  TypeType type;
  void *data;
} Type;

typedef struct {
  int size;
  Type *type;
} ArrayTypeData;

typedef struct {
  Identifier *name;
} StructTypeData;

typedef struct {

} StringTypeData;

typedef struct {

} IntTypeData;


// CREATE

Type *type_create(TypeType type_type, void *data) {
  Type *type = (Type *) malloc(sizeof(Type));
  type->type = type_type;
  type->data = data;
  return type;
}

Type *type_create_int_type() {
  typed_data(IntTypeData);
  return type_create(INT_TYPE, data);
}

Type *type_create_string_type() {
  typed_data(StringTypeData);
  return type_create(STRING_TYPE, data);
}

Type *type_create_struct_type(Identifier *name) {
  typed_data(StructTypeData);
  data->name = name;
  return type_create(STRUCT_TYPE, data);
}

Type *type_create_array_type(int size, Type *type) {
  typed_data(ArrayTypeData);
  data->size = size;
  data->type = type;
  return type_create(ARR_TYPE, data);
}

// PRINT

void type_print(Type *type, FILE *file);

void type_print_int_type(FILE *file) { 
  fprintf(file, "int");
}

void type_print_string_type(FILE *file) {
  fprintf(file, "string");
}

void type_print_struct_type(Type *type, FILE *file) {
  casted_data(StructTypeData, type);
  idf_print_identifier(data->name, file);
}

void type_print_array_type(Type *type, FILE *file) {
  casted_data(ArrayTypeData, type);
  type_print(data->type, file);
  fprintf(file, "[%d]", data->size);
}

void type_print(Type *type, FILE *file) {
  if (type == NULL) {
    fprintf(file, "NULL");
    return;
  }
  switch (type->type) {
    case INT_TYPE: type_print_int_type(file); break;
    case STRING_TYPE: type_print_string_type(file); break;
    case STRUCT_TYPE: type_print_struct_type(type, file); break;
    case ARR_TYPE: type_print_array_type(type, file); break;
    default: TYPE_ERROR();
  }
}

// DEALLOC

void type_dealloc(Type *type);
TYPE_DEALLOC(int, IntTypeData, {})
TYPE_DEALLOC(string, StringTypeData, {})
TYPE_DEALLOC(struct, StructTypeData, {idf_dealloc_identifier(data->name);})
TYPE_DEALLOC(array, ArrayTypeData, {type_dealloc(data->type);});
void type_dealloc(Type *type) {
  if (type == NULL)
    return;
  switch (type->type) {
    case INT_TYPE: type_dealloc_int_type(type); break;
    case STRING_TYPE: type_dealloc_string_type(type); break;
    case STRUCT_TYPE: type_dealloc_struct_type(type); break;
    case ARR_TYPE: type_dealloc_array_type(type); break;
    default: TYPE_ERROR();
  }
  free(type);
}