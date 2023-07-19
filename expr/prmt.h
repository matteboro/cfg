#pragma once

#include "nt_bind.h"

#define PRMT_ERROR() { fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1); }
#define if_null_print(ptr, file) if (ptr == NULL) { fprintf(file, "NULL"); return; }

struct Parameter_s;
typedef struct Parameter_s Parameter;

Parameter *prmt_create(NameTypeBinding *);
void prmt_print(Parameter *, FILE *);
void prmt_dealloc(Parameter *);

struct Parameter_s {
  NameTypeBinding *nt_bind;
  FileInfo file_info;
};

LIST(prmt, Parameter, prmt_dealloc, prmt_print)
DEFAULT_LIST_MERGED_FILE_INFO(prmt, Parameter)

Parameter *prmt_create(NameTypeBinding *nt_bind) {
  Parameter *prmt = (Parameter *) malloc(sizeof(Parameter));
  prmt->nt_bind = nt_bind;
  prmt->file_info = nt_bind->file_info;
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
