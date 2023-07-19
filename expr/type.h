
#pragma once

#include "idf.h"

#define TYPE_ERROR() { fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1); }
#define typed_data(type) type *data = (type *) malloc(sizeof(type))
#define casted_data(type, elem) type *data = (type *) elem->data

#define TYPE_DEALLOC(infix, data_type, dealloc_code) \
  void type_dealloc_##infix##_type(Type *type) \
    { if (!type->data) return; data_type *data = (data_type *)type->data; dealloc_code; free(data); }

#define TYPE_GETTER(prefix, obj_type, obj_name, payload_type, type_type)  \
obj_type *type_## prefix ## _get_ ## obj_name  (Type* type) {             \
  assert(type->type == type_type);                                        \
  casted_data(payload_type, type);                                        \
  return data->obj_name;                                                  \
} 

#define TYPE_NON_PTR_GETTER(prefix, obj_type, obj_name, payload_type, type_type)  \
obj_type type_## prefix ## _get_ ## obj_name  (Type* type) {             \
  assert(type->type == type_type);                                        \
  casted_data(payload_type, type);                                        \
  return data->obj_name;                                                  \
} 


typedef enum {
  INT_TYPE,
  STRING_TYPE,
  ARR_TYPE,
  STRUCT_TYPE,
} TypeType;

typedef struct {
  TypeType type;
  void *data;
  FileInfo file_info;
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

TYPE_GETTER(struct, Identifier, name, StructTypeData, STRUCT_TYPE)

TYPE_GETTER(array, Type, type, ArrayTypeData, ARR_TYPE)
TYPE_NON_PTR_GETTER(array, int, size, ArrayTypeData, ARR_TYPE)

// CREATE

Type *type_create(TypeType type_type, void *data, FileInfo file_info) {
  Type *type = (Type *) malloc(sizeof(Type));
  type->type = type_type;
  type->data = data;
  type->file_info = file_info;
  return type;
}

Type *type_create_generic_int_type() {
  typed_data(IntTypeData);
  return type_create(INT_TYPE, data, file_info_create_null());
}

Type *type_create_generic_string_type() {
  typed_data(StringTypeData);
  return type_create(STRING_TYPE, data, file_info_create_null());
}

Type *type_create_int_type(FileInfo file_info) {
  typed_data(IntTypeData);
  return type_create(INT_TYPE, data, file_info);
}

Type *type_create_string_type(FileInfo file_info) {
  typed_data(StringTypeData);
  return type_create(STRING_TYPE, data, file_info);
}

Type *type_create_struct_type(Identifier *name) {
  typed_data(StructTypeData);
  data->name = name;
  return type_create(STRUCT_TYPE, data, name->file_info);
}

Type *type_create_array_type(int size, Type *type, FileInfo file_info) {
  typed_data(ArrayTypeData);
  data->size = size;
  data->type = type;
  return type_create(ARR_TYPE, data, file_info);
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

// COPY

Type *type_copy(Type *type);

void *type_copy_int_type(Type *type) { 
  (void) type;
  return malloc(sizeof(IntTypeData));
}

void *type_copy_string_type(Type *type) {
  (void) type;
  return malloc(sizeof(StringTypeData));
}

void *type_copy_struct_type(Type *type) {
  casted_data(StructTypeData, type);
  StructTypeData *data_copy = (StructTypeData *) malloc(sizeof(StructTypeData));
  data_copy->name = idf_copy_identifier(data->name);
  return data_copy;
}

void *type_copy_array_type(Type *type) {
  casted_data(ArrayTypeData, type);
  ArrayTypeData *data_copy = (ArrayTypeData *) malloc(sizeof(ArrayTypeData));
  data_copy->size = data->size;
  data_copy->type = type_copy(data->type);
  return data_copy;
}

Type *type_copy(Type *type) {
  if (type == NULL)
    return NULL;
  Type *type_copy = (Type *) malloc(sizeof(Type));
  type_copy->type = type->type;
  void *data = NULL;
  switch (type->type) {
    case INT_TYPE:      data = type_copy_int_type(type);     break;
    case STRING_TYPE:   data = type_copy_string_type(type);  break;
    case STRUCT_TYPE:   data = type_copy_struct_type(type);  break;
    case ARR_TYPE:      data = type_copy_array_type(type);   break;
    default: TYPE_ERROR();
  }
  type_copy->data = data;
  type_copy->file_info = type->file_info;
  return type_copy;
}

// UTILITY

// NOTE: this function is needed when the type is an array, 
// it returns the type of the values in the array.
// it also works for multidemnsional array if in the future 
// will be added
Type *type_extract_ultimate_type(Type *type) {
  if (type->type == ARR_TYPE) {
    ArrayTypeData *data = (ArrayTypeData *)type->data;
    return type_extract_ultimate_type(data->type); 
  }
  return type;
}

// TODO: have list of basic types and iterate through them
bool type_is_basic(Type *type) {
  type = type_extract_ultimate_type(type);
  if (type->type == INT_TYPE || type->type == STRING_TYPE)
    return True;
  return False;
}

Type *type_create_basic_type(TypeType t_type) {
  switch (t_type) {
    case INT_TYPE:      return type_create_generic_int_type();
    case STRING_TYPE:   return type_create_generic_string_type();
    default:            return NULL;
  }
  return NULL;
}


bool type_equal(Type *type1, Type *type2) {
  if (type1 == NULL || type2 == NULL)
    return False;
  if (type1->type == type2->type) {
    if(type_is_basic(type1))
      return True;
    if(type1->type == STRUCT_TYPE) {
      return idf_equal_identifiers(type_struct_get_name(type1), type_struct_get_name(type2));
    } 
    else if (type1->type == ARR_TYPE) {
      if (type_array_get_size(type1) == type_array_get_size(type2))
        return type_equal(type_array_get_type(type1), type_array_get_type(type2));
    }
  }
  return False;
}