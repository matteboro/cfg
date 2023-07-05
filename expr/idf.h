#ifndef IDF_HEADER
#define IDF_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../lxr/lxr.h"

// IDENTIFIER

typedef struct {
  char *name;
} Identifier;

Identifier *idf_create_identifier(const char *name);
void idf_dealloc_identifier(Identifier *id);
void idf_print_identifier(Identifier *id, FILE *file);
Identifier *idf_create_identifier_from_token(Token token);

Identifier *idf_create_identifier(const char *name) {
  Identifier *id = (Identifier *)malloc(sizeof(Identifier));
  id->name = (char *) malloc(strlen(name)+1);
  strcpy(id->name, name);
  return id;
}

void idf_dealloc_identifier(Identifier *id) {
  free(id->name);
  free(id);
}

void idf_print_identifier(Identifier *id, FILE *file) {
  fprintf(file, "id:%s", id->name);
} 

Identifier *idf_create_identifier_from_token(Token token) {
  // TODO : error handling, assume (token.type == IDENTIFIER_TOKEN)
  Identifier *id = (Identifier *)malloc(sizeof(Identifier));
  id->name = (char *) malloc(token.data_length+1);
  id->name = lxr_get_token_data_as_cstring(token);
  return id;
}

// END IDENTIFIER
#endif // end IDF_HEADER