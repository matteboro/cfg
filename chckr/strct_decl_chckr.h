#ifndef STRCT_DECL_CHCKR_HEADER
#define STRCT_DECL_CHCKR_HEADER

#include "../prsr/prgrm.h"
#include "strct_graph.h"
#include "type_chckr.h"

int strct_decl_chckr_count_identifier_in_attributes(Identifier *id, AttributeList *attrbs) {
  int counter = 0;
  FOR_EACH(AttributeList, attrb_it, attrbs) {
    if (idf_equal_identifiers(id, attrb_it->node->nt_bind->name))
      ++counter;
  }

  // fprintf(stdout, "  counter = %d, for ", counter);
  // idf_print_identifier(id, stdout);
  // fprintf(stdout, "\n");
  
  return counter;
}

bool strct_decl_chckr_check_double_attributes(StructDeclarationList *structs) {
  FOR_EACH(StructDeclarationList, strct_it, structs) {

    // fprintf(stdout, "struct: ");
    // idf_print_identifier(((StructDeclarationNodeData *)node_it->node->data)->name, stdout);
    // fprintf(stdout, "\n");

    AttributeList *attrbs = strct_it->node->attributes;
    FOR_EACH(AttributeList, attrb_it, attrbs) {
      if (strct_decl_chckr_count_identifier_in_attributes(attrb_it->node->nt_bind->name, attrbs) > 1)
        return False;
    }
  }
  return True;
}

bool strct_decl_check_attribute_type_existence(StructDeclarationList *structs) {
  FOR_EACH(StructDeclarationList, strct_it, structs) {
    AttributeList *attrbs = strct_it->node->attributes;
    FOR_EACH(AttributeList, attrb_it, attrbs) {
      Type *type = attrb_it->node->nt_bind->type;
      if (!type_chckr_type_exists(structs, type))
        return False;
    }
  }
  return True;
}

bool strct_decl_chckr_check(StructDeclarationList *structs) {

  if(!strct_decl_chckr_check_double_attributes(structs)) {
    fprintf(stdout, "ERROR, did not pass struct declaration analysis. There are attributes with same names within a struct.\n");
    return False;
  }

  if(!strct_decl_check_attribute_type_existence(structs)) {
    fprintf(stdout, "ERROR, did not pass struct declaration analysis. One of the attribute has non existent type.\n");
    return False;
  }

  bool result = True;
  StructGraph* struct_graph = strct_graph_maker(structs);
  if(!strct_graph_analyzer(struct_graph)) {
    result = False;
  }

  strct_graph_dealloc(struct_graph);
  return result;
}


#endif // end STRCT_DECL_CHCKR_HEADER