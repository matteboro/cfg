#pragma once

#include "../expr/obj_drf.h"
#include "../prsr/strct_decl.h"
#include "avlb_vars.h"
#include "chckr_env.h"


#define OBJ_DRF_CHCKR_ERROR_HEADER(obj_derefs) \
  fprintf(stdout, "ERROR: did not pass object deref analysis.\n\n  in object dereference:\n    "); \
  obj_drf_list_print(obj_derefs, stdout); fprintf(stdout, ",\n  ") 

// TODO: I think this function should be move to strct_decl.h with strct_decl_list prefix
StructDeclaration *obj_drf_chckr_get_struct_decl_from_identifier(StructDeclarationList *structs, Identifier *name) {
  FOR_EACH(StructDeclarationList, strct_it, structs) {
    if (idf_equal_identifiers(name, strct_it->node->name))
      return strct_it->node;
  }
  return NULL;
}

bool obj_drf_chckr_check_array_dereference(ObjectDeref *obj_drf,ObjectDerefList *obj_derefs, ASTCheckingAnalysisState *an_state) {
  if (not obj_drf_is_array_deref(obj_drf))
    return True;

  Expression *index = obj_drf_array_get_index(obj_drf);
  Type *index_type = expr_chckr_get_returned_type(index, an_state);

  if (index_type == NULL) {
    OBJ_DRF_CHCKR_ERROR_HEADER(obj_derefs);
    fprintf(stdout, "in: ");
    obj_drf_print(obj_drf, stdout);
    fprintf(stdout, "type of index expression is not valid\n\n");
    single_line_file_info_print_context(index->file_info, stdout); fprintf(stdout, "\n\n");
    return False;
  }

  if (!type_is_integer(index_type)) {
    OBJ_DRF_CHCKR_ERROR_HEADER(obj_derefs);
    fprintf(stdout, "in: ");
    obj_drf_print(obj_drf, stdout);
    fprintf(stdout, " type of index expression is not integer as expected but: ");
    type_print(index_type, stdout);
    fprintf(stdout, "\n\n");
    single_line_file_info_print_context(index->file_info, stdout); fprintf(stdout, "\n\n");
    type_dealloc(index_type);
    return False;
  }

  index = expr_chckr_simplify(index);
  obj_drf_array_set_index(obj_drf, index);

  // TODO: in case expression is integer check if it is less than the array size
  // NOTE: I have the object deref type at disposition

  type_dealloc(index_type);
  return True;
}

bool obj_drf_chckr_check_for_array_correspondence(Type *type, ObjectDeref *deref, ObjectDerefList *obj_derefs) {

  if ((type_is_array(type) && not obj_drf_is_array_deref(deref)) ||
      ((obj_drf_is_array_deref(deref)) && (not (type_is_array(type) || type_is_pointer(type))))) {
    OBJ_DRF_CHCKR_ERROR_HEADER(obj_derefs);
    fprintf(stdout, "object dereference: ");
    obj_drf_print(deref, stdout);
    fprintf(stdout, " should or should not be referenced as an array");
    fprintf(stdout, "\n\n");
    single_line_file_info_print_context(deref->file_info, stdout);
    fprintf(stdout, "\n\n");
    return False;
  }
  return True;
}

bool obj_drf_check_for_basic_type(Type *type, ObjectDerefList *obj_derefs, Identifier *idf) {
  if (type_is_basic(type)) {
    // TODO: better error
    fprintf(stdout, "ERROR, did not pass object deref analysis. Object dereference %s of ", idf->name);
    obj_drf_list_print(obj_derefs, stdout);
    fprintf(stdout, " is of basic type, expected struct\n");
    return True;
  }
  return False;
}

bool obj_drf_check_not_for_basic_type(Type *type, ObjectDerefList *obj_derefs, Identifier *idf) {
  if (!type_is_basic(type)) {
    // TODO: better error
    fprintf(stdout, "ERROR, did not pass object deref analysis. Object dereference %s of ", idf->name);
    obj_drf_list_print(obj_derefs, stdout);
    fprintf(stdout, " is not of basic type as expected\n");
    return True;
  }
  return False;
}

Type *obj_drf_check_last_element(
  ObjectDeref *last_elem, 
  Type *type, 
  ObjectDerefList *full_derefs, 
  Identifier *name) {
  // check for correspondance between type and dereference
  if ((not (type_is_array(type) || type_is_pointer(type))) && obj_drf_is_array_deref(last_elem)) {
    OBJ_DRF_CHCKR_ERROR_HEADER(full_derefs);
    fprintf(stdout, "object dereference: ");
    obj_drf_print(last_elem, stdout);
    fprintf(stdout, " should not be referenced as an array since it is not an array type but: ");
    type_print(type, stdout);
    fprintf(stdout, "\n\n");
    single_line_file_info_print_context(last_elem->file_info, stdout);
    fprintf(stdout, "\n\n");
    return NULL;
  } 
  else if ((not type_is_pointer(type)) && obj_drf_is_single_element_deref(last_elem)) {
    OBJ_DRF_CHCKR_ERROR_HEADER(full_derefs);
    fprintf(stdout, "object dereference: ");
    obj_drf_print(last_elem, stdout);
    fprintf(stdout, " should not be referenced as a single element pointer dereference since it is not of pointer type but: ");
    type_print(type, stdout);
    fprintf(stdout, "\n\n");
    single_line_file_info_print_context(last_elem->file_info, stdout);
    fprintf(stdout, "\n\n");
    return NULL;
  }

  // the last element should be a basic type (the ultimate type should be) or pointer
  if (obj_drf_check_not_for_basic_type(type, full_derefs, name))
    return NULL;

  // if type and deref are both array I have to return the ultimate type
  if ((type_is_array(type) || type_is_pointer(type)) && obj_drf_is_array_deref(last_elem)) {
    type = type_extract_ultimate_type(type);
  }
  else if (type_is_pointer(type) && obj_drf_is_single_element_deref(last_elem)) {
    type = type_extract_ultimate_type(type);
  }

  return type;
}

