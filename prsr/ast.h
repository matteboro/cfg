#ifndef AST_HEADER
#define AST_HEADER

#include "../expr/expr.h"
#include "assgnbl.h"
#include <assert.h>
#include "../utility/list.h"
#include "strct_decl.h"
#include "stmnt.h"
#include "func_decl.h"

// #define AST_DEBUG
#ifdef AST_DEBUG
#define AST_PRINT_DEBUG() fprintf(stdout, "%s\n", __FUNCTION__); 
#else
#define AST_PRINT_DEBUG()
#endif
#define AST_ERROR() fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1);


typedef enum {
  EXPRESSION,
  NODES,
  FUNC_DECLARATION,
  PROGRAM,
  STATEMENT,
} ASTNodeType;

// DEFINITIONS

typedef void ASTNodeData;

typedef struct {
  ASTNodeType type;
  ASTNodeData *data;
} ASTNode;

void ast_print_node_ident(ASTNode *node, FILE *file, size_t ident);
void ast_print_node(ASTNode *node, FILE *file);
void ast_dealloc_node(ASTNode *root);

// ASTNODE LIST

/*
// 

// typedef struct ASTNodeList_s{
//   struct ASTNodeList_s *next;
//   ASTNode *node;
// } ASTNodeList;

// //// CREATE

// ASTNodeList *ast_list_create_empty() {
//   ASTNodeList *list = (ASTNodeList *)malloc(sizeof(ASTNodeList));
//   list->next = NULL;
//   list->node = NULL;
//   return list;
//   // return NULL;
// }

// ASTNodeList *ast_list_create(ASTNode *node) {
//   ASTNodeList *list = (ASTNodeList *)malloc(sizeof(ASTNodeList));
//   list->next = NULL;
//   list->node = node;
//   return list;
// }

//// APPEND

// void ast_list_append(ASTNodeList *list, ASTNode *node) {
//   if (list->next == NULL) {
//     if (list->node == NULL) {
//       list->node = node;
//     } else {
//       list->next = ast_list_create(node);
//     }
//     return;
//   }

//   ASTNodeList *n = list->next;
//   while(n->next != NULL)
//     n = n->next;

//   n->next = ast_list_create(node);
//   return;
// }

//// DEALLOC

// void ast_list_dealloc(ASTNodeList *list) {
//   if (list == NULL)
//     return;
//   if (list->next != NULL)
//     ast_list_dealloc(list->next);
  
//   if (list->node != NULL)
//     ast_dealloc_node(list->node);
  
//   free(list);
// }

//// SIZE

// size_t ast_list_size(ASTNodeList *list) {
//   if (list->next != NULL)
//     return ast_list_size(list->next) + 1;
  
//   if (list->node != NULL)
//     return 1;
  
//   return 0;
// }

//// GET AT

// ASTNode *ast_list_get_at(ASTNodeList *list, size_t index) {
//   if (list->next == NULL && list->node == NULL && index == 0)
//     return NULL;
//   assert(index < ast_list_size(list));
//   ASTNodeList *n = list;
//   for (size_t i=0; i<index; ++i)
//     n = n->next;
//   return n->node;
// }

//// PRINT

// void ast_list_print(ASTNodeList *list, FILE *file) {
//   if_null_print(list, file);
//   if (list->node != NULL)
//     ast_print_node(list->node, file);
  
//   if (list->next != NULL){
//     fprintf(file, ", ");
//     ast_list_print(list->next, file);
//   }
// }

// LIST(ast, ASTNode, ast_dealloc_node, ast_print_node)

// DEFAULT_LIST_IMPLEMENTATION(ASTNode)                    
// DEFAULT_LIST_CREATE_EMPTY(ast, ASTNode)              
// DEFAULT_LIST_CREATE(ast, ASTNode)     
// DEFAULT_LIST_APPEND(ast, ASTNode)                    
// DEFAULT_LIST_DEALLOC(ast, ASTNode, ast_dealloc_node)     
// DEFAULT_LIST_SIZE(ast, ASTNode)                      
// DEFAULT_LIST_GET_AT(ast, ASTNode)                    
// DEFAULT_LIST_PRINT(ast, ASTNode, ast_print_node) 
*/

LIST(ast, ASTNode, ast_dealloc_node, ast_print_node)

void ast_list_print_ident(ASTNodeList *list, FILE *file, size_t ident) {
  if_null_print(list, file);
  if (list->node != NULL)
    ast_print_node_ident(list->node, file, ident);
  
  if (list->next != NULL){
    fprintf(file, ", \n");
    ast_list_print_ident(list->next, file, ident);
  }
}

