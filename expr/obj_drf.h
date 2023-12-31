#pragma once

#include "idf.h"
#include "expr_interface.h"

#define OBJ_DRF_ERROR() { fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1); }
#define if_null_print(ptr, file) if (ptr == NULL) { fprintf(file, "NULL"); return; }
#define OBJ_DRF_DEALLOC(infix, data_type, dealloc_code) \
  void obj_drf_dealloc_##infix##_type_deref(ObjectDeref *obj_drf) \
    { if (!obj_drf->data) return; data_type *data = (data_type *)obj_drf->data; dealloc_code; free(data); }

#define OBJ_DRF_VERBOSE_PRINT_SWITCH 0

typedef struct Attribute Attribute;

typedef enum {
  ARR_DEREF,
  STRUCT_BASIC_DEREF,
  SINGLE_ELEMENT_PTR_DRF,
} ObjectDerefType;

typedef struct {
  ObjectDerefType type;
  Identifier *name;
  Type *real_type;
  void *data;
  FileInfo file_info;
  Attribute *attribute; // can be null
} ObjectDeref;

typedef struct { 
  
} StructOrBasicTypeObjectDerefData; 

typedef struct { 
 
} SingleElementPointerTypeObjectDerefData; 

typedef struct { 
  Expression *index;
} ArrayTypeObjectDerefData; 

void obj_drf_set_real_type(ObjectDeref *obj_drf, Type *type) {
  obj_drf->real_type = type;
}

Type* obj_drf_get_real_type(ObjectDeref *obj_drf) {
  return obj_drf->real_type;
}

void obj_drf_set_attribute(ObjectDeref *obj_drf, Attribute *attrb) {
  assert(obj_drf != NULL);
  assert(attrb != NULL);
  obj_drf->attribute = attrb;
}

Attribute *obj_drf_get_attribute(ObjectDeref *obj_drf) {
  assert(obj_drf != NULL);
  return obj_drf->attribute;
}

Expression *obj_drf_array_get_index(ObjectDeref *obj_drf) {
  assert(obj_drf->type == ARR_DEREF);
  casted_data(ArrayTypeObjectDerefData, obj_drf);
  return data->index;
}

void obj_drf_array_set_index(ObjectDeref *obj_drf, Expression *index) {
  assert(obj_drf->type == ARR_DEREF);
  casted_data(ArrayTypeObjectDerefData, obj_drf);
  data->index = index;
}

// CREATE

ObjectDeref *obj_drf_create(ObjectDerefType type, Identifier* name, void *data, FileInfo file_info) {
  ObjectDeref *obj_drf = (ObjectDeref *) malloc(sizeof(ObjectDeref));
  obj_drf->name = name;
  obj_drf->type = type;
  obj_drf->data = data;
  obj_drf->file_info = file_info;
  obj_drf->real_type = NULL;
  obj_drf->attribute = NULL;
  return obj_drf;
}

ObjectDeref *obj_drf_create_array_type_deref(Identifier *name, Expression *index, FileInfo file_info) {
  typed_data(ArrayTypeObjectDerefData);
  data->index = index;
  return obj_drf_create(ARR_DEREF, name, data, file_info);
}

ObjectDeref *obj_drf_create_struct_or_basic_type_deref(Identifier *name) {
  typed_data(StructOrBasicTypeObjectDerefData);
  return obj_drf_create(STRUCT_BASIC_DEREF, name, data, name->file_info);
}

ObjectDeref *obj_drf_create_single_element_pointer_deref(Identifier *name, FileInfo file_info) {
  typed_data(SingleElementPointerTypeObjectDerefData);
  return obj_drf_create(SINGLE_ELEMENT_PTR_DRF, name, data, file_info);
}

// PRINT

void obj_drf_print_struct_or_basic_type_deref(ObjectDeref *obj_drf, FILE *file) {
  if_null_print(obj_drf, file);
  idf_print_identifier(obj_drf->name, file);
}

void obj_drf_print_single_element_pointer_deref(ObjectDeref *obj_drf, FILE *file) {
  if_null_print(obj_drf, file);
  fprintf(file, "[");
  idf_print_identifier(obj_drf->name, file);
  fprintf(file, "]");
}

void obj_drf_print_array_type_deref(ObjectDeref *obj_drf, FILE *file) {
  if_null_print(obj_drf, file);
  casted_data(ArrayTypeObjectDerefData, obj_drf);
  if_null_print(data, file);
  idf_print_identifier(obj_drf->name, file);
  fprintf(file, "[");
  expr_print_expression(data->index, file);
  fprintf(file, "]");
}

