#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../lxr/lxr.h"
#include "../utility/list.h"

// IDENTIFIER

typedef struct {
  char *name;
  FileInfo file_info;
} Identifier;

Identifier *idf_create_identifier(const char *name, FileInfo f_info);
void idf_dealloc_identifier(Identifier *id);
void idf_print_identifier(Identifier *id, FILE *file);
Identifier *idf_create_identifier_from_token(Token token);

Identifier *idf_create_identifier(const char *name, FileInfo f_info) {
  Identifier *id = (Identifier *)malloc(sizeof(Identifier));
  id->name = (char *) malloc(strlen(name)+1);
  id->file_info = f_info;
  strcpy(id->name, name);
  return id;
}

Identifier *idf_copy_identifier(Identifier *id) {
  return idf_create_identifier(id->name, id->file_info);
} 

bool idf_equal_identifiers(Identifier *id1, Identifier *id2) {
  return strcmp(id1->name, id2->name) == 0;
}

void idf_dealloc_identifier(Identifier *id) {
  free(id->name);
  free(id);
}

void idf_print_identifier(Identifier *id, FILE *file) {
  fprintf(file, "%s", id->name);
} 

Identifier *idf_create_identifier_from_token(Token token) {
  // TODO : error handling, assume (token.type == IDENTIFIER_TOKEN)
  Identifier *id = (Identifier *)malloc(sizeof(Identifier));
  // id->name = (char *) malloc(token.data_length+1);
  id->name = lxr_get_token_data_as_cstring(token);
  id->file_info = token.file_info;
  return id;
}

// IDENTIFIER LIST

LIST(idf, Identifier, idf_dealloc_identifier, idf_print_identifier)

int idf_list_find(IdentifierList *list, Identifier *id) {
  int counter = 0;
  for (IdentifierList *it = list; (it != NULL && it->node != NULL); it = it->next) 
  {
    if (idf_equal_identifiers(it->node, id))
      return counter;
    ++counter;
  }
  return -1;
}