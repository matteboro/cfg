#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../lxr/lxr.h"
#include "../expr/expr.h"

/*

ASSGNM     :: [ IDENTIFIER_TOKEN - EQUAL_TOKEN - < EXPRESSION > - SEMICOLON_TOKEN ]

EXPRESSION :: [ < TERM > ( ( PLUS_TOKEN | MINUS_TOKEN ) - < TERM > )* ]

TERM       :: [ < FACTOR > ( ( ASTERISK_TOKEN | SLASH_TOKEN ) - < FACTOR > )* ]

FACTOR     :: [ IDENDIFIER_TOKEN ],
              [ INTEGER_TOKEN ],
              [ OPEN_PAREN_TOKEN - < EXPRESSION > - CLOSE_PAREN_TOKEN ]
*/

#ifdef PRSR_DEBUG
#define PRSR_DEBUG_PRINT() fprintf(stdout, "%s\n", __FUNCTION__);
#define PRSR_MATCH_DEBUG_PRINT() fprintf(stdout, "%s(", __FUNCTION__); lxr_print_token_type(token_type); fprintf(stdout, ")\n"); 
#define prsr_next_token() fprintf(stdout, "  "); lookhaed = lxr_next_token(lexer_ptr); lxr_print_token(lookhaed); fprintf(stdout, "\n");
#else
#define PRSR_DEBUG_PRINT() 
#define PRSR_MATCH_DEBUG_PRINT()
#define prsr_next_token() lookhaed = lxr_next_token(lexer_ptr);
#endif
#define PRSR_ERROR() fprintf(stdout, "error inside function: %s\n", __FUNCTION__); exit(1);

void  prsr_parse_assignment();
Expression *prsr_parse_expression(), *prsr_parse_term(), *prsr_parse_factor();

typedef enum {
  BYNARY_EXPRESSION,
  ASSIGNMENT,
} ASTNodeType;

typedef struct {} *ASTNodeData;

typedef struct {
  
} *AssignmentNodeData;

typedef struct {
  ASTNodeType type;
  ASTNodeData data;
} ASTNode;

static Token lookhaed;
Lexer *lexer_ptr;

void prsr_match(TokenType token_type) {
  PRSR_MATCH_DEBUG_PRINT();
  if (lookhaed.type == token_type) {
    prsr_next_token();
    return;
  }
  PRSR_ERROR();
}

Expression *prsr_parse_factor() {
  PRSR_DEBUG_PRINT();
  switch (lookhaed.type) {
  case IDENTIFIER_TOKEN: {
    Token token = lookhaed;
    prsr_match(IDENTIFIER_TOKEN);
    return expr_create_operand_expression_from_token(token);
  } 
  case INTEGER_TOKEN: {
    Token token = lookhaed;
    prsr_match(INTEGER_TOKEN);
    return expr_create_operand_expression_from_token(token);
  } 
  case OPEN_PAREN_TOKEN: {
    prsr_match(OPEN_PAREN_TOKEN);
    Expression *expression = prsr_parse_expression();
    prsr_match(CLOSE_PAREN_TOKEN);
    return expression;
  }
  default:
    PRSR_ERROR();
  }
}

Expression *prsr_parse_term() {
  PRSR_DEBUG_PRINT();
  Expression *root = prsr_parse_factor();
  while(lookhaed.type == ASTERISK_TOKEN || lookhaed.type == SLASH_TOKEN) {
    OperationType op = lookhaed.type == ASTERISK_TOKEN ? MULT_OPERATION : DIV_OPERATION;
    prsr_match(lookhaed.type);
    Expression *right = prsr_parse_factor();
    root = expr_create_binary_expression(root, op, right);
  }
  return root;
}

Expression *prsr_parse_expression() {
  PRSR_DEBUG_PRINT();
  Expression *root = prsr_parse_term();
  while(lookhaed.type == PLUS_TOKEN || lookhaed.type == MINUS_TOKEN) {
    OperationType op = lookhaed.type == PLUS_TOKEN ? SUM_OPERATION : SUB_OPERATION;
    prsr_match(lookhaed.type);
    Expression *right = prsr_parse_term();
    root = expr_create_binary_expression(root, op, right);
  }
  return root;
}

void prsr_parse_assignment() {
  PRSR_DEBUG_PRINT();
  prsr_match(IDENTIFIER_TOKEN);
  prsr_match(EQUAL_TOKEN);
  prsr_parse_expression();
  prsr_match(SEMICOLON_TOKEN);
}

bool prsr_parse(const char *data) {

  Lexer lexer = lxr_init(data);
  lexer_ptr = &lexer;

  prsr_next_token();

  prsr_parse_assignment();

  if (lookhaed.type == END_TOKEN) 
    return True;

  return False;
}

Expression *prsr_parse_only_expression(const char *data) {

  Lexer lexer = lxr_init(data);
  lexer_ptr = &lexer;

  prsr_next_token();

  Expression *expression = prsr_parse_expression();

  if (lookhaed.type == END_TOKEN) 
    return expression;

  expr_dealloc_expression(expression);
  return NULL;
}