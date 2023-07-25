#pragma once

#include "ObjectHeader.h"
#include "../../../prsr/strct_decl.h"

// DEFINITION

typedef struct {
  StructDeclaration *struct_decl;
  size_t num_attributes;
  Object **attributes_objects;
} StructObjectData;


Object *Object_Struct_Create(StructDeclaration *struct_decl);
void Object_Struct_Destroy(Object *obj);

// IMPLEMENTATION

Object *Object_Struct_Create(StructDeclaration *struct_decl) {
  assert(struct_decl != NULL);

  size_t num_attributes = attrb_list_size(struct_decl->attributes);
  Object **attributes_objects = (Object **)(sizeof(Object *)*num_attributes);

  FOR_EACH_ENUM(AttributeList, attrb_it, struct_decl->attributes, attrb_counter) {
    Type *attrb_type = attrb_get_type(attrb_it->node);
    Object *obj = Object_Create_From_Type(attrb_type);
    attributes_objects[attrb_counter] = obj;
  }

  typed_data(StructObjectData);
  data->attributes_objects = attributes_objects;
  data->num_attributes = num_attributes;
  data->struct_decl = struct_decl;

  return Object_Create(STRUCT_OBJ, strct_decl_get_size(struct_decl), data, type_copy(struct_decl->real_type));
}

void Object_Struct_Destroy(Object *obj) {
  assert(Object_Is_Struct(obj));
  
  casted_data(StructObjectData, obj);
  for (size_t i=0; i < data->num_attributes; ++i)
    Object_Destroy(data->attributes_objects[i]);
  free(data->attributes_objects);
  free(data);
}
