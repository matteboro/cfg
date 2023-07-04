#ifndef NT_BIND_HEADER
#define NT_BIND_HEADER

#include "idf.h"
#include "type.h"

typedef struct {
  Identifier *name;
  Type *type;
} NameTypeBinding;

NameTypeBinding *nt_bind_create(Identifier *name, Type *type) {
  NameTypeBinding *nt_bind = (NameTypeBinding *) malloc(sizeof(NameTypeBinding));
  nt_bind->name = name;
  nt_bind->type = type;
  return nt_bind;
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

#endif // end NT_BIND_HEADER