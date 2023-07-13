#ifndef STRCT_DECL_CHCKR_HEADER
#define STRCT_DECL_CHCKR_HEADER

#include "../prsr/ast.h"
#include "strct_graph.h"

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

bool strct_decl_chckr_check(ASTNode *program) {

  ProgramNodeData *program_data = (ProgramNodeData *) program->data;

  if(!strct_decl_chckr_check_double_attributes(program_data->struct_declarations)) {
    fprintf(stdout, "ERROR, did not pass struct declaration analysis. There are attributes with same names within a struct.\n");
    return False;
  }

  bool result = True;
  StructGraph* struct_graph = strct_graph_maker(program_data->struct_declarations);
  if(!strct_graph_analyzer(struct_graph)) {
    result = False;
  }

  strct_graph_dealloc(struct_graph);
  return result;
}


#endif // end STRCT_DECL_CHCKR_HEADER