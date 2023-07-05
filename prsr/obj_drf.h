#ifndef OBJ_DRF_HEADER
#define OBJ_DRF_HEADER

#include "../expr/idf.h"

#define typed_data(type) type *data = (type *) malloc(sizeof(type))
#define casted_data(type, elem) type *data = (type *) elem->data
#define OBJ_DRF_ERROR() fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1);
#define if_null_print(ptr, file) if (ptr == NULL) { fprintf(file, "NULL"); return; }
#define OBJ_DRF_DEALLOC(infix, data_type, dealloc_code) \
  void obj_drf_dealloc_##infix##_type_deref(ObjectDeref *obj_drf) \
    { if (!obj_drf->data) return; data_type *data = (data_type *)obj_drf->data; dealloc_code; free(data); }


typedef enum {
  ARR_DEREF,
  STRUCT_BASIC_DEREF,
} ObjectDerefType;

typedef struct {
  ObjectDerefType type;
  void *data;
} ObjectDeref;

typedef struct { 
  Identifier *name; 
} StructOrBasicTypeObjectDerefData; 

typedef struct { 
  Identifier *name; 
  int index;
} ArrayTypeObjectDerefData; 

// CREATE

ObjectDeref *obj_drf_create(ObjectDerefType type, void *data) {
  ObjectDeref *obj_drf = (ObjectDeref *) malloc(sizeof(ObjectDeref));
  obj_drf->type = type;
  obj_drf->data = data;
  return obj_drf;
}

ObjectDeref *obj_drf_create_array_type_deref(Identifier *name, int index) {
  typed_data(ArrayTypeObjectDerefData);
  data->name = name;
  data->index = index;
  return obj_drf_create(ARR_DEREF, data);
}

ObjectDeref *obj_drf_create_struct_or_basic_type_deref(Identifier *name) {
  typed_data(StructOrBasicTypeObjectDerefData);
  data->name = name;
  return obj_drf_create(STRUCT_BASIC_DEREF, data);
}

// PRINT

void obj_drf_print_struct_or_basic_type_deref(ObjectDeref *obj_drf, FILE *file) {
  casted_data(StructOrBasicTypeObjectDerefData, obj_drf);
  if_null_print(data, file);
  idf_print_identifier(data->name, file);
}

void obj_drf_print_array_type_deref(ObjectDeref *obj_drf, FILE *file) {
  casted_data(ArrayTypeObjectDerefData, obj_drf);
  if_null_print(data, file);
  idf_print_identifier(data->name, file);
  fprintf(file, "[%d]", data->index);
}

void obj_drf_print(ObjectDeref *obj_drf, FILE *file) {
  if_null_print(obj_drf, file);
  if (obj_drf->type == ARR_DEREF)
    obj_drf_print_array_type_deref(obj_drf, file);
  else if (obj_drf->type == STRUCT_BASIC_DEREF)
    obj_drf_print_struct_or_basic_type_deref(obj_drf, file);
  else
    OBJ_DRF_ERROR();
}

// DEALLOC

OBJ_DRF_DEALLOC(struct_or_basic, StructOrBasicTypeObjectDerefData, {idf_dealloc_identifier(data->name);})
OBJ_DRF_DEALLOC(array, StructOrBasicTypeObjectDerefData, {idf_dealloc_identifier(data->name);})

void obj_drf_dealloc(ObjectDeref *obj_drf) {
  if (obj_drf == NULL)
    return;
  if (obj_drf->type == ARR_DEREF)
    obj_drf_dealloc_array_type_deref(obj_drf);
  else if (obj_drf->type == STRUCT_BASIC_DEREF)
    obj_drf_dealloc_struct_or_basic_type_deref(obj_drf);
  else
    OBJ_DRF_ERROR();
  free(obj_drf);
}


// ATTRIBUTE LIST

typedef struct ObjectDerefList_s{
  struct ObjectDerefList_s *next;
  ObjectDeref *obj_deref;
} ObjectDerefList;

//// CREATE EMPTY

ObjectDerefList *obj_drf_list_create_empty() {
  ObjectDerefList *list = (ObjectDerefList *) malloc(sizeof(ObjectDerefList));
  list->next = NULL;
  list->obj_deref = NULL;
  return list;
  // return NULL;
}

ObjectDerefList *obj_drf_list_create(ObjectDeref *obj_deref) {
  ObjectDerefList *list = (ObjectDerefList *) malloc(sizeof(ObjectDerefList));
  list->next = NULL;
  list->obj_deref = obj_deref;
  return list;
}

//// APPEND

void obj_drf_list_append(ObjectDerefList *list, ObjectDeref *obj_deref) {
  if (list->next == NULL) {
    if (list->obj_deref == NULL) {
      list->obj_deref = obj_deref;
    } else {
      list->next = obj_drf_list_create(obj_deref);
    }
    return;
  }

  ObjectDerefList *n = list->next;
  while(n->next != NULL)
    n = n->next;

  n->next = obj_drf_list_create(obj_deref);
  return;
}

//// DEALLOC

void obj_drf_list_dealloc(ObjectDerefList *list) {
  if (list == NULL)
    return;
  if (list->next != NULL)
    obj_drf_list_dealloc(list->next);
  
  if (list->obj_deref != NULL)
    obj_drf_dealloc(list->obj_deref);
  
  free(list);
}

//// SIZE

size_t obj_drf_list_size(ObjectDerefList *list) {
  if (list->next != NULL)
    return obj_drf_list_size(list->next) + 1;
  
  if (list->obj_deref != NULL)
    return 1;
  
  return 0;
}

//// GET AT

ObjectDeref *obj_drf_list_get_at(ObjectDerefList *list, size_t index) {
  if (list->next == NULL && list->obj_deref == NULL && index == 0)
    return NULL;
  assert(index < obj_drf_list_size(list));
  ObjectDerefList *n = list;
  for (size_t i=0; i<index; ++i)
    n = n->next;
  return n->obj_deref;
}

//// PRINT

void obj_drf_list_print(ObjectDerefList *list, FILE *file) {
  if_null_print(list, file);
  if (list->obj_deref != NULL)
    obj_drf_print(list->obj_deref, file);
  
  if (list->next != NULL){
    fprintf(file, ".");
    obj_drf_list_print(list->next, file);
  }
}

// END ATTRIBUTE LIST

#endif // end OBJ_DRF_HEADER