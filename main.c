#include "prsr/prsr.h"


bool int_same_value(int *f1, int *f2) {
  return (*f1) == (*f2);
}

int main () {

  int *val;

  intList *list = int_list_create_empty();

  for(int i=0; i<10; ++i) {
    val = malloc(sizeof(int));
    *val = i;
    int_list_append(list, val);
  }

  // val = malloc(sizeof(int));
  // *val = 0;
  // int_list_append(list, val);

  // FOR_EACH(intList, it1, list) {
  //   FOR_EACH(intList, it2, it1->next) {
  //     fprintf(stdout, "[ "); int_list_print(it2, stdout); fprintf(stdout, " ]\n");
  //   }
  // }

  fprintf(stdout, "predicate is %s\n", int_list_check_binary_predicate(list, int_same_value) ? "true" : "false");

  int_list_dealloc(list);

  return 0;
}

/*

Function: {
  Name: foo,
  Params: [ bar, zed ],
  Code: [
    Assignment: {
      Variable: id.baz,
      Expression: (baz * 2) - 69
    },
    ...
    Declaration: {
      Variable: id.foo,
      Expression: 7 * id.bar
    }
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

TODO: add else and elif parsing --> DONE
ParseIf(first):
  if (first)
    IF_TOKEN
  else
    ELIF_TOKEN
  ParseExpression()::expression
  ParseStatements()::if_body
  if (ELSE_TOKEN)
    ParseStatements()::else_body
    ret ElseIfNode(expression, if_body, else_body)
  else if (ELIF_TOKEN)
    ParseIf(False)::else_body
    ret ElseIfNode(expression, if_body, else_body)
  else
    ret IfNode(expression, if_body)

DONE
ParseWhile():
  WHILE_TOKEN
  ParseExpression()::expression
  ParseStatements()::body

DONE
IfElseNodeData:
  ASTNode *guard,
  ASTNode *if_body,
  ASTNode *else_body,

DONE
NOTE: in case of chains of elif in the else else_body attribute we 
      will put another IfElseNode with the guard of the elif. We 
      do that until we find an else, and that's where we stop.

Next step is to parse an entire program. In my language a program is made
of a list of function declaration(definition), that are visible across the 
all program, and a series of global statements. There is a special function 
called main() that is called at the end of the run of all the global 
statements. The global statements are called in the order they are written.
The variable declared in the global space are visible inside every function.

DONE
ProgramNodeData:
  ASTNode *func_declarations;
  ASTNode *global_statements;

DONE
ParseProgram():
  while (!END_TOKEN)
    if (FUNC_TOKEN)
      functions.append(ParseFunctionDeclaration())
    else 
      statements.append(ParseStatement())
  ret ProgramNode(functions, statements)


STRING AND ARRAY TYPES

  Now i want to add two new data types: array and string:
    - for string i have to be able to parse string literals (meaning somethign between "");
    - for arrays what i want to do is having instead of var keyword the arr keyword and to 
      declare an array of size 10 you have to write arr[10] my_array;

  Having that i would have new types of operands and params (STRING and ARRAY_DEREF), and i 
  would have to distinguish between variables declaration and array declaration. Also in an
  assignment the left hand side could be a variable or a array dereference. In the case of
  a variable is sufficient to store the identifier, in the case of array we will need to 
  store both the identifer and the integer index of the dereference. So now the assignment 
  store a left value (variable identifier or array dereference) and right value (meaning
  an expression).

  Notes on the implementation of string data type:

    I have added the possibility to have string literals as values of variables or operands. 
    The possibility to accept string as values in certain expression will resolve in a 
    later stage of compilation. There will be operands and operations on strings [1]. The matter
    of having the string param type is still not resolved. The idea is that we only need two
    type of parameters, the ones used in function declarations (just identifiers, for the moment),
    and the ones used in a function call, which are all expressions. So it is a consequence to
    think that neither the integer nor the string parameter type are necessary.  

    [1]: such as concat, len, prefix, etc ...

  Parsing array declaration:

    DONE
    ParseArrayDeclaration():
      ARR_TOKEN
      OPEN_SQUARE_TOKEN
      INTEGER_TOKEN::size
      CLOSE_SQUARE_TOKEN
      IDENTIFIER::name

    DONE
    ArrayDeclarationNodeData:
      Identifier name;
      Int size;
      ASTNodeList init_values;  // to upgarde to ExpressionList

    The next step is to add the possibility to initialize an array in this syntax:
      arr[5] bar = [1, 2, "hello", 3, "foo"];
    To do this i would use a list (or maybe an array because the size is known) of
    expressions that would initialize the values in sequence.

    DONE
    Now i have to accept array dereference as an operand in an expression. The operand
    has to store the identifier of the array and the index, this has to be an 
    expression (that later will be checked to be an integer expression).

DONE
Should i create an AssignableElement data structure to hold data of elements that
can be put on the left hand side of an assignment (so a variable or an array
dereference)? I think so. If I do so the assignment statement has to change the data.
The Identifier attribute need to go and in its place an AssignableElement will
be placed. For the moment just two types of AssignableElement will be able: 
  - VAR,
  - ARRAY_DEREF.

Pointers:

  Syntax: 
    ptr var k1;              declaration
    ptr var k2 = ptr(f);     declaration with initialization
    var foo = val(1k);       pointer dereference

Struct
  Syntax
    arr[20] f;
    data point { 
      var t, 
      var p, 
      arr[20] f 
    };
    var p = point{ 20, 4, f };

NOTE: i should give types to the parameters accepted by a function. For example:
  foo(var t, var p, arr[20] f) { ... }

I now want to change the type system in my language. I want types. I'll get rid of
var keyword and I will have 2 basics type: int, string, and 2 ways to declare
new types: data { ... } and <type> arr[size].
Steps:
  1) add to the lexer the appropriate keywords;
  2) modify the AST to have types in the declarations;
  3) modify the parser to generate the appropriate AST;

DONE
TODO: i have to change how funccall works, i want them to have a list of expression,
not a list of parameters. 


I want:
  variable declarations, function definition parameters and struct attributes to hold 
  an instance of an object of the form < name, type >, where name is the name of that 
  variable/attribute/parameter and the type is the type of that entity. Thid object 
  will be called NameTypeBinding { Identifier *name, Type *type }.

DONE
Now we have to parse types as a separate method:
  grammar for declaration: 
    DECLARATION: TYPE "::" IDENTIFIER ( '=' EXPRESSION ) ';'
  ex:
    int arr[5] :: int_array = [1, 2, 3, 4, 5];

  DONE
  # let say we know we are inside a declaration of some sort(var, param or attribute)
  ParseType(start_type):
    ParseArrayType()::array_type
    if (array_type)
      # add start_type to end of array_type chain
    else
      ret Type(INT_TYPE)

  DONE
  ParseDeclaration():
    ParseType()::type
    "::"
    IDENTIFIER::name
    if ('=')
      ParseExpression()::init_value
    ret DeclarationNode(NameTypeBinding(name, type), init_value)

DONE
I've implemented the parsing of types in variable declaration, now i have to do it
also for function declaration params. To do that before i have to add NameTypeBinding 
somewhere inside Paramater. 

NOTE: for the moment I do not implement multiple dimension arrays, not because it is
difficoult to parse the type, that's trivial, the hard part i think would be to create
and manage the data structure to hold the possible initial values. 

TODO: another thing we should do is to permit to have function parameters to be array
      of undeclared sizes. Such as: func foo(int arr[?], int size) { ... }

Struct:

  The moment is arrived to parse user defined data structure. This will change the things 
  a bit. For example we will need struct dereferencing. We have to able to, given a struct:
    data my_data { int :: x, string :: s }
  and an object: 
    my_data :: o; 
  to dereference the single attributes of the o object in this manner:
    o.x = 10;
    o.s = "Hello, World";

  Syntax:
    data data_name {
      # battributes: type :: name,
      int :: x,
      string arr[5] :: ns
    }

  DONE
  Attributes: I think that for future expansions and clarity i should create the Attribute
    data structure, and the AttributeList. An Attribute, for the moment, just hold a 
    NameTypeBinding. A struct declaration is an ASTNode that holds a list of attributes and
    the name of the structure.

  Struct deref: to have dereferencing of structure attributes I first need to parse the point.
    Then i have to create some sort of StructDeref data structure that holds an identifier and
    one of these three things:
      - Identifier,
      - StructDeref,
      - ArrayDeref;

    So I maybe need an object called ObjectDeref that holds this possible chain of derefrences,
    and that is used in multiple places, such as in an Assignable, in an expression-s operand and
    possibly somewhere else.

  DONE
  Let's think about object dereferencing. We could have something like:
    strc_attrb1.arr_attrb1[5].arr_attrb2[3].int_attrb = 5;
  and i want a data structure of this kind:
    id::strc_attrb1;
    | 
    |---> id::arr_attrb1, index::5;
          |
          |---> id::arr_attrb2, index::3;
                |
                |---> id::int_attrb;
                      |
                      |---> NULL;

  On this structure we later could run some checks, for example internal elements of this list
  could only be array or struct types, while the end element can only be one of the basics 
  types (for the moment int or string).
  So there should only be two types of ObjectDereference:
    StructOrBasicTypeObjectDeref { Identfier::name }
    ArrayTypeObjectDeref { Identifier::name, int index }

  I then decided to create this structure as a list of ObjectDeref. Now I have to create an 
  expression operand for this type of structure.

DON
Last thing I want to add before we pass to the new step of checking for errors in code are the
string operations:
  - |string|            : string length,           string --> int
  - string1 <> string1  : concatenation,  string X string --> string
NOTE: I have ended up doing another syntax: < str > for length and str2 | str1 for concatanation.

Then the next steps, as seen from very far above, are:
  code checking --[ and then ]--> CFG generation;

Before I jump into code analysis to check for correction I want to change a bit how some AST node
are constructed. At the moment I have the same type of nodes for blocks statement, a statement 
made of a list of statements, and for the global function declarations. These nodes are both 
ASTNodeList type and I want to distinguish between them. 

Checks:
  - struct declaration:
    - check if name of type is available;
    - check if types of attributes exist;
    - check that attributes do not have same names;
    - check to not have circular encapsulation;

  - functions declarations:
    - check if name is available;
    - check if types of parameter exist;

  - declaration:
    - check if name is available;
    - check if type exists;
    - check if initialization expression is of correct type;

  - assignment:
    - check if assignable exists (object dereference);
    - check if expression is of correct type;

  - expression:
    - we need a way to find the return type of an expression;

  - function call:
    - check if the type of the expression match the type of the parameter;
    
  - object dereference:
    - struct:
      - check if attribute exists;
    - array:
      - check if index expression is of type int;
  
  What i probably need is a map that connects name to the right entity. Kinda like the 
  NameTypeBinding object. But maybe I can work on the data structure already created for the
  AST. That would be faster, but maybe less elegant. We will see. The first thing i want to try 
  is the struct declarations check. 

  ALMOST DONE
  Do we need a StructDeclaration object?
    What we currently have is: a program node whose data points to a ASTNode of type NODES, whose 
    data contains a list of ASTNode of type STRUCT_DECL, whose data contains an Identifier (the 
    name of the struct) and a list of Attributes. 
    What would be more convenient is to have a StructDeclaration object that contains an Identifier 
    (the name of the struct) and a list of Attributes. And then the program node data contains a 
    list of StructDeclaration. 

    DONE
    Next step is to adjust the struct declaration checker (it should simplify the things).
  FINISHED

  I think we should do the same for function declarations. The program node data should contain a 
  list of FunctionDeclaration, which is a data structure that contains:
    - a Type object (the returned type);
    - a list of Parameter;
    - a list of Statements (this could be a ASTNode of type NODES containing statement nodes);
        |
        |---> this give me another idea: probably Statement should be an object of himself; it could
              have its own sub-types (Assignment, Declaration, If, While for the moment);

DONE
A couple of things to do:
  - re-implement the Parameter object, there should not be different type of parameter. It should
    contain a NameTypeBinding (for the moment just that, maybe in the future a default type or 
    something else);
  - clean the AST nodes type, maybe it will only remain the Program node. In that case remove ast
    and create a Program object, and the parser return a Program;
  - we need to implement the possibility of having function call as statement; as for the moment
    those are parsed only has operands in expressions;
  - add the parsing of return statement (pretty straight-forward);

How do we know the type returned by an expression?

  Let's think about how we could calculate the type returned by an expression. My idea is to have
  a function with this signature:
    bool expr_returned_type(Expression *expr, Type **ret_type);
  The value returned by this function is True if the expression is consistent inside and it does
  return a value of a valid type. If the value returned is True then ret_type points to a pointer
  that points to the returned type.
  How to know the type of an operand? 
    - if it is a literal it is easy: either int or string; 
    - if it is an expression, we recursively calculate the type of the sub-expression; 
    - if it is a function call, we check the return type of the function;
    - if it is a dereference object we need to do some more work:
      - We probably need some sort of table that holds the knowledge about all declared object (so all)
        the NameTypeBinding valid to that point) and a table holding information about all the declared 
        types (for the latter the part regarding declared structs is already inside the program node of
        the outputted AST). I will think about it in the future...

We could start simple and check for all variable declarations and check if the type actually exists. To do
we need a way to query the struct declarations for the existence of a struct with a specific name. But 
actually the same task has to be done in other checkers, for example in function return type, parameter's
type or struct's attribute's type.

Statement checks:
  - variable declaration checks:
    - type exists,
    - name is avalilable,
    - init values matches type,
  - function call checks:
    - function exists,
    - number of params is correct,
    - types of params are correct,
  - assignment checks:
    - variable exists,
    - type of assigned value is correct,
  - return checks:
    - type of expression match return type of function,

To do all if these we need to keep a list of all the variable alive. A Variable is just a NameTypeBinding.
The AliveVariableTable is a list of Variable and a way to decide wich variables have to be forgotten a the
next end of a block statement. This system is a sort of stack of set of variables. Next exit of a blok 
statement we remove the higher set in the stack. We could do it with a list of list of variable. When we
enter a block we create a new list of variable and append it to the list of list of variables. The new
variables declared are always appended to the last element of the list of list of variables. When we exit a 
block statement we remove the last element of the list of list of variables.

To check if a name is available we scan all the elements of the list of list of variables to see if there
is already a variable with that name.

Once I have the correct function for extracting the type of a object dereference list I should be able 
easily to have the function that exctract the returned type of an expression. And then if I have that
most of the checking inside the statements are done.

  we have avlb_vars, structs and obj_derefs made like this:

    obj_start :: obj1 :: obj2 :: obj3 :: obj_final

  There are two special ObjectDereference: obj_start and obj_final. 
    - obj_start.name has to be searched inside the available variables;
  the the situation is usually like this:

        ... :: obj_(i-1) :: obj_i :: obj_(i+1) :: ...
                              
  If I just get on obj_i I know the struct type of obj_(i-1) and it is called prev_struct,

    find obj_i.name in prev_struct->attributes, if not error
    if I am not on last element {
      type <-- extract ultimate type
      check if type is not basic, if it is error,
      see if array deref match, if not error,
      prev_struct <-- structs.get_from_type(type)
    }
    else { "" I am on last element ""
      get ret_type ""  ""
      ult_type <-- extract ultimate type
      check if type is basic, if it is not error,
      return ret_type
    }

IDEA: 
  during the checking of an object dereference list I could enrich the quantity of information of each 
  dereference, I indeed could add the type of the dereferenced object, since I am checking for its existence 
  and correctness.

Pointers:

  Before I start implementing the CFG and the interpreter I want to implement pointers; the syntax I want to
  use is the following:
    - declaration:
        int ptr :: p;
    - declaration with initialization of array:
        int ptr :: p = create int[n];
    - declaration with initialization of single element:
        int ptr :: p = create int;
    - assignment:
        p = create int[n+1];
    - single element assignment
        p = create int;
    - array deref:
        p[index] = ...;
    - single element deref: 
        [p] = ...;
  
  When a pointer is declared and not initialize its value is Null. When a pointer is assigned, if its value is
  not Null, the previous value is automtically deallocated (i.e. there is no statement to dealloc the memory
  regione pointed to by a pointer).
  Can create become an expression?
    It would make the implementation of pointers easier and also more elegant. First idea is to create a special type
    of expression, the CreateExpression. The data contained in a create expression is: the type of the data 
    created (has to match the ultimate type of the variable being assigned), the size of the array creation (1 
    in case of single element).

  How to parse a create expression?
    parse_general_expression():
      if (CREATE_TOKEN)
        parse_create_expression()
      else
        parse_expression()
  
  Now I can parse ptr declaration and create expression. Next goal is to implement the the new type of dereference
  introduced with pointers, the single element pointer dereference.

  Pointers can be now handles by the parsing phase fo the compiler. Now we have to work on the checking phase.
  Where: 
    - ultimate type should consider pointers;
    - object deref should consider pointers deref (single element ot array)
    

CFG:
  We have to start to think about how to implement the CFG, in order first to be able to interpret 
  it in a concrete manner, and secondly in an abstract way. 

  object --> void *
  int    --> int *
  string --> char *
  struct --> object *
  arr    --> object *

  What is an Object: 
    The Object is a thing that contains some value, it could be:
      - a basic object: such as a int or a string;
      - a struct object: a collection of Object;
      - an array object: a list of Objects, of the same kind; 
    Each object has an ID (a unique reference), that could be seen as a name.

============================================================================================

TODO list:
  [x] array initialization;
  [x] AssignableElement;
  [x] left unary operations (such as - and !);
  [x] change funccall to have list of exprs;
  [x] add types;
   |  [x] Type;
   |  [x] NameTypeBinding;
   |  [x] add NameTypeBinding to func declarations params, var declaration;
  [x] parse data struct;
   |  [x] Attribute and AttributeList;
   |  [x] ASTStructDeclarationNode;
   |  [x] parsing of structs;
  [x] ObjectDereference (and move it where it is used);
  [x] string operations;
  [x] re-implement Parameter;
  [x] clean AST of garbage (maybe remove it, leave only Program);
  [x] function call as statement;
  [x] add return statement to parsing phase;
  [/] distinguish the ASTNodeList type of node between all the cases;
  [ ] pointers;
  [ ] undefined arr sizes in function declaration parameters;

============================================================================================
*/