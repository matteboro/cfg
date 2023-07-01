#include "../expr/expr.h"
#include <assert.h>

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

typedef struct ASTNodeList_s{
  struct ASTNodeList_s *next;
  ASTNode *node;
} ASTNodeList;

//// CREATE

ASTNodeList *ast_list_create_empty() {
  ASTNodeList *list = (ASTNodeList *)malloc(sizeof(ASTNodeList));
  list->next = NULL;
  list->node = NULL;
  return list;
  // return NULL;
}

ASTNodeList *ast_list_create(ASTNode *node) {
  ASTNodeList *list = (ASTNodeList *)malloc(sizeof(ASTNodeList));
  list->next = NULL;
  list->node = node;
  return list;
}

//// APPEND

void ast_list_append(ASTNodeList *list, ASTNode *node) {
  if (list->next == NULL) {
    if (list->node == NULL) {
      list->node = node;
    } else {
      list->next = ast_list_create(node);
    }
    return;
  }

  ASTNodeList *n = list->next;
  while(n->next != NULL)
    n = n->next;

  n->next = ast_list_create(node);
  return;
}

//// DEALLOC

void ast_list_dealloc(ASTNodeList *list) {
  if (list->next != NULL)
    ast_list_dealloc(list->next);
  
  if (list->node != NULL)
    ast_dealloc_node(list->node);
  
  free(list);
}

//// SIZE

size_t ast_list_size(ASTNodeList *list) {
  if (list->next != NULL)
    return ast_list_size(list->next) + 1;
  
  if (list->node != NULL)
    return 1;
  
  return 0;
}

//// GET AT

ASTNode *ast_list_get_at(ASTNodeList *list, size_t index) {
  if (list->next == NULL && list->node == NULL && index == 0)
    return NULL;
  assert(index < ast_list_size(list));
  ASTNodeList *n = list;
  for (size_t i=0; i<index; ++i)
    n = n->next;
  return n->node;
}

//// PRINT

void ast_list_print(ASTNodeList *list, FILE *file) {
  if_null_print(list, file);
  if (list->node != NULL)
    ast_print_node(list->node, file);
  
  if (list->next != NULL){
    fprintf(file, ", ");
    ast_list_print(list->next, file);
  }
}

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

typedef struct {
  Expression *expression;
} ExpressionNodeData;

typedef struct {
  Identifier *id;
  ASTNode *expression;
} AssignmentNodeData;

// TODO: we could have lists of nodes that accepts ASTNode of only certain type
typedef struct {
  ASTNodeList *nodes;
} NodeListData;

// CREATE

ASTNode *ast_create_node(ASTNodeType type, ASTNodeData *data) {
  ASTNode *node = (ASTNode *) malloc(sizeof(ASTNode));
  node->type = type;
  node->data = data;
  return node;
}

ASTNode *ast_create_assignment_node(Identifier *id, ASTNode *expression_node) {
  AssignmentNodeData *data = (AssignmentNodeData *) malloc(sizeof(AssignmentNodeData));
  data->id = id;
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

// PRINT

void ast_print_node_list_node(ASTNode *node, FILE *file, size_t ident);

void ast_print_expression_node(ASTNode *node, FILE *file, size_t ident) {
  ExpressionNodeData *data = (ExpressionNodeData *) node->data;
  print_spaces(ident, file);
  fprintf(file, "Expression: < ");
  expr_print_expression(data->expression, file);
  fprintf(file, " >\n");
}

void ast_print_assignment_node(ASTNode *node, FILE *file, size_t ident) {
  AssignmentNodeData *data = (AssignmentNodeData *) node->data;
  print_spaces(ident, file);
  fprintf(file, "Assignment: {\n");
  print_spaces(ident, file);
  fprintf(file, "  Idenfier: ");
  idf_print_identifier(data->id, file);
  fprintf(file, ",\n");
  ast_print_expression_node(data->expression, file, ident+2);
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
  } else {
    AST_ERROR();
  }
}

void ast_print_node(ASTNode *node, FILE *file) {
  ast_print_node_ident(node, file, 0);
}

void ast_print_node_list_node(ASTNode *node, FILE *file, size_t ident) {
  NodeListData *data = (NodeListData *) node->data;
  print_spaces(ident, file);
  fprintf(file, "Nodes: [\n");
  ast_list_print_ident(data->nodes, file, ident+2);
  fprintf(file, "\n");
  print_spaces(ident, file);
  fprintf(file, "]");
}

// DEALLOC

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
  idf_dealloc_identifier(data->id);
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
  if (root->type == ASSIGNMENT) {
    ast_dealloc_assignment_node(root);
  } else if (root->type == EXPRESSION) {
    ast_dealloc_expression_node(root);
  } else if (root->type == NODES) {
    ast_dealloc_node_list_node(root);
  } else {
    AST_ERROR();
  }
  free(root);
}