#pragma once

#include "idf.h"
#include "type.h"

typedef struct {
  Identifier *name;
  Type *type;
  FileInfo file_info;
} NameTypeBinding;

NameTypeBinding *nt_bind_create(Identifier *name, Type *type) {
  NameTypeBinding *nt_bind = (NameTypeBinding *) malloc(sizeof(NameTypeBinding));
  nt_bind->name = name;
  nt_bind->type = type;
  nt_bind->file_info = file_info_merge(name->file_info, type->file_info);
  return nt_bind;
}

NameTypeBinding *nt_bind_copy(NameTypeBinding *nt_bind) {
  NameTypeBinding *nt_bind_copy = (NameTypeBinding *) malloc(sizeof(NameTypeBinding));
  nt_bind_copy->name = idf_copy_identifier(nt_bind->name);
  nt_bind_copy->type = type_copy(nt_bind->type);
  return nt_bind_copy;
}

void nt_bind_print(NameTypeBinding *nt_bind, FILE *file) {
  type_print(nt_bind->type, file);
  fprintf(file, " ");
  idf_print_identifier(nt_bind->name, file);
}

void nt_bind_dealloc(NameTypeBinding *nt_bind) {
  idf_dealloc_identifier(nt_bind->name);
  type_dealloc(nt_bind->type);
  free(nt_bind);
}
