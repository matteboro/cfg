#pragma once

#include "ComposedObject.h"

// this method DO NOT TAKE possession of Type *real_type (you do not have to pass a copy)
Object *Object_Array_Create(ArraySize size, Type *objects_type) {
  assert(size > 0);
  assert(objects_type != NULL);

  ObjectArray *objects = ObjectArray_Create(size);
  for(size_t i = 0; i < size; ++i ) {
    ObjectArray_Push(objects, Object_Create_From_Type(objects_type));
  }

  assert(ObjectArray_Full(objects));

  // TODO: I do not like: 
  //  - cast to int of size --> change type;
  //  - file_info_create_null() --> type_create_generic_array_type();
  return 
    Object_Composed_Create(
      type_get_size(objects_type)*size, 
      size, 
      objects, 
      NULL, 
      type_create_array_type((int) size, type_copy(objects_type), file_info_create_null()));
}