// END ASTNODE LIST

/*
to implement a new type of AST node you have to define:
  - the data structure holding the data of that node;
  - the create function;
  - the print function (and modify the general one to redirect there);
  - the dealloc function (and modify the general one to redirect there);
*/

typedef struct {
  Expression *expression;
} ExpressionNodeData;

// TODO: we could have lists of nodes that accepts ASTNode of only certain type
typedef struct {
  ASTNodeList *nodes;
} NodeListData;

typedef struct {
  Identifier *func_name;
  ParameterList *params;
  ASTNode *statements;
} FunctionDeclarationNodeData;

typedef struct {
  ASTNode *func_declarations;
  ASTNode *global_statements;
  // ASTNode *struct_declarations;
  StructDeclarationList *struct_declarations;
} ProgramNodeData;

typedef struct {
  Statement *stmnt;
} StatementNodeData;

// CREATE

ASTNode *ast_create_node(ASTNodeType type, ASTNodeData *data) {
  ASTNode *node = (ASTNode *) malloc(sizeof(ASTNode));
  node->type = type;
  node->data = data;
  return node;
}

ASTNode *ast_create_program_node(
  ASTNode *func_declarations, 
  ASTNode *global_statements, 
  StructDeclarationList *struct_declarations) {
  ProgramNodeData *data = (ProgramNodeData *) malloc(sizeof(ProgramNodeData));
  data->func_declarations = func_declarations;
  data->global_statements = global_statements;
  data->struct_declarations = struct_declarations;
  return ast_create_node(PROGRAM, data);
}

ASTNode *ast_create_statement_node(Statement *stmnt) {
  StatementNodeData *data = (StatementNodeData *)malloc(sizeof(StatementNodeData));
  data->stmnt = stmnt;
  return ast_create_node(STATEMENT, data);
}

ASTNode *ast_create_expression_node(Expression *expression) {
  ExpressionNodeData *data = (ExpressionNodeData *) malloc(sizeof(ExpressionNodeData));
  data->expression = expression;
  return ast_create_node(EXPRESSION, data);
}

ASTNode *ast_create_node_list_node(ASTNodeList *list) {
  NodeListData *data = (NodeListData *) malloc(sizeof(NodeListData));
  data->nodes = list;
  return ast_create_node(NODES, data);
}

ASTNode *ast_create_func_declaration_node(Identifier *id, ParameterList *params, ASTNode *statements) {
  FunctionDeclarationNodeData *data = (FunctionDeclarationNodeData *) malloc(sizeof(FunctionDeclarationNodeData));
  data->func_name = id;
  data->params = params;
  data->statements = statements;
  return ast_create_node(FUNC_DECLARATION, data);
}

// PRINT

void ast_print_node_list_node(ASTNode *node, FILE *file, size_t ident);
void ast_print_func_declaration_node(ASTNode *node, FILE *file, size_t ident);

void ast_print_program_node(ASTNode *node, FILE *file, size_t ident) {
  AST_PRINT_DEBUG();
  assert(node->type == PROGRAM);
  ProgramNodeData *data = (ProgramNodeData *) node->data;

  print_spaces(ident, file);
  fprintf(file, "Program: {\n");

  print_spaces(ident, file);
  fprintf(file, "  Functions: {\n");

  //functions
  ast_print_node_ident(data->func_declarations, file, ident+4);
  fprintf(file, "\n");

  print_spaces(ident, file);
  fprintf(file, "  }\n");

  print_spaces(ident, file);
  fprintf(file, "  Data Structs: {\n");

  //structs
  strct_decl_list_print_ident(data->struct_declarations, file, ident+4);
  fprintf(file, "\n");

  print_spaces(ident, file);
  fprintf(file, "  }\n");

  print_spaces(ident, file);
  fprintf(file, "  Global Statements: {\n");

  //statements
  ast_print_node_ident(data->global_statements, file, ident+4);
  fprintf(file, "\n");

  print_spaces(ident, file);
  fprintf(file, "  }\n");

  print_spaces(ident, file);
  fprintf(file, "}");
}

void ast_print_statement_node(ASTNode *node, FILE *file, size_t ident) {
  AST_PRINT_DEBUG();
  assert(node->type == STATEMENT);
  (void) ident;
  StatementNodeData *data = (StatementNodeData *) node->data;
  stmnt_print_ident(data->stmnt, file, ident);
}

