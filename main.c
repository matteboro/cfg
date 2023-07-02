#include "prsr/prsr.h"

int main () {

  Identifier *id = idf_create_identifier("boo");
  idf_print_identifier(id, stdout); fprintf(stdout, "\n");
  idf_dealloc_identifier(id);


  id = idf_create_identifier("boo");
  Operand *op = oprnd_create_operand(IDENTIFIER_OPERAND, id);
  oprnd_print_operand(op, stdout); fprintf(stdout, "\n");
  oprnd_dealloc_operand(op);

  Expression *expr = prsr_parse_expression_from_string("(boo * f) + bar(69, foo, 420 - 69 * baz)");
  expr_print_expression(expr, stdout); fprintf(stdout, "\n");
  expr_dealloc_expression(expr);

  return 0;
}

/*

func lor(t) {
  
}

func foo(bar, zed) {
  # comment
  var bar = 3 * lor(5);
}

Function: {
  Name: foo
  Params: [ bar, zed ]
  Code: [
    Assignment:  ...,
    ...
    Declaration: ...
  ]
}

DONE
now i want the declaration AST node:
DeclarationNodeData:
  Identifier id,
  bool has_init,
  ASTNode expression

DONE
StatementsNodeData
  ASTNodeList

DONE
FunctionDeclarationNodeData:
  char* name, ParamaterList params, ASTNode statements

DONE
ParseAssignemnt():
  IDENTIFIER::id
  '='
  ParseExpression()::expression

DONE
ParseVariableDeclaration():
  VAR_TOKEN
  IDENTIFIER::id
  if ('=')
    ParseExpression()::expression
  else
    NULL::expression

DONE
ParseStatement():
  if (VAR_TOKEN) 
    ParseVariableDeclaration()::statement
  else if ('{')
    ParseStatements()::statements
    ret statements
  else
    ParseAssignment()::statement
  ';'
  ret statement

DONE
ParseStatements(): (for the moment a list of variable declarations and assignments)
  '{'
  while (!'}')
    ParseStatement()::curr_statement
    statements.append(curr_statement)
  '}'

DONE
ParseFuncDeclParams():
  while (true)
    IDENTIFIER::param
    params.append(param)
    if (',')
      ','
    else
      break;
  ret params

DONE
ParseFunctionDeclaration():
  FUNC_TOKEN 
  IDENTIFIER::func_name 
  '(' 
  ParseFuncDeclParams()::params 
  ')' 
  ParseStatements()::body 

DONE
ConditionalStatementNodeData:
  Expression *guard,
  ASTNode *body

TODO: add else and elif parsing
ParseIf():
  IF_TOKEN
  ParseExpression()::expression
  ParseStatements()::body

DONE
ParseWhile():
  WHILE_TOKEN
  ParseExpression()::expression
  ParseStatements()::body

IfElseNodeData:
  ASTNode *guard,
  ASTNode *if_body,
  ASTNode *else_body,

NOTE: in case of chains of elif in the else else_body attribute we 
      will put another IfElseNode with the guard of the elif. We 
      do that until we find an else, and that's where we stop.

*/