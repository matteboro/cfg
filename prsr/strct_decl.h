#pragma once

#include "../expr/idf.h"
#include "attrb.h"
#include "../utility/list.h"

#define print_spaces(n, file) {for (size_t spaces_counter=0; spaces_counter<n; ++spaces_counter) fprintf(file, " ");}

struct StructDeclaration;
typedef struct StructDeclaration StructDeclaration;

struct StructDeclaration {
  Identifier *name;
  AttributeList *attributes;
  FileInfo file_info;
  ByteSize size;
};

ByteSize strct_decl_get_size(StructDeclaration *decl) {
  assert(decl != NULL);
  return decl->size;
}

void strct_decl_set_size(StructDeclaration *decl, ByteSize size) {
  assert(decl != NULL);
  decl->size = size;
  return;
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
  assert(strct != NULL);
  return strct->size != NullByteSize;
}

// this function can be used only if every attribute of a struct type have the appropriate
// struct declaration paired with
ByteSize strct_decl_calculate_size(StructDeclaration *strct, StructDeclarationList *structs) {
  if (strct_decl_size_is_known(strct))
    return strct_decl_get_size(strct);
  ByteSize size = 0;
  FOR_EACH(AttributeList, attrb_it, strct->attributes) {
    if (attrb_size_is_known(attrb_it->node)) {
      size += attrb_get_size(attrb_it->node);
      continue;
    }
    Type *attrb_type = attrb_get_type(attrb_it->node);
    Type *attrb_ult_type = type_extract_ultimate_type(attrb_type);
    assert(type_is_struct(attrb_ult_type));
    StructDeclaration *struct_of_type = type_struct_get_struct_decl(attrb_ult_type);
    assert(struct_of_type != NULL);
    ByteSize struct_size = strct_decl_calculate_size(struct_of_type, structs);
    type_set_ultimate_type_size(attrb_type, struct_size);
    size += type_get_size(attrb_type);
  }
  return size;
}