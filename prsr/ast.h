#ifndef AST_HEADER
#define AST_HEADER

#include "../expr/expr.h"
#include "assgnbl.h"
#include "attrb.h"
#include <assert.h>
#include "../utility/list.h"

// #define AST_DEBUG
#ifdef AST_DEBUG
#define AST_PRINT_DEBUG() fprintf(stdout, "%s\n", __FUNCTION__); 
#else
#define AST_PRINT_DEBUG()
#endif
#define AST_ERROR() fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1);

#define print_spaces(n, file) {for (size_t spaces_counter=0; spaces_counter<n; ++spaces_counter) fprintf(file, " ");}

typedef enum {
  EXPRESSION,
  ASSIGNMENT,
  NODES,
  DECLARATION,
  FUNC_DECLARATION,
  IF_STATEMENT,
  IF_ELSE_STATEMENT,
  WHILE_STATEMENT,
  PROGRAM,
  ARR_DECLARATION,
  STRUCT_DECLARATION,
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

typedef struct {
  AssignableElement *assgnbl;
  ASTNode *expression;
} AssignmentNodeData;

// TODO: we could have lists of nodes that accepts ASTNode of only certain type
typedef struct {
  ASTNodeList *nodes;
} NodeListData;

typedef struct {
  NameTypeBinding *nt_bind;
  bool has_init;
  ExpressionList *init_values;
} DeclarationNodeData;

typedef struct {
  Identifier *func_name;
  ParameterList *params;
  ASTNode *statements;
} FunctionDeclarationNodeData;

typedef struct {
  ASTNode *expression;
  ASTNode *body;
} ConditionalStatementNodeData;

typedef struct {
  ASTNode *expression;
  ASTNode *if_body;
  ASTNode *else_body;
} IfElseStatementNodeData;

typedef struct {
  ASTNode *func_declarations;
  ASTNode *global_statements;
  ASTNode *struct_declarations;
} ProgramNodeData;

typedef struct {
  Identifier *name;
  int size;
  ExpressionList *init_values; 
} ArrayDeclarationNodeData;

typedef struct {
  Identifier *name;
  AttributeList *attributes;
} StructDeclarationNodeData;

// CREATE

ASTNode *ast_create_node(ASTNodeType type, ASTNodeData *data) {
  ASTNode *node = (ASTNode *) malloc(sizeof(ASTNode));
  node->type = type;
  node->data = data;
  return node;
}

ASTNode *ast_create_struct_declaration_node(Identifier *name, AttributeList *attributes) {
  StructDeclarationNodeData *data = (StructDeclarationNodeData *) malloc(sizeof(StructDeclarationNodeData));
  data->name = name;
  data->attributes = attributes;
  return ast_create_node(STRUCT_DECLARATION, data);
}

ASTNode *ast_create_array_declaration_node(Identifier *name, int size, ExpressionList *init_values) {
  ArrayDeclarationNodeData *data = (ArrayDeclarationNodeData *) malloc(sizeof(ArrayDeclarationNodeData));
  data->init_values = init_values;
  data->size = size;
  data->name = name;  
  return ast_create_node(ARR_DECLARATION, data);
}

ASTNode *ast_create_program_node(
  ASTNode *func_declarations, 
  ASTNode *global_statements, 
  ASTNode *struct_declarations) {
  ProgramNodeData *data = (ProgramNodeData *) malloc(sizeof(ProgramNodeData));
  data->func_declarations = func_declarations;
  data->global_statements = global_statements;
  data->struct_declarations = struct_declarations;
  return ast_create_node(PROGRAM, data);
}

ASTNode *ast_create_assignment_node(AssignableElement *assgnbl, ASTNode *expression_node) {
  AssignmentNodeData *data = (AssignmentNodeData *) malloc(sizeof(AssignmentNodeData));
  data->assgnbl = assgnbl;
  data->expression = expression_node;
  return ast_create_node(ASSIGNMENT, data);
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

// here expression could be NULL, if it is the value has_init should be false
ASTNode *ast_create_declaration_node(NameTypeBinding *nt_bind, ExpressionList *init_values) {
  DeclarationNodeData *data = (DeclarationNodeData *) malloc(sizeof(DeclarationNodeData));
  data->nt_bind = nt_bind;
  data->has_init = init_values == NULL ? False : True;
  data->init_values = init_values;
  return ast_create_node(DECLARATION, data);
}

ASTNode *ast_create_if_node(ASTNode *expression, ASTNode *body) {
  ConditionalStatementNodeData *data = (ConditionalStatementNodeData *) malloc(sizeof(ConditionalStatementNodeData));
  data->expression = expression;
  data->body = body;
  return ast_create_node(IF_STATEMENT, data);
}

ASTNode *ast_create_if_else_node(ASTNode *expression, ASTNode *if_body, ASTNode *else_body) {
  IfElseStatementNodeData *data = (IfElseStatementNodeData *) malloc(sizeof(IfElseStatementNodeData));
  data->expression = expression;
  data->if_body = if_body;
  data->else_body = else_body;
  return ast_create_node(IF_ELSE_STATEMENT, data);
}

ASTNode *ast_create_while_node(ASTNode *expression, ASTNode *body) {
  ConditionalStatementNodeData *data = (ConditionalStatementNodeData *) malloc(sizeof(ConditionalStatementNodeData));
  data->expression = expression;
  data->body = body;
  return ast_create_node(WHILE_STATEMENT, data);
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

void ast_print_struct_declaration_node(ASTNode *node, FILE *file, size_t ident) {
  AST_PRINT_DEBUG();
  assert(node->type == STRUCT_DECLARATION);
  StructDeclarationNodeData *data = (StructDeclarationNodeData *) node->data;

  print_spaces(ident, file);
  fprintf(file, "Struct Declaration: {\n");

  print_spaces(ident, file);
  fprintf(file, "  Name: ");
  idf_print_identifier(data->name, file);
  fprintf(file, ",\n");

  print_spaces(ident, file);
  fprintf(file, "  Attributes: ");
  attrb_list_print(data->attributes, file);
  fprintf(file, "\n");

  print_spaces(ident, file);
  fprintf(file, "}");
}

void ast_print_array_declaration_node(ASTNode *node, FILE *file, size_t ident) {
  AST_PRINT_DEBUG();
  assert(node->type == ARR_DECLARATION);
  ArrayDeclarationNodeData *data = (ArrayDeclarationNodeData *) node->data;

  print_spaces(ident, file);
  fprintf(file, "Array Declaration: {\n");

  print_spaces(ident, file);
  fprintf(file, "  Idenfier: ");
  idf_print_identifier(data->name, file);
  fprintf(file, ",\n");

  if (data->init_values != NULL) {
    print_spaces(ident, file);
    fprintf(file, "  Size: %d, \n", data->size);
    print_spaces(ident, file);
    fprintf(file, "  Init Values: [ ");
    expr_list_print(data->init_values, file);
    fprintf(file, " ]\n");
  } else {
    print_spaces(ident, file);
    fprintf(file, "  Size: %d \n", data->size);
  }

  print_spaces(ident, file);
  fprintf(file, "}");
}

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
  ast_print_node_ident(data->struct_declarations, file, ident+4);
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

void ast_print_expression_node(ASTNode *node, FILE *file, size_t ident) {
  AST_PRINT_DEBUG();
  assert(node->type == EXPRESSION);
  ExpressionNodeData *data = (ExpressionNodeData *) node->data;
  print_spaces(ident, file);
  fprintf(file, "Expression: < ");
  expr_print_expression(data->expression, file);
  fprintf(file, " >\n");
}

void ast_print_assignment_node(ASTNode *node, FILE *file, size_t ident) {
  AST_PRINT_DEBUG();
  assert(node->type == ASSIGNMENT);
  AssignmentNodeData *data = (AssignmentNodeData *) node->data;
  print_spaces(ident, file);
  fprintf(file, "Assignment: {\n");
  print_spaces(ident, file);
  fprintf(file, "  Assignable: ");
  assgnbl_print(data->assgnbl, file);
  //idf_print_identifier(data->id, file);
  fprintf(file, ",\n");
  ast_print_expression_node(data->expression, file, ident+2);
  print_spaces(ident, file);
  fprintf(file, "}");
}

void ast_print_declaration_node(ASTNode *node, FILE *file, size_t ident) {
  AST_PRINT_DEBUG();
  assert(node->type == DECLARATION);
  DeclarationNodeData *data = (DeclarationNodeData *) node->data;
  print_spaces(ident, file);
  fprintf(file, "Declaration: {\n");
  print_spaces(ident, file);
  fprintf(file, "  Idenfier: ");
  nt_bind_print(data->nt_bind, file);
  //idf_print_identifier(data->id, file);
  if (data->has_init) {
    fprintf(file, ", \n");
    print_spaces(ident, file);
    fprintf(file, "  Init Value/s: ");
    if (expr_list_size(data->init_values) > 1)
      fprintf(file, "[ ");
    expr_list_print(data->init_values, file);
    if (expr_list_size(data->init_values) > 1)
      fprintf(file, " ]");
    fprintf(file, "\n");
  } else {
    fprintf(file, "\n");
  }
  
  print_spaces(ident, file);
  fprintf(file, "}");
}

void ast_print_if_node(ASTNode *node, FILE *file, size_t ident) {
  AST_PRINT_DEBUG();
  assert(node->type == IF_STATEMENT);
  ConditionalStatementNodeData *data = (ConditionalStatementNodeData *) node->data;

  print_spaces(ident, file);
  fprintf(file, "If: {\n");

  print_spaces(ident, file);
  fprintf(file, "  Guard: {\n");
  ast_print_node_ident(data->expression, file, ident+4);

  print_spaces(ident, file);
  fprintf(file, "  }\n");

  print_spaces(ident, file);
  fprintf(file, "  Body: {\n");

  ast_print_node_ident(data->body, file, ident+4);
  fprintf(file, "\n");

  print_spaces(ident, file);
  fprintf(file, "  }\n");

  print_spaces(ident, file);
  fprintf(file, "}");
}

void ast_print_if_else_node(ASTNode *node, FILE *file, size_t ident) {
  AST_PRINT_DEBUG();
  assert(node->type == IF_ELSE_STATEMENT);
  IfElseStatementNodeData *data = (IfElseStatementNodeData *) node->data;

  print_spaces(ident, file);
  fprintf(file, "IfElse: {\n");

  print_spaces(ident, file);
  fprintf(file, "  Guard: {\n");
  ast_print_node_ident(data->expression, file, ident+4);

  print_spaces(ident, file);
  fprintf(file, "  }\n");

  print_spaces(ident, file);
  fprintf(file, "  If Body: {\n");
  ast_print_node_ident(data->if_body, file, ident+4);
  fprintf(file, "\n");

  print_spaces(ident, file);
  fprintf(file, "  }\n");

  print_spaces(ident, file);
  fprintf(file, "  Else Body: {\n");
  ast_print_node_ident(data->else_body, file, ident+4);
  fprintf(file, "\n");

  print_spaces(ident, file);
  fprintf(file, "  }\n");

  print_spaces(ident, file);
  fprintf(file, "}");
}

void ast_print_while_node(ASTNode *node, FILE *file, size_t ident) {
  AST_PRINT_DEBUG();
  assert(node->type == WHILE_STATEMENT);
  ConditionalStatementNodeData *data = (ConditionalStatementNodeData *) node->data;

  print_spaces(ident, file);
  fprintf(file, "While: {\n");

  print_spaces(ident, file);
  fprintf(file, "  Guard: {\n");
  ast_print_node_ident(data->expression, file, ident+4);

  print_spaces(ident, file);
  fprintf(file, "  }\n");

  print_spaces(ident, file);
  fprintf(file, "  Body: {\n");
  ast_print_node_ident(data->body, file, ident+4);
  fprintf(file, "\n");

  print_spaces(ident, file);
  fprintf(file, "  }\n");

  print_spaces(ident, file);
  fprintf(file, "}");
}


void ast_print_node_ident(ASTNode *node, FILE *file, size_t ident) {
  if (node->type == ASSIGNMENT) {
    ast_print_assignment_node(node, file, ident);
  } else if (node->type == EXPRESSION) {
    ast_print_expression_node(node, file, ident);
  } else if (node->type == NODES) {
    ast_print_node_list_node(node, file, ident);
  } else if (node->type == DECLARATION) {
    ast_print_declaration_node(node, file, ident);
  } else if (node->type == FUNC_DECLARATION) {
    ast_print_func_declaration_node(node, file, ident);
  } else if (node->type == IF_STATEMENT) {
    ast_print_if_node(node, file, ident);
  } else if (node->type == IF_ELSE_STATEMENT) {
    ast_print_if_else_node(node, file, ident);
  } else if (node->type == WHILE_STATEMENT) {
    ast_print_while_node(node, file, ident);
  } else if (node->type == PROGRAM) {
    ast_print_program_node(node, file, ident);
  } else if (node->type == ARR_DECLARATION) {
    ast_print_array_declaration_node(node, file, ident);
  } else if (node->type == STRUCT_DECLARATION) {
    ast_print_struct_declaration_node(node, file, ident);
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

void ast_dealloc_struct_declaration_node(ASTNode *node) {
  AST_PRINT_DEBUG();
  assert(node->type == STRUCT_DECLARATION);
  StructDeclarationNodeData *data = (StructDeclarationNodeData *) node->data;
  idf_dealloc_identifier(data->name);
  attrb_list_dealloc(data->attributes);
  free(data);
}

void ast_dealloc_array_declaration_node(ASTNode *node) {
  AST_PRINT_DEBUG();
  assert(node->type == ARR_DECLARATION);
  ArrayDeclarationNodeData *data = (ArrayDeclarationNodeData *) node->data;
  idf_dealloc_identifier(data->name);
  expr_list_dealloc(data->init_values);
  // ast_dealloc_node(data->init_values);
  free(data);
}

void ast_dealloc_program_node(ASTNode *node) {
  AST_PRINT_DEBUG();
  assert(node->type == PROGRAM);
  ProgramNodeData *data = (ProgramNodeData *) node->data;
  ast_dealloc_node(data->func_declarations);
  ast_dealloc_node(data->global_statements);
  ast_dealloc_node(data->struct_declarations);
  free(data);
}

void ast_dealloc_expression_node(ASTNode *node) {
  AST_PRINT_DEBUG();
  assert(node->type == EXPRESSION);
  ExpressionNodeData *data = (ExpressionNodeData *) node->data;
  expr_dealloc_expression(data->expression);
  free(data);
}

void ast_dealloc_assignment_node(ASTNode *node) {
  AST_PRINT_DEBUG();
  assert(node->type == ASSIGNMENT);
  AssignmentNodeData *data = (AssignmentNodeData *) node->data;
  ast_dealloc_node(data->expression);
  assgnbl_dealloc(data->assgnbl);
  //idf_dealloc_identifier(data->id);
  free(data);
}

void ast_dealloc_declaration_node(ASTNode *node) {
  AST_PRINT_DEBUG();
  assert(node->type == DECLARATION);
  DeclarationNodeData *data = (DeclarationNodeData *) node->data;
  if (data->has_init)
    expr_list_dealloc(data->init_values);
  // ast_dealloc_node(data->expression);
  // idf_dealloc_identifier(data->id);
  nt_bind_dealloc(data->nt_bind);
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

void ast_dealloc_conditional_statement_node(ASTNode *node) {
  AST_PRINT_DEBUG();
  assert(node->type == IF_STATEMENT || node->type == WHILE_STATEMENT);
  ConditionalStatementNodeData *data = (ConditionalStatementNodeData *) node->data;
  ast_dealloc_node(data->body);
  ast_dealloc_node(data->expression);
  free(data);
}

void ast_dealloc_if_else_node(ASTNode *node) {
  AST_PRINT_DEBUG();
  assert(node->type == IF_ELSE_STATEMENT);
  IfElseStatementNodeData *data = (IfElseStatementNodeData *) node->data;
  ast_dealloc_node(data->if_body);
  ast_dealloc_node(data->else_body);
  ast_dealloc_node(data->expression);
  free(data);
}

void ast_dealloc_node(ASTNode *root) {
  AST_PRINT_DEBUG();
  if (root == NULL)
    return;
  if (root->type == ASSIGNMENT) {
    ast_dealloc_assignment_node(root);
  } else if (root->type == EXPRESSION) {
    ast_dealloc_expression_node(root);
  } else if (root->type == NODES) {
    ast_dealloc_node_list_node(root);
  } else if (root->type == DECLARATION) {
    ast_dealloc_declaration_node(root);
  } else if (root->type == FUNC_DECLARATION) {
    ast_dealloc_func_declaration_node(root);
  } else if (root->type == IF_STATEMENT || root->type == WHILE_STATEMENT) {
    ast_dealloc_conditional_statement_node(root);
  } else if (root->type == IF_ELSE_STATEMENT) {
    ast_dealloc_if_else_node(root);
  } else if (root->type == PROGRAM) {
    ast_dealloc_program_node(root);
  } else if (root->type == ARR_DECLARATION) {
    ast_dealloc_array_declaration_node(root);
  } else if (root->type == STRUCT_DECLARATION) {
    ast_dealloc_struct_declaration_node(root);
  } else {
    AST_ERROR();
  }
  free(root);
}

#endif // end AST_HEADER