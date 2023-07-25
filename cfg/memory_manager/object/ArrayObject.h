#pragma once

#include "ObjectHeader.h"

// DEFINITION

typedef struct {
  ArraySize array_size;
  Object **objects;
} ArrayObjectData;

Object *Object_Array_Create(ArraySize size, Type *objects_type);
void Object_Array_Destroy(Object *obj);

// IMPLEMENTATION

// this method DO NOT TAKE possession of Type *real_type (you do not have to pass a copy)
Object *Object_Array_Create(ArraySize size, Type *objects_type) {

  assert(size > 0);
  assert(objects_type != NULL);

  Object **objects = (Object **) malloc(sizeof(Object *)*size);
  for(size_t i = 0; i < size; ++i ) {
    objects[i] = Object_Create_From_Type(objects_type);
  }
  typed_data(ArrayObjectData);
  data->array_size = size;
  data->objects = objects;

  // TODO: I do not like: 
  //  - cast to int of size --> change type;
  //  - file_info_create_null() --> type_create_generic_array_type();
  return Object_Create(
    ARRAY_OBJ, 
    type_get_size(objects_type)*size, 
    data, 
    type_create_array_type((int) size, type_copy(objects_type), file_info_create_null()));
}

void Object_Array_Destroy(Object *obj) {
  assert(Object_Is_Array(obj));
  casted_data(ArrayObjectData, obj);

  for(size_t i=0; i<data->array_size; ++i) {
    Object_Destroy(data->objects[i]);
  }
  free(data);
}


