#ifndef STRCT_GRAPH_HEADER
#define STRCT_GRAPH_HEADER

#include "../prsr/ast.h"

struct StructGraphElem_s;
typedef struct StructGraphElem_s StructGraphElem;

struct StructGraph_s;
typedef struct StructGraph_s StructGraph;

StructGraph *strct_graph_maker(ASTNodeList *struct_declarations);
void strct_graph_dump_dot(StructGraph *struct_graph, FILE *file);
bool strct_graph_anlyzer(StructGraph *struct_graph);
void strct_graph_dealloc(StructGraph *struct_graph);
void strct_graph_element_dealloc(StructGraphElem *struct_graph_elem);

struct StructGraphElem_s {
  IdentifierList *sub_structs;
  Identifier *name;
};

struct StructGraph_s {
  StructGraphElem **structs;
  size_t num_structs;
};

StructGraph *strct_graph_maker(ASTNodeList *struct_declarations) {

  fprintf(stdout, "num. of structs: %lu\n", ast_list_size(struct_declarations));

  // INIT VALUES

  size_t num_structs = ast_list_size(struct_declarations);
  StructGraphElem **structs = (StructGraphElem **) malloc(sizeof(StructGraphElem *)*num_structs);

  // FILL STRUCTS

  // I iterate trhough each struct declaration
  for (size_t i=0; i<num_structs; ++i) 
  {
    ASTNode *elem = ast_list_get_at(struct_declarations, i);

    if (elem->type != STRUCT_DECLARATION)
    {
      fprintf(stdout, "ERROR in %s at fill structs step, element is not a struct declaration.\n", __FUNCTION__);
      exit(1);
    }

    StructDeclarationNodeData *struct_node_data = (StructDeclarationNodeData *)elem->data;

    StructGraphElem *graph_elem = (StructGraphElem *) malloc(sizeof(StructGraphElem));
    graph_elem->sub_structs = idf_list_create_empty();
    graph_elem->name = idf_copy_identifier(struct_node_data->name);
    
    AttributeList *attrbs = struct_node_data->attributes; 
    size_t num_attrbs = attrb_list_size(attrbs);

    // I iterate through each attribute of the struct:
    //   - if is struct I insert;
    //   - if is array I check if subtype is struct, if it is I insert;
    for (size_t j=0; j<num_attrbs; ++j) 
    {
      Attribute *attrb = attrb_list_get_at(attrbs, j);
      if (attrb->nt_bind->type->type == STRUCT_TYPE) 
      {
        StructTypeData *struct_type_data = (StructTypeData *)attrb->nt_bind->type->data;
        idf_list_append(graph_elem->sub_structs, idf_copy_identifier(struct_type_data->name));
      } 
      else if (attrb->nt_bind->type->type == ARR_TYPE) 
      {
        ArrayTypeData *array_type_data = (ArrayTypeData *)attrb->nt_bind->type->data;
        if (array_type_data->type->type == STRUCT_TYPE)
        {
          StructTypeData *struct_type_data = (StructTypeData *)array_type_data->type->data;
          idf_list_append(graph_elem->sub_structs, idf_copy_identifier(struct_type_data->name));
        }
      }
    }
    
    structs[i] = graph_elem;
  }

  // CREATE OBJECT

  StructGraph *graph = (StructGraph *) malloc(sizeof(StructGraph));
  graph->num_structs = num_structs;
  graph->structs = structs;

  return graph;
}

// ANALYZER

bool strct_graph_anlyzer(StructGraph *struct_graph) {
  fprintf(stdout, "%s unimplemented\n", __FUNCTION__);
  return False;
}

// DUMP DOT

void strct_graph_dump_dot(StructGraph *graph, FILE *file) {
  if (graph == NULL) 
  {
    fprintf(file, "NULL\n");
    return;
  }
  fprintf(file, "digraph structs {\n");
  for (size_t i=0; i<graph->num_structs; ++i) 
  {
    char *struct_name = graph->structs[i]->name->name;
    fprintf(file, "%s [shape=square, label=\"%s\"];\n", struct_name, struct_name);

    IdentifierList *sub_structs = graph->structs[i]->sub_structs;
    size_t num_sub_structs = idf_list_size(sub_structs);

    for (size_t j=0; j<num_sub_structs; ++j) 
    {
      char *sub_struct_name = idf_list_get_at(sub_structs, j)->name;
      fprintf(file, "%s -> %s;\n", sub_struct_name, struct_name);
    }
  }
  fprintf(file, "}\n");
}

// DEALLOC

void strct_graph_element_dealloc(StructGraphElem *graph_elem) {
  idf_list_dealloc(graph_elem->sub_structs);
  idf_dealloc_identifier(graph_elem->name);
  free(graph_elem);
}

void strct_graph_dealloc(StructGraph *graph) {
  if (graph == NULL)
    return;
  
  for (size_t i=0; i<graph->num_structs; ++i) {
    strct_graph_element_dealloc(graph->structs[i]);
  }
  free(graph->structs);
  free(graph);
}

#endif // end STRCT_GRAPH_HEADER