Type *obj_drf_chckr_check(ObjectDerefList *obj_derefs, ASTCheckingAnalysisState *an_state) {
  AvailableVariables* av_vars = chckr_analysis_state_get_av_vars(an_state); 
  StructDeclarationList* structs = chckr_analysis_state_get_structs(an_state); 

  assert(obj_drf_list_size(obj_derefs) > 0);

  // the first element of the object dereference should be an available variable
  ObjectDeref *first_elem = obj_drf_list_get_at(obj_derefs, 0);
  Var* var = avlb_vars_get_var_from_identifier(av_vars, first_elem->name);

  if (var == NULL) {
    OBJ_DRF_CHCKR_ERROR_HEADER(obj_derefs);
    fprintf(stdout, "var with name: "); 
    obj_drf_print(first_elem, stdout);
    fprintf(stdout, " does not exist\n\n");
    single_line_file_info_print_context(first_elem->file_info, stdout); 
    fprintf(stdout, "\n\n");
    return NULL;
  }

  // check in case is array dereference
  if (!obj_drf_chckr_check_array_dereference(first_elem, obj_derefs, an_state))
    return NULL;

  // if is an available var, extract type and name
  Type *var_type = var->nt_bind->type;
  Identifier *var_name = var->nt_bind->name;

  // set first element type
  obj_drf_set_real_type(first_elem, type_copy(var_type));

  // if is the last element
  if (obj_drf_list_size(obj_derefs) == 1) {
    var_type = obj_drf_check_last_element(first_elem, var_type, obj_derefs, var_name);
    return type_copy(var_type);
  }

  // if it a list of object dereferences

  if (!obj_drf_chckr_check_for_array_correspondence(var->nt_bind->type, first_elem, obj_derefs))
    return NULL;

  if (obj_drf_check_for_basic_type(var_type, obj_derefs, var_name))
    return NULL;

  // get the struct declaration of the first element 
  StructDeclaration *prev_struct = 
    obj_drf_chckr_get_struct_decl_from_identifier(
      structs, 
      type_struct_get_name(
        type_extract_ultimate_type(var_type)));
  Type *elem_type = NULL;

  FOR_EACH(ObjectDerefList, obj_drf_it, obj_derefs->next) {

    // get the attribute having the name of the attribute
    Attribute *deref_attribute = strct_decl_get_attribute_from_identifier(prev_struct, obj_drf_it->node->name);
    
    // if attribute does not exist in the struct
    if (deref_attribute == NULL) {
      OBJ_DRF_CHCKR_ERROR_HEADER(obj_derefs);
      fprintf(stdout, "object dereference: ");
      obj_drf_print(obj_drf_it->node, stdout);
      fprintf(stdout, " does not link to an attribute in the struct: ");
      idf_print_identifier(prev_struct->name, stdout);
      fprintf(stdout, "\n\n");
      single_line_file_info_print_context(obj_drf_it->node->file_info, stdout);
      fprintf(stdout, "\n\n");
      return NULL;
    }

    // extracte the type of the attribute
    elem_type = deref_attribute->nt_bind->type;

    // set the object deref type with the attribute type
    obj_drf_set_real_type(obj_drf_it->node, type_copy(elem_type));
    obj_drf_set_attribute(obj_drf_it->node, deref_attribute);

    // check in case is array dereference
    if (not obj_drf_chckr_check_array_dereference(obj_drf_it->node, obj_derefs, an_state))
      return NULL;

    // if it the last element
    if (obj_drf_it->next == NULL) { 
      elem_type = obj_drf_check_last_element(obj_drf_it->node, elem_type, obj_derefs, obj_drf_it->node->name);
      break;
    }

    if (obj_drf_check_for_basic_type(elem_type, obj_derefs, obj_drf_it->node->name))
      return NULL;

    if (not obj_drf_chckr_check_for_array_correspondence(elem_type, obj_drf_it->node, obj_derefs))
      return NULL;
    
    prev_struct = 
      obj_drf_chckr_get_struct_decl_from_identifier(structs, type_struct_get_name(type_extract_ultimate_type(elem_type)));
  }

  return type_copy(elem_type);
}