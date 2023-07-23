#pragma once

#include "../prsr/prgrm.h"
#include "strct_graph.h"
#include "type_chckr.h"
#include "chckr_env.h"

#define STRCT_DECL_CHCKR_ERROR_HEADER() \
  fprintf(stdout, "ERROR: did not pass struct declaration analysis.\n  ");


int strct_decl_chckr_count_identifier_in_attributes(Identifier *id, AttributeList *attrbs) {
  int counter = 0;
  FOR_EACH(AttributeList, attrb_it, attrbs) {
    if (idf_equal_identifiers(id, attrb_it->node->nt_bind->name))
      ++counter;
  }
  return counter;
}

bool attrb_same_name(Attribute *attrb1, Attribute *attrb2) {
  return idf_equal_identifiers(attrb1->nt_bind->name, attrb2->nt_bind->name);
}


bool strct_decl_chckr_check_double_attributes(StructDeclarationList *structs) {
  FOR_EACH(StructDeclarationList, strct_it, structs) {
    AttributeList *attrbs = strct_it->node->attributes;
    FOR_EACH(AttributeList, attrb_it, attrbs) {
      if (strct_decl_chckr_count_identifier_in_attributes(attrb_it->node->nt_bind->name, attrbs) > 1) {
        STRCT_DECL_CHCKR_ERROR_HEADER();
        fprintf(
          stdout, 
          "in struct %s attribute with name %s is declared more then one time\n\n", 
          strct_it->node->name->name, 
          attrb_it->node->nt_bind->name->name);
        single_line_file_info_print_context(attrb_it->node->file_info, stdout); fprintf(stdout, "\n\n");

        return False;
      }
    }
  }
  return True;
}

bool strct_decl_check_attribute_type_existence(StructDeclarationList *structs) {
  FOR_EACH(StructDeclarationList, strct_it, structs) {
    AttributeList *attrbs = strct_it->node->attributes;
    FOR_EACH(AttributeList, attrb_it, attrbs) {
      Type *type = attrb_it->node->nt_bind->type;
      if (!type_chckr_type_exists(structs, type)) {

        STRCT_DECL_CHCKR_ERROR_HEADER();
        fprintf(
          stdout, 
          "in struct %s attribute with name %s is of unknown type: ", 
          strct_it->node->name->name, 
          attrb_it->node->nt_bind->name->name); 
          type_print(type, stdout);
          fprintf(stdout, "\n\n");
        single_line_file_info_print_context(attrb_it->node->file_info, stdout); fprintf(stdout, "\n\n");

        return False;
      }
    }
  }
  return True;
}

bool strct_decl_chckr_check(StructDeclarationList *structs) {

  
  if(!strct_decl_chckr_check_double_attributes(structs)) {
    return False;
  }

  if(!strct_decl_check_attribute_type_existence(structs)) {
    return False;
  }

  bool result = True;
  StructGraph* struct_graph = strct_graph_maker(structs);
  if(!strct_graph_analyzer(struct_graph)) {
    result = False;
  }
  // strct_graph_dump_dot(struct_graph, stdout);
  strct_graph_dealloc(struct_graph);

  // now I can assume some properties of the struct declarations and I can calculate the ByteSize of each
  FOR_EACH(StructDeclarationList, strct_it, structs) {
    if (strct_decl_size_is_known(strct_it->node))
      continue;
    ByteSize size = strct_decl_calculate_size(strct_it->node, structs);
    strct_decl_set_size(strct_it->node, size);
  }

  return result;
}