void obj_drf_print(ObjectDeref *obj_drf, FILE *file) {
  if_null_print(obj_drf, file);

  type_print_verbose(obj_drf->real_type, file);
#if OBJ_DRF_VERBOSE_PRINT_SWITCH
  fprintf(file, "%s", obj_drf->attribute == NULL ? "" : "|!|:");
#endif
  if (obj_drf->type == ARR_DEREF)
    obj_drf_print_array_type_deref(obj_drf, file);
  else if (obj_drf->type == STRUCT_BASIC_DEREF)
    obj_drf_print_struct_or_basic_type_deref(obj_drf, file);
  else if (obj_drf->type == SINGLE_ELEMENT_PTR_DRF)
    obj_drf_print_single_element_pointer_deref(obj_drf, file);
  else
    OBJ_DRF_ERROR();
}

// DEALLOC

OBJ_DRF_DEALLOC(
  array,
  ArrayTypeObjectDerefData, 
  { expr_dealloc_expression(data->index); }
)

OBJ_DRF_DEALLOC(
  struct_or_basic,
  StructOrBasicTypeObjectDerefData,
  {}
)

OBJ_DRF_DEALLOC(
  single_element_pointer,
  SingleElementPointerTypeObjectDerefData,
  {}
)

void obj_drf_dealloc(ObjectDeref *obj_drf) {
  if (obj_drf == NULL)
    return;
  if (obj_drf->type == ARR_DEREF)
    obj_drf_dealloc_array_type_deref(obj_drf);
  else if (obj_drf->type == STRUCT_BASIC_DEREF)
    obj_drf_dealloc_struct_or_basic_type_deref(obj_drf);
  else if (obj_drf->type == SINGLE_ELEMENT_PTR_DRF)
    obj_drf_dealloc_single_element_pointer_type_deref(obj_drf);
  else
    OBJ_DRF_ERROR();

  idf_dealloc_identifier(obj_drf->name);
  if (obj_drf->real_type)
    type_dealloc(obj_drf->real_type);

  free(obj_drf);
}

#define OBJ_DRF_LIST(prefix, type_name, dealloc_func)             \
DEFAULT_LIST_IMPLEMENTATION(type_name)                            \
DEFAULT_LIST_CREATE_EMPTY(prefix, type_name)                      \
DEFAULT_LIST_CREATE(prefix, type_name)                            \
DEFAULT_LIST_APPEND(prefix, type_name)                            \
DEFAULT_LIST_DEALLOC(prefix, type_name, dealloc_func)             \
DEFAULT_LIST_SIZE(prefix, type_name)                              \
DEFAULT_LIST_GET_AT(prefix, type_name)                            \
DEFAULT_LIST_POP_LAST(prefix, type_name)                          \
DEFAULT_LIST_IS_EMPTY(prefix, type_name)                          \
DEFAULT_LIST_GET_FIRST(prefix, type_name)                         \
DEFAULT_LIST_GET_LAST(prefix, type_name)                          \
DEFAULT_LIST_MERGED_FILE_INFO(prefix, type_name)


OBJ_DRF_LIST(obj_drf, ObjectDeref, obj_drf_dealloc)

void obj_drf_list_print(ObjectDerefList *list, FILE *file) { 
  if_null_print(list, file);
  if (list->node != NULL) 
    obj_drf_print(list->node, file); 
  if (list->next != NULL) { 
    fprintf(file, "."); 
    obj_drf_list_print(list->next, file); 
  } 
}

// UTILITY

bool obj_drf_is_of_type(ObjectDeref* obj_drf, ObjectDerefType ob_t) {
  assert(obj_drf != NULL);
  return obj_drf->type == ob_t;
}

bool obj_drf_is_array_deref(ObjectDeref* obj_drf) {
  return obj_drf_is_of_type(obj_drf, ARR_DEREF);
}

bool obj_drf_is_name_deref(ObjectDeref* obj_drf) {
  return obj_drf_is_of_type(obj_drf, STRUCT_BASIC_DEREF);
}

bool obj_drf_is_single_element_deref(ObjectDeref* obj_drf) {
  return obj_drf_is_of_type(obj_drf, SINGLE_ELEMENT_PTR_DRF);
}


bool obj_drf_is_pointer_deref(ObjectDeref *obj_drf) {
  assert(obj_drf != NULL);

  Type *type = obj_drf_get_real_type(obj_drf);
  assert(type != NULL);

  if (type_is_pointer(type)) {
    if (obj_drf_is_array_deref(obj_drf) || obj_drf_is_single_element_deref(obj_drf)) {
      return True;
    }
    assert(obj_drf_is_name_deref(obj_drf));
  }
  return False;
}

bool obj_drf_have_attribute(ObjectDeref *obj_drf) {
  assert(obj_drf != NULL);
  return obj_drf->attribute != NULL;
}