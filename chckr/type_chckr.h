#pragma once

#include "../prsr/strct_decl.h"
#include "../expr/type.h"
#include "../prsr/assgnbl.h"
#include "avlb_vars.h"
#include "chckr_env.h"

bool type_chckr_type_exists(StructDeclarationList *, Type *);
Type *type_chckr_get_type_of_assignable_element(AssignableElement *, AvailableVariables *, StructDeclarationList *);

bool type_chckr_type_exists(StructDeclarationList *structs, Type *type) {

  if (type_is_basic(type))
    return True;

  Type *ult_type = type_extract_ultimate_type(type);

  // TODO : hide sequent information
  assert(ult_type->type == STRUCT_TYPE);
  Identifier *type_name = ((StructTypeData *)ult_type->data)->name;
  
  FOR_EACH(StructDeclarationList, strct_it, structs) {
    if (idf_equal_identifiers(strct_it->node->name, type_name)) {
      if (strct_decl_size_is_known(strct_it->node)) {
        type_set_ultimate_type_size(type, strct_decl_get_size(strct_it->node));
      }
      type_struct_set_struct_decl(ult_type, strct_it->node);
      return True;
    }
  }
  return False;
}

Type *type_chckr_get_type_of_assignable_element(
  AssignableElement *assgnbl, AvailableVariables *av_vars, StructDeclarationList *structs) {
  (void) assgnbl; (void) av_vars; (void) structs;  
  // ObjectDerefList *obj_deref_list = assgnbl->obj_derefs;
  return NULL;
}