void ast_print_expression_node(ASTNode *node, FILE *file, size_t ident) {
  AST_PRINT_DEBUG();
  assert(node->type == EXPRESSION);
  ExpressionNodeData *data = (ExpressionNodeData *) node->data;
  print_spaces(ident, file);
  fprintf(file, "Expression: < ");
  expr_print_expression(data->expression, file);
  fprintf(file, " >\n");
}

void ast_print_node_ident(ASTNode *node, FILE *file, size_t ident) {
  if (node->type == EXPRESSION) {
    ast_print_expression_node(node, file, ident);
  } else if (node->type == NODES) {
    ast_print_node_list_node(node, file, ident);
  } else if (node->type == FUNC_DECLARATION) {
    ast_print_func_declaration_node(node, file, ident);
  } else if (node->type == PROGRAM) {
    ast_print_program_node(node, file, ident);
  } else if (node->type == STATEMENT) {
    ast_print_statement_node(node, file, ident);
  } else {
    AST_ERROR();
  }
}

void ast_print_node(ASTNode *node, FILE *file) {
  ast_print_node_ident(node, file, 0);
}

void ast_print_node_list_node(ASTNode *node, FILE *file, size_t ident) {
  AST_PRINT_DEBUG();
  assert(node->type == NODES);
  NodeListData *data = (NodeListData *) node->data;
  print_spaces(ident, file);
  fprintf(file, "Nodes: [\n");
  ast_list_print_ident(data->nodes, file, ident+2);
  fprintf(file, "\n");
  print_spaces(ident, file);
  fprintf(file, "]");
}

void ast_print_func_declaration_node(ASTNode *node, FILE *file, size_t ident) {
  AST_PRINT_DEBUG();
  assert(node->type == FUNC_DECLARATION);
  FunctionDeclarationNodeData *data = (FunctionDeclarationNodeData *) node->data;

  print_spaces(ident, file);
  fprintf(file, "Function Declaration: {\n");

  print_spaces(ident, file);
  fprintf(file, "  Name: ");
  idf_print_identifier(data->func_name, file);
  fprintf(file, ",\n");

  print_spaces(ident, file);
  fprintf(file, "  Parameters: ");
  prmt_list_print(data->params, file);
  fprintf(file, ",\n");

  ast_print_node_ident(data->statements, file, ident+2);

  fprintf(file, "\n");
  print_spaces(ident, file);
  fprintf(file, "}");
}


// DEALLOC

void ast_dealloc_program_node(ASTNode *node) {
  AST_PRINT_DEBUG();
  assert(node->type == PROGRAM);
  ProgramNodeData *data = (ProgramNodeData *) node->data;
  ast_dealloc_node(data->func_declarations);
  ast_dealloc_node(data->global_statements);
  strct_decl_list_dealloc(data->struct_declarations);
  free(data);
}

void ast_dealloc_statement_node(ASTNode *node) {
  AST_PRINT_DEBUG();
  assert(node->type == STATEMENT);
  StatementNodeData *data = (StatementNodeData *) node->data;
  stmnt_dealloc(data->stmnt);
  free(data);
}

void ast_dealloc_expression_node(ASTNode *node) {
  AST_PRINT_DEBUG();
  assert(node->type == EXPRESSION);
  ExpressionNodeData *data = (ExpressionNodeData *) node->data;
  expr_dealloc_expression(data->expression);
  free(data);
}

void ast_dealloc_func_declaration_node(ASTNode *node) {
  AST_PRINT_DEBUG();
  assert(node->type == FUNC_DECLARATION);
  FunctionDeclarationNodeData *data = (FunctionDeclarationNodeData *) node->data;
  ast_dealloc_node(data->statements);
  idf_dealloc_identifier(data->func_name);
  prmt_list_dealloc(data->params);
  free(data);
}

void ast_dealloc_node_list_node(ASTNode *node) {
  AST_PRINT_DEBUG();
  assert(node->type == NODES); 
  NodeListData *data = (NodeListData *) node->data;
  ast_list_dealloc(data->nodes);
  free(data);
}

void ast_dealloc_node(ASTNode *root) {
  AST_PRINT_DEBUG();
  if (root == NULL)
    return;
  if (root->type == EXPRESSION) {
    ast_dealloc_expression_node(root);
  } else if (root->type == NODES) {
    ast_dealloc_node_list_node(root);
  } else if (root->type == FUNC_DECLARATION) {
    ast_dealloc_func_declaration_node(root);
  } else if (root->type == PROGRAM) {
    ast_dealloc_program_node(root);
  } else if (root->type == STATEMENT) {
    ast_dealloc_statement_node(root);
  } else {
    AST_ERROR();
  }
  free(root);
}

#endif // end AST_HEADER