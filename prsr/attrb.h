#pragma once

#include "../expr/nt_bind.h"

#define if_null_print(ptr, file) if (ptr == NULL) { fprintf(file, "NULL"); return; }

struct Attribute {
  NameTypeBinding *nt_bind;
  FileInfo file_info;
  int64_t relative_position;
};

typedef struct Attribute Attribute;

void attrb_set_relative_position(Attribute *attrb, int64_t relative_position) {
  assert(attrb != NULL);
  assert(relative_position >= 0);
  attrb->relative_position = relative_position;
}

int64_t attrb_get_relative_position(Attribute *attrb) {
  assert(attrb != NULL);
  return attrb->relative_position;
}

bool attrb_relative_position_is_valid(Attribute *attrb) {
  assert(attrb != NULL);
  return attrb->relative_position >= 0;
}

typedef struct Attribute Attribute;

Attribute *attrb_create(NameTypeBinding *nt_bind) {
  Attribute *attribute = (Attribute *) malloc(sizeof(Attribute));
  attribute->nt_bind = nt_bind;
  attribute->file_info = nt_bind->file_info;
  attribute->relative_position = -1;
  return attribute;
}

void attrb_print(Attribute *attribute, FILE *file) {
  nt_bind_print(attribute->nt_bind, file);
  fprintf(file, ":%ld", attribute->relative_position);
}

void attrb_dealloc(Attribute *attribute) {
  nt_bind_dealloc(attribute->nt_bind);
  free(attribute);
}

LIST(attrb, Attribute, attrb_dealloc, attrb_print)
DEFAULT_LIST_MERGED_FILE_INFO(attrb, Attribute);

//UTILITY

bool attrb_size_is_known(Attribute *attrb) {
  if (attrb == NULL)
    return False;
  return type_size_is_known(attrb->nt_bind->type);
}

ByteSize attrb_get_size(Attribute *attrb) {
  if (attrb == NULL)
    return NullByteSize;
  return type_get_size(attrb->nt_bind->type);
}

Type *attrb_get_type(Attribute *attrb) {
  if (attrb)
    return attrb->nt_bind->type;
  return NULL;
}