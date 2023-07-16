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

bool obj_drf_chckr_check_for_array_correspondence(Type *type, ObjectDeref *deref) {
  // TODO: could have better error: object <obj> in <obj_deref_list> should etc...
  // TODO: here we could also check for the correctness of index (i.e. it is between boundaries)
  // TODO: should also check in case of multidimensional arrays (when they will be implemented)
  if ((deref->type != ARR_DEREF && type->type == ARR_TYPE) ||
      (deref->type == ARR_DEREF && type->type != ARR_TYPE)) {
    fprintf(stdout, "ERROR, did not pass object deref analysis. The object dereference ");
    obj_drf_print(deref, stdout);
    fprintf(stdout, " should or should not be referenced as an array\n");
    return False;
  }
  return True;
}

bool obj_drf_check_for_basic_type(Type *type, ObjectDerefList *obj_derefs, Identifier *idf) {
  if (type_is_basic(type)) {
    fprintf(stdout, "ERROR, did not pass object deref analysis. Object dereference %s of ", idf->name);
    obj_drf_list_print(obj_derefs, stdout);
    fprintf(stdout, " is of basic type, expected struct\n");
    return True;
  }
  return False;
}

bool obj_drf_check_not_for_basic_type(Type *type, ObjectDerefList *obj_derefs, Identifier *idf) {
  if (!type_is_basic(type)) {
    fprintf(stdout, "ERROR, did not pass object deref analysis. Object dereference %s of ", idf->name);
    obj_drf_list_print(obj_derefs, stdout);
    fprintf(stdout, " is not of basic type as expected\n");
    return True;
  }
  return False;
}

bool obj_drf_chckr_check(ObjectDerefList *obj_derefs, AvailableVariables *av_vars, StructDeclarationList *structs) {

  assert(obj_drf_list_size(obj_derefs) > 0);
  ObjectDeref *first_elem = obj_drf_list_get_at(obj_derefs, 0);
  Var* var = avlb_vars_get_var_from_identifier(av_vars, first_elem->name);

  if (var == NULL) {
    fprintf(stdout, "ERROR, did not pass object deref analysis. Var with name %s does not exist\n", first_elem->name->name);
    return False;
  }

  if (!obj_drf_chckr_check_for_array_correspondence(var->nt_bind->type, first_elem))
    return False;

  if (obj_drf_list_size(obj_derefs) == 1) {
    if (obj_drf_check_not_for_basic_type(var->nt_bind->type, obj_derefs, var->nt_bind->name))
      return False;
    return True;
  }

  // (obj_drf_list_size(obj_derefs) > 1)
  if (obj_drf_check_for_basic_type(var->nt_bind->type, obj_derefs, var->nt_bind->name))
    return False;

  StructDeclaration *prev_struct = 
    obj_drf_chckr_get_struct_decl_from_identifier(
      structs, 
      type_struct_get_name(
        type_extract_ultimate_type(var->nt_bind->type)));

  FOR_EACH(ObjectDerefList, obj_drf_it, obj_derefs->next) {
    Type *elem_type = strct_decl_get_type_of_attribute_from_identifier(prev_struct, obj_drf_it->node->name);
    if (obj_drf_it->next == NULL) { // on last element
      if (obj_drf_check_not_for_basic_type(elem_type, obj_derefs, obj_drf_it->node->name))
        return False;
      if (!obj_drf_chckr_check_for_array_correspondence(elem_type, obj_drf_it->node))
        return False;
      break;
    }

    if (obj_drf_check_for_basic_type(elem_type, obj_derefs, obj_drf_it->node->name))
      return False;
    if (!obj_drf_chckr_check_for_array_correspondence(elem_type, obj_drf_it->node))
      return False;
    
    prev_struct = 
      obj_drf_chckr_get_struct_decl_from_identifier(structs, type_struct_get_name(type_extract_ultimate_type(elem_type)));
  }


  return True;
}