
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
obj_type type_## prefix ## _get_ ## obj_name  (Type* type) {              \
  assert(type->type == type_type);                                        \
  casted_data(payload_type, type);                                        \
  return data->obj_name;                                                  \
} 

#define TYPE_VERBOSE_PRINT_SWITCH 0
#define TYPE_PRINT_SIZE_SWITCH 0
#define TYPE_PRINT_STRUCT_DECL_SWITCH 0

// FORWARD DECLARATION

struct StructDeclaration;
typedef struct StructDeclaration StructDeclaration;

// BYTE SIZE

typedef unsigned long ByteSize;
#define NullByteSize 0

// TYPE IMPLEMENTATION
// TODO : I should put the struct declaration inside struct type data

typedef enum {
  INT_TYPE,
  STRING_TYPE,
  ARR_TYPE,
  STRUCT_TYPE,
  PTR_TYPE,
} TypeType;

typedef struct {
  TypeType type;
  void *data;
  FileInfo file_info;
  ByteSize size;
} Type;

typedef struct {
  int size;
  Type *type;
} ArrayTypeData;

typedef struct {
  Type *type;
} PointerTypeData;

typedef struct {
  Identifier *name;
  StructDeclaration *struct_decl;
} StructTypeData;

typedef struct {

} StringTypeData;

typedef struct {

} IntTypeData;

ByteSize type_get_size(Type *type) {
  if (type)
    return type->size;
  return NullByteSize;
}

TYPE_GETTER(struct, Identifier, name, StructTypeData, STRUCT_TYPE)
TYPE_GETTER(struct, StructDeclaration, struct_decl, StructTypeData, STRUCT_TYPE)

TYPE_GETTER(array, Type, type, ArrayTypeData, ARR_TYPE)
TYPE_NON_PTR_GETTER(array, int, size, ArrayTypeData, ARR_TYPE)

// CREATE

Type *type_create(TypeType type_type, void *data, FileInfo file_info, ByteSize size) {
  Type *type = (Type *) malloc(sizeof(Type));
  type->type = type_type;
  type->data = data;
  type->file_info = file_info;
  type->size = size;
  return type;
}

Type *type_create_int_type(FileInfo file_info) {
  typed_data(IntTypeData);
  return type_create(INT_TYPE, data, file_info, 8);
}

Type *type_create_string_type(FileInfo file_info) {
  typed_data(StringTypeData);
  return type_create(STRING_TYPE, data, file_info, 8);
}

Type *type_create_generic_int_type() {
  return type_create_int_type(file_info_create_null());
}

Type *type_create_generic_string_type() {
  return type_create_string_type(file_info_create_null());
}

Type *type_create_struct_type(Identifier *name) {
  typed_data(StructTypeData);
  data->name = name;
  data->struct_decl = NULL;
  return type_create(STRUCT_TYPE, data, name->file_info, NullByteSize);
}

Type *type_create_array_type(int size, Type *type, FileInfo file_info) {
  typed_data(ArrayTypeData);
  data->size = size;
  data->type = type;
  return type_create(ARR_TYPE, data, file_info, type->size * size);
}

Type *type_create_pointer_type(Type *type, FileInfo file_info) {
  typed_data(PointerTypeData);
  data->type = type;
  return type_create(PTR_TYPE, data, file_info, 8);
}

// PRINT

#define TYPE_COLOR green

void type_print(Type *type, FILE *file);

void type_print_int_type(FILE *file) { 
  fprintf(file, "int");
}

void type_print_string_type(FILE *file) {
  fprintf(file, "string");
}

void type_print_pointer_type(Type *type, FILE *file) {
  casted_data(PointerTypeData, type);
  type_print(data->type, file);
  fprintf(file, " ptr");
}

void type_print_struct_type(Type *type, FILE *file) {
  casted_data(StructTypeData, type);
  idf_print_identifier(data->name, file);
#if TYPE_PRINT_STRUCT_DECL_SWITCH
  purple(file);
  fprintf(file, "(");
  data->struct_decl != NULL ? fprintf(file, "!") : fprintf(file, "?");
  fprintf(file, ")");
  reset(file);
#endif
}

void type_print_array_type(Type *type, FILE *file) {
  casted_data(ArrayTypeData, type);
  type_print(data->type, file);
  TYPE_COLOR(file);
  fprintf(file, "[%d]", data->size);
}

