#include "../expr/expr.h"

#define AST_ERROR() fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1);
#define print_spaces(n, file) {for (size_t spaces_counter=0; spaces_counter<n; ++spaces_counter) fprintf(file, " ");}

typedef enum {
  EXPRESSION,
  ASSIGNMENT,
} ASTNodeType;

typedef void ASTNodeData;

typedef struct {
  ASTNodeType type;
  ASTNodeData *data;
} ASTNode;

typedef struct {
  Expression *expression;
} ExpressionNodeData;

typedef struct {
  Identifier *id;
  ASTNode *expression;
} AssignmentNodeData;

ASTNode *ast_create_node(ASTNodeType type, ASTNodeData *data) {
  ASTNode *node = (ASTNode *) malloc(sizeof(ASTNode));
  node->type = type;
  node->data = data;
  return node;
}

ASTNode *ast_create_assignment_node(Token identifier_token, ASTNode *expression_node) {
  if (identifier_token.type != IDENTIFIER_TOKEN) {
    AST_ERROR();
  }
  AssignmentNodeData *data = (AssignmentNodeData *) malloc(sizeof(AssignmentNodeData));
  data->id = expr_create_identifier_from_token(identifier_token);
  data->expression = expression_node;
  return ast_create_node(ASSIGNMENT, data);
}

ASTNode *ast_create_expression_node(Expression *expression) {
  ExpressionNodeData *data = (ExpressionNodeData *) malloc(sizeof(ExpressionNodeData));
  data->expression = expression;
  return ast_create_node(EXPRESSION, data);
}

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
  expr_print_identifier(data->id, file);
  fprintf(file, ",\n");
  ast_print_expression_node(data->expression, file, ident+2);
  print_spaces(ident, file);
  fprintf(file, "}\n");
}


void ast_print_node_aux(ASTNode *node, FILE *file, size_t ident) {
  if (node->type == ASSIGNMENT) {
    ast_print_assignment_node(node, file, ident);
  } else if (node->type == EXPRESSION) {
    ast_print_expression_node(node, file, ident);
  } else {
    AST_ERROR();
  }
}

void ast_print_node(ASTNode *node, FILE *file) {
  ast_print_node_aux(node, file, 0);
}