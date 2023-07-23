#pragma once

#include "../expr/obj_drf.h"
#include "../prsr/strct_decl.h"
#include "avlb_vars.h"
#include "chckr_env.h"

// TODO: I think this function should be move to strct_decl.h with strct_decl_list prefix
StructDeclaration *obj_drf_chckr_get_struct_decl_from_identifier(StructDeclarationList *structs, Identifier *name) {
  FOR_EACH(StructDeclarationList, strct_it, structs) {
    if (idf_equal_identifiers(name, strct_it->node->name))
      return strct_it->node;
  }
  return NULL;
}

bool obj_drf_chckr_check_array_dereference(ObjectDeref *obj_drf, ASTCheckingAnalysisState *an_state) {
  if (obj_drf->type != ARR_DEREF)
    return True;

  Expression *index = obj_drf_array_get_index(obj_drf);
  Type *index_type = expr_chckr_get_returned_type(index, an_state);

  if (index_type == NULL) {
    fprintf(stdout, "ERROR: did not pass object deref analysis.\n   Type of index expression is not valid\n\n");
    single_line_file_info_print_context(index->file_info, stdout); fprintf(stdout, "\n\n");
    return False;
  }

  if (!type_is_integer(index_type)) {
    fprintf(stdout, "ERROR: did not pass object deref analysis.\n   Type of index expression is not integer as expected but: ");
    type_print(index_type, stdout);
    fprintf(stdout, "\n\n");
    single_line_file_info_print_context(index->file_info, stdout); fprintf(stdout, "\n\n");
    type_dealloc(index_type);
    return False;
  }

  // TODO: in case expression is integer check if it is less than the array size
  // NOTE: I have the object deref type at disposition

  type_dealloc(index_type);
  return True;
}

bool obj_drf_chckr_check_for_array_correspondence(Type *type, ObjectDeref *deref) {
  // TODO: better error
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

Type *obj_drf_chckr_check(ObjectDerefList *obj_derefs, ASTCheckingAnalysisState *an_state) {
  AvailableVariables* av_vars = chckr_analysis_state_get_av_vars(an_state); 
  StructDeclarationList* structs = chckr_analysis_state_get_structs(an_state); 

  assert(obj_drf_list_size(obj_derefs) > 0);

  // the first element of the object dereference should be an available variable
  ObjectDeref *first_elem = obj_drf_list_get_at(obj_derefs, 0);
  Var* var = avlb_vars_get_var_from_identifier(av_vars, first_elem->name);

  if (var == NULL) {
    fprintf(stdout, "ERROR: did not pass object deref analysis.\n  in object dereference: "); 
    obj_drf_list_print(obj_derefs, stdout);
    fprintf(stdout, " var with name: "); 
    obj_drf_print(first_elem, stdout);
    fprintf(stdout, " does not exist\n\n");
    single_line_file_info_print_context(first_elem->file_info, stdout); fprintf(stdout, "\n\n");
    return NULL;
  }

  // check in case is array dereference
  if (!obj_drf_chckr_check_array_dereference(first_elem, an_state))
    return NULL;

  // if is an available var, extract type and name
  Type *var_type = var->nt_bind->type;
  Identifier *var_name = var->nt_bind->name;

  // set first element type
  obj_drf_set_real_type(first_elem, type_copy(var_type));

  // if is the last element
  if (obj_drf_list_size(obj_derefs) == 1) {
    if (var_type->type != ARR_TYPE && first_elem->type == ARR_DEREF) {
      // TODO: better error
      fprintf(stdout, "ERROR, did not pass object deref analysis. The object dereference ");
      obj_drf_print(first_elem, stdout);
      fprintf(stdout, " should not be referenced as an array\n");
      return NULL;
    } 
    else if (var_type->type == ARR_TYPE && first_elem->type == ARR_DEREF) {
      var_type = type_extract_ultimate_type(var_type);
    }
    if (obj_drf_check_not_for_basic_type(var_type, obj_derefs, var_name))
      return NULL;

    return type_copy(var_type);
  }

  // (obj_drf_list_size(obj_derefs) > 1)
  if (!obj_drf_chckr_check_for_array_correspondence(var->nt_bind->type, first_elem))
    return NULL;

  if (obj_drf_check_for_basic_type(var_type, obj_derefs, var_name))
    return NULL;

  StructDeclaration *prev_struct = 
    obj_drf_chckr_get_struct_decl_from_identifier(
      structs, 
      type_struct_get_name(
        type_extract_ultimate_type(var_type)));

  Type *elem_type = NULL;
  FOR_EACH(ObjectDerefList, obj_drf_it, obj_derefs->next) {

    // get the attribute type having the name of the attribute
    elem_type = strct_decl_get_type_of_attribute_from_identifier(prev_struct, obj_drf_it->node->name);

    // if attribute does not exist in the struct
    if (elem_type == NULL) {
      // TODO: better error
      fprintf(stdout, "ERROR, did not pass object deref analysis. Object dereference %s of ", obj_drf_it->node->name->name);
      obj_drf_list_print(obj_derefs, stdout);
      fprintf(stdout, " is not an attribute in the struct %s\n", prev_struct->name->name);
      return NULL;
    }

    // set the object deref type with the attribute type
    obj_drf_set_real_type(obj_drf_it->node, type_copy(elem_type));

    // check in case is array dereference
    if (!obj_drf_chckr_check_array_dereference(obj_drf_it->node, an_state))
      return NULL;

    if (obj_drf_it->next == NULL) { // on last element
      if (obj_drf_check_not_for_basic_type(elem_type, obj_derefs, obj_drf_it->node->name))
        return NULL;

      if (elem_type->type == ARR_TYPE && obj_drf_it->node->type == ARR_DEREF) {
        elem_type = type_extract_ultimate_type(elem_type);
      } 
      else if ((obj_drf_it->node->type == ARR_DEREF && elem_type->type != ARR_TYPE)) {
        // TODO: better error
        fprintf(stdout, "ERROR, did not pass object deref analysis. The object dereference ");
        obj_drf_print(obj_drf_it->node, stdout);
        fprintf(stdout, " should not be referenced as an array\n");
        elem_type = NULL;
      }
      break;
    }

    if (obj_drf_check_for_basic_type(elem_type, obj_derefs, obj_drf_it->node->name))
      return NULL;
    if (!obj_drf_chckr_check_for_array_correspondence(elem_type, obj_drf_it->node))
      return NULL;
    
    prev_struct = 
      obj_drf_chckr_get_struct_decl_from_identifier(structs, type_struct_get_name(type_extract_ultimate_type(elem_type)));
  }

  return type_copy(elem_type);
}