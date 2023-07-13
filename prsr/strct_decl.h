#pragma once

#include "../expr/idf.h"
#include "attrb.h"
#include "../utility/list.h"

#define print_spaces(n, file) {for (size_t spaces_counter=0; spaces_counter<n; ++spaces_counter) fprintf(file, " ");}


typedef struct {
  Identifier *name;
  AttributeList *attributes;
} StructDeclaration;

StructDeclaration *strct_decl_create(Identifier *name, AttributeList *attributes);
void strct_decl_dealloc(StructDeclaration *decl);
void strct_decl_print(StructDeclaration *decl, FILE *file);
void strct_decl_print_ident(StructDeclaration *decl, FILE *file, size_t ident);

LIST(strct_decl, StructDeclaration, strct_decl_dealloc, strct_decl_print)

void strct_decl_list_print_ident(StructDeclarationList *list, FILE *file, size_t ident) {
  if_null_print(list, file);
  if (list->node != NULL) {
    strct_decl_print_ident(list->node, file, ident);
  }
  
  if (list->next != NULL){
    fprintf(file, ", \n");
    strct_decl_list_print_ident(list->next, file, ident);
  }
}

StructDeclaration *strct_decl_create(Identifier *name, AttributeList *attributes) {
  StructDeclaration *decl = (StructDeclaration *)malloc(sizeof(StructDeclaration));
  decl->attributes = attributes;
  decl->name = name;
  return decl;
}

void strct_decl_dealloc(StructDeclaration *decl) {
  if (decl == NULL)
    return;
  idf_dealloc_identifier(decl->name);
  attrb_list_dealloc(decl->attributes);
  free(decl);
}

void strct_decl_print(StructDeclaration *decl, FILE *file) {
  idf_print_identifier(decl->name, file);
  fprintf(file, ": ");
  attrb_list_print(decl->attributes, file);
}

void strct_decl_print_ident(StructDeclaration *decl, FILE *file, size_t ident) {
  print_spaces(ident, file);
  idf_print_identifier(decl->name, file);
  fprintf(file, ": ");
  attrb_list_print(decl->attributes, file);
}


