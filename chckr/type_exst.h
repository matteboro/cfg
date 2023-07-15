#pragma once

#include "../prsr/strct_decl.h"
#include "../expr/type.h"

bool type_exists(StructDeclarationList *, Type *);

bool type_exists(StructDeclarationList *structs, Type *type) {
  type = type_extract_ultimate_type(type);
  if (type->type == INT_TYPE || type->type == STRING_TYPE)
    return True;
  
  Identifier *type_name = ((StructTypeData *)type->data)->name;
  FOR_EACH(StructDeclarationList, strct_it, structs) {
    if (idf_equal_identifiers(strct_it->node->name, type_name))
      return True;
  }
  return False;
}