void type_print(Type *type, FILE *file) {
  if (type == NULL) {
    fprintf(file, "NULL");
    return;
  }
  TYPE_COLOR(file);
  switch (type->type) {
    case INT_TYPE:    type_print_int_type(file); break;
    case STRING_TYPE: type_print_string_type(file); break;
    case STRUCT_TYPE: type_print_struct_type(type, file); break;
    case ARR_TYPE:    type_print_array_type(type, file); break;
    case PTR_TYPE:    type_print_pointer_type(type, file); break;
    default: TYPE_ERROR();
  }
#if TYPE_PRINT_SIZE_SWITCH
  reset(file);
  fprintf(file, ":");
  red(file);
  type->size != NullByteSize ? fprintf(file, "%lu", type->size) : fprintf(file, "?");
  green(file);
#endif
  reset(file);
}

void type_print_verbose(Type *type, FILE* file) {              
#if TYPE_VERBOSE_PRINT_SWITCH                  
  fprintf(file, "[");                          
  if (type != NULL)           
    type_print(type, file);   
  else                                         
    fprintf(file, "?");                        
  fprintf(file, "]");                          
#else
  (void) type;
  (void) file;
#endif
}

// DEALLOC

void type_dealloc(Type *type);
TYPE_DEALLOC(int, IntTypeData, {})
TYPE_DEALLOC(string, StringTypeData, {})
TYPE_DEALLOC(struct, StructTypeData, {idf_dealloc_identifier(data->name);})
TYPE_DEALLOC(array, ArrayTypeData, {type_dealloc(data->type);})
TYPE_DEALLOC(pointer, PointerTypeData, {type_dealloc(data->type);})

void type_dealloc(Type *type) {
  if (type == NULL)
    return;
  switch (type->type) {
    case INT_TYPE:    type_dealloc_int_type(type); break;
    case STRING_TYPE: type_dealloc_string_type(type); break;
    case STRUCT_TYPE: type_dealloc_struct_type(type); break;
    case ARR_TYPE:    type_dealloc_array_type(type); break;
    case PTR_TYPE:    type_dealloc_pointer_type(type); break;
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
  data_copy->struct_decl = data->struct_decl;
  return data_copy;
}

void *type_copy_array_type(Type *type) {
  casted_data(ArrayTypeData, type);
  ArrayTypeData *data_copy = (ArrayTypeData *) malloc(sizeof(ArrayTypeData));
  data_copy->size = data->size;
  data_copy->type = type_copy(data->type);
  return data_copy;
}

void *type_copy_pointer_type(Type *type) {
  casted_data(PointerTypeData, type);
  PointerTypeData *data_copy = (PointerTypeData *) malloc(sizeof(PointerTypeData));
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
    case PTR_TYPE:      data = type_copy_pointer_type(type);   break;
    default: TYPE_ERROR();
  }
  type_copy->data = data;
  type_copy->file_info = type->file_info;
  type_copy->size = type->size;
  return type_copy;
}

// UTILITY

// NOTE: this function is needed when the type is an array or ptr, 
// it returns the type of the values in the array (por tr).
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

bool type_is_integer(Type *type) {
  if (type == NULL)
    return False;
  return type->type == INT_TYPE;
}

bool type_is_string(Type *type) {
  if (type == NULL)
    return False;
  return type->type == STRING_TYPE;
}

bool type_is_of_type(Type *type, TypeType tt) {
  if (type)
    return type->type == tt;
  return False;
}

bool type_size_is_known(Type *type) {
  if (type == NULL) 
    return False;
  return type->size != NullByteSize;
}

void type_set_ultimate_type_size(Type *type, ByteSize size) {
  if (type == NULL || size == NullByteSize)
    return;

  if (type->type == ARR_TYPE) {
    casted_data(ArrayTypeData, type);
    type_set_ultimate_type_size(data->type, size);
    int array_size = data->size;
    type->size = data->type->size * array_size;
  } 
  else if (type->type == STRUCT_TYPE) {
    // casted_data(StructTypeData, type);
    type->size = size;
  }
  return;
}

void type_struct_set_struct_decl(Type *type, StructDeclaration *struct_decl) {
  assert(type->type == STRUCT_TYPE);
  casted_data(StructTypeData, type);
  data->struct_decl = struct_decl;
  return;
}

bool type_is_struct(Type *type) {
  return type_is_of_type(type, STRUCT_TYPE);
}

bool type_is_array(Type *type) {
  return type_is_of_type(type, ARR_TYPE);
}