#pragma once

#include "../expr/obj_drf.h"
#include "../prsr/strct_decl.h"
#include "avlb_vars.h"

StructDeclaration *obj_drf_chckr_get_struct_decl_from_identifier(StructDeclarationList *structs, Identifier *name) {
  FOR_EACH(StructDeclarationList, strct_it, structs) {
    if (idf_equal_identifiers(name, strct_it->node->name))
      return strct_it->node;
  }
  return NULL;
}

bool obj_drf_chckr_check(ObjectDerefList *obj_derefs, AvailableVariables *av_vars, StructDeclarationList *structs) {

  // check if dereference list is correct
  // the ultimate type of each internal element should be struct, and a basic type for the last one
  // the name of the first element should be in av_vars, where I also get the type of the var
  // the name and type of the rest of the internal elements are in the appropriate struct in structs

  assert(obj_drf_list_size(obj_derefs) > 0);

  if (obj_drf_list_size(obj_derefs) == 1) {
    ObjectDeref *first_elem = obj_drf_list_get_at(obj_derefs, 0);
    Var* var = avlb_vars_get_var_from_identifier(av_vars, first_elem->name);

    if (var == NULL) {
      fprintf(stdout, "ERROR, did not pass object deref analysis. Var with name %s does not exist\n", first_elem->name->name);
      return False;
    }

    if (!type_is_basic(var->nt_bind->type)) {
      fprintf(stdout, "ERROR, did not pass object deref analysis. Type in name-type-bind ");
      nt_bind_print(var->nt_bind, stdout);
      fprintf(stdout, " is not basic, as expected\n");
      return False;
    }
  } 
  else { // (obj_drf_list_size(obj_derefs) > 1)
    ObjectDeref *first_elem = obj_drf_list_get_at(obj_derefs, 0);
    Var* var = avlb_vars_get_var_from_identifier(av_vars, first_elem->name);

    if (var == NULL) {
      fprintf(stdout, "ERROR, did not pass object deref analysis. Var with name %s does not exist\n", first_elem->name->name);
      return False;
    }

    if (type_is_basic(var->nt_bind->type)) {
      fprintf(stdout, "ERROR, did not pass object deref analysis. Variable %s ", var->nt_bind->name->name);
      obj_drf_list_print(obj_derefs, stdout);
      fprintf(stdout, " is of basic type, expected struct\n");
      return False;
    }

    StructDeclaration *prev_struct = 
      obj_drf_chckr_get_struct_decl_from_identifier(
        structs, 
        type_struct_get_name(
          type_extract_ultimate_type(var->nt_bind->type)));

    Type *elem_type = NULL;

    FOR_EACH(ObjectDerefList, obj_drf_it, obj_derefs->next) {
      elem_type = strct_decl_get_type_of_attribute_from_identifier(prev_struct, obj_drf_it->node->name);

      if (obj_drf_it->next == NULL) { // on last element
        if (!type_is_basic(elem_type)) {
          fprintf(stdout, "ERROR, did not pass object deref analysis. Last object dereference of ");
          obj_drf_list_print(obj_derefs, stdout);
          fprintf(stdout, " is not basic as expected\n");
          return False;
        }
        break;
      }

      if (type_is_basic(elem_type)) {
        fprintf(stdout, "ERROR, did not pass object deref analysis. Object dereference %s of ", obj_drf_it->node->name->name);
        obj_drf_list_print(obj_derefs, stdout);
        fprintf(stdout, " is basic, expected struct\n");
        return False;
      }
      
      prev_struct = 
        obj_drf_chckr_get_struct_decl_from_identifier(structs, type_struct_get_name(type_extract_ultimate_type(elem_type)));
    }
  }

  return True;
}