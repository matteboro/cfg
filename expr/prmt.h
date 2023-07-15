#pragma once

#include "nt_bind.h"

#define PRMT_ERROR() { fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1); }
#define if_null_print(ptr, file) if (ptr == NULL) { fprintf(file, "NULL"); return; }

struct Parameter_s;
typedef struct Parameter_s Parameter;

Parameter *prmt_create(NameTypeBinding *);
void prmt_print(Parameter *, FILE *);
void prmt_dealloc(Parameter *);

LIST(prmt, Parameter, prmt_dealloc, prmt_print)

struct Parameter_s {
  NameTypeBinding *nt_bind;
};

Parameter *prmt_create(NameTypeBinding *nt_bind) {
  Parameter *prmt = (Parameter *) malloc(sizeof(Parameter));
  prmt->nt_bind = nt_bind;
  return prmt;
}

void prmt_print(Parameter *prmt, FILE *file) {
  if_null_print(prmt, file);
  nt_bind_print(prmt->nt_bind, file);
}

void prmt_dealloc(Parameter *prmt) {
  if (prmt == NULL) 
    return;
  nt_bind_dealloc(prmt->nt_bind);
  free(prmt);
}
