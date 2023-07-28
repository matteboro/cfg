#pragma once

#include "ObjectHeader.h"
#include "../../../prsr/strct_decl.h"

// DEFINITION

typedef struct {
  StructDeclaration *struct_decl;
  size_t num_attributes;
  ObjectArray *attribute_objects;
} StructObjectData;


Object *Object_Struct_Create(StructDeclaration *struct_decl);
void Object_Struct_Destroy(Object *obj);

// IMPLEMENTATION

ObjectArray *StructDeclaration_To_ObjectArray(StructDeclaration *struct_decl) {

  size_t num_attributes = strct_decl_get_total_number_of_attributes(struct_decl); 
  ObjectArray *objects = ObjectArray_Create(num_attributes);

  FOR_EACH_ENUM(AttributeList, attrb_it, struct_decl->attributes, attrb_counter) {
    Type *attrb_type = attrb_get_type(attrb_it->node);

    if (type_is_struct(attrb_type)) {
      StructDeclaration *sub_struct = type_struct_get_struct_decl(attrb_type);
      ObjectArray *sub_struct_objects = StructDeclaration_To_ObjectArray(sub_struct);
      ObjectArray_PushArray(objects, sub_struct_objects);
    } 
    else if (type_is_array(attrb_type)) {
      size_t array_size = type_array_get_size(attrb_type);
      Type *sub_type = type_array_get_type(attrb_type);

      if (type_is_struct(sub_type)) {
        for (size_t i=0; i < array_size; ++i) {
          StructDeclaration *array_sub_struct = type_struct_get_struct_decl(sub_type);
          ObjectArray *array_sub_struct_objects = StructDeclaration_To_ObjectArray(array_sub_struct);
          ObjectArray_PushArray(objects, array_sub_struct_objects);
        }
      } 
      else {
        for (size_t i=0; i < array_size; ++i) {
          Object *obj = Object_Create_From_Type(sub_type);
          ObjectArray_Push(objects, obj);
        }
      }
    } 
    else {
      Object *obj = Object_Create_From_Type(attrb_type);
      ObjectArray_Push(objects, obj);
    }
  }

  // fprintf(stdout, "available space: %lu\n", ObjectArray_AvailableSpace(objects));
  assert(ObjectArray_Full(objects));
  return objects;
}

Object *Object_Struct_Create(StructDeclaration *struct_decl) {
  assert(struct_decl != NULL);

  size_t num_attributes = strct_decl_get_total_number_of_attributes(struct_decl);  
  ObjectArray *objects = StructDeclaration_To_ObjectArray(struct_decl);

  typed_data(StructObjectData);
  data->attribute_objects = objects;
  data->num_attributes = num_attributes;
  data->struct_decl = struct_decl;

  return Object_Create(STRUCT_OBJ, strct_decl_get_size(struct_decl), data, type_copy(struct_decl->real_type));
}

void Object_Struct_Destroy(Object *obj) {
  assert(Object_Is_Struct(obj));
  
  casted_data(StructObjectData, obj);
  ObjectArray_Destroy(data->attribute_objects);
  free(data);
}

void Object_Struct_Print(Object *obj, FILE *file) {
  assert(Object_Is_Struct(obj));
  casted_data(StructObjectData, obj);

  ObjectArray_Print(data->attribute_objects, file);
}