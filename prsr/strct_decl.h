#pragma once

#include "../expr/idf.h"
#include "attrb.h"
#include "../utility/list.h"

#define print_spaces(n, file) {for (size_t spaces_counter=0; spaces_counter<n; ++spaces_counter) fprintf(file, " ");}


typedef struct {
  Identifier *name;
  AttributeList *attributes;
  FileInfo file_info;
  ByteSize size;
} StructDeclaration;

ByteSize strct_decl_get_size(StructDeclaration *decl) {
  if (decl)
    return decl->size;
  return NullByteSize;
}

StructDeclaration *strct_decl_create(Identifier *name, AttributeList *attributes, FileInfo file_info);
void strct_decl_dealloc(StructDeclaration *decl);
void strct_decl_print(StructDeclaration *decl, FILE *file);
void strct_decl_print_ident(StructDeclaration *decl, FILE *file, size_t ident);

Type *strct_decl_get_type_of_attribute_from_identifier(StructDeclaration *strct, Identifier *name);

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

StructDeclaration *strct_decl_create(Identifier *name, AttributeList *attributes, FileInfo file_info) {
  StructDeclaration *decl = (StructDeclaration *)malloc(sizeof(StructDeclaration));
  decl->attributes = attributes;
  decl->name = name;
  decl->file_info = file_info;
  ByteSize size = NullByteSize;
  if (attrb_list_check_for_all_unary_predicate(attributes, attrb_size_is_known)) {
    size = 0;
    FOR_EACH(AttributeList, attrb_it, attributes) {
      size += attrb_get_size(attrb_it->node);
    }
  }
  decl->size = size;
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
  strct_decl_print_ident(decl, file, 0);
}

void strct_decl_print_ident(StructDeclaration *decl, FILE *file, size_t ident) {
  print_spaces(ident, file);
  if_null_print(decl, file);
  idf_print_identifier(decl->name, file);
#if TYPE_PRINT_SIZE_SWITCH
  fprintf(file, ":");
  red(file);
  decl->size != NullByteSize ? fprintf(file, "%lu", decl->size) : fprintf(file, "?");
  reset(file);
#endif
  fprintf(file, " {\n");
  FOR_EACH(AttributeList, attrb_it, decl->attributes) {
    print_spaces(ident+2, file);
    attrb_print(attrb_it->node, file);
    fprintf(file, "\n");
  }
  print_spaces(ident, file);
  fprintf(file, "}");
}

// UTILITY

Type *strct_decl_get_type_of_attribute_from_identifier(StructDeclaration *strct, Identifier *name) {
  FOR_EACH(AttributeList, attrb_it, strct->attributes) {
    if (idf_equal_identifiers(attrb_it->node->nt_bind->name, name)) {
      return attrb_it->node->nt_bind->type;
    }
  }
  return NULL;
}

bool strct_decl_size_is_known(StructDeclaration *strct) {
  if (strct == NULL)
    return False;
  return strct->size != NullByteSize;
}


// TODO: after I have the struct declaration inside struct type I can put this function after 
//       I checked every type in the statements

ByteSize strct_decl_calculate_size(StructDeclaration *strct, StructDeclarationList *structs) {
  if (strct_decl_size_is_known(strct))
    return strct_decl_get_size(strct);
  
  ByteSize size = 0;

  FOR_EACH(AttributeList, attrb_it, strct->attributes) {
    if (attrb_size_is_known(attrb_it->node)) {
      size += attrb_get_size(attrb_it->node);
      continue;
    }
    Type *type = attrb_get_type(attrb_it->node);
    Type *ult_type = type_extract_ultimate_type(type);
    // TO FINISH
  }
}