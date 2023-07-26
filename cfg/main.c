#include "CFG.h"

int main() {
  fprintf(stdout, "Hello, World!\n");
  return 0;
}

/*


CFGStatement could be:

  decl var;                     --> {Variable}

  var := cfg_xpression;         --> {Variable, {CFGOperand, Operation, CFGOperand}}

  var <= cfg_call;              --> {Variable, {CFGCall}}

  undecl var;                   --> {Variable}

This:

  int :: v;
  int :: t = 0;
  v = 10 * t;

Should become:

  decl Variable:{
        .name = "v", 
        .var_idx = 1, 
        .type = int};

  decl Variable:{
        .name = "t", 
        .var_idx = 2, 
        .type = int};

  Variable:{"t", 2, int} := CFGExpression:{
                              .type = OPERAND_CFGEXPR, 
                              .data = CFGOperand:{
                                          .type = LITERAL, 
                                          .data = {INT_LITERAL, 0}}};

  Variable:{"v", 1, int} := CFGExpression:{
                              .type = BINOP_CFGEXPR,
                              .data = CFGBinaryExpression:{
                                .left = CFGOperand:{LITERAL, {INT_LITERAL, 10}}
                                .op = SUM,
                                .right = CFGOPERAND:{VARIABLE, {Variable:{"t", 2, int}}}}};

It is straightforward when the type inside a variable is a basic type such as int, when insted the type
is constructed (with the constructs array or struct) we need to tell the memory manager one more 
information: if we see the constructed type as a list of simpler types we do not only want to tell
the memory manager what constructed object we want, but also what simpler object inside the constructed
one we want. So if we have a constructed object CO made of i objects, each one with a unique name;

  CO = [ "a" : SO_1, "b" : SO_2, ..., "n_label" : SO_i ]

    the unique names can be mapped to imcreasing indeces starting from 0:

  CO = [ 0 : SO_1, 1 : SO_2, ..., n : SO_i ]

Now lets say in our source code we created an object co :: CO, we could possibly want to access a sub-object
of co with a classical dot dereference, and assign something to it, just like it:

  co :: CO;
  co.a = [ Expression ];
  ...

Well for the occasions we have to introduce a memory access operator with this syntax and semantics:

syntax:
  access < Variable v, ObjectIndex idx >

semantics:
  retrieve the Object o inside the Variable v, assert that o is of a constructed type (array, struct), 
  then retrieve the object with index idx from the list of Object that compose o;

But actually we want something more like:

  access < Variable v_cons, Variable v_idx >

because what we usally have is an access controlled by the value of an expression. The only thing we want
for sure is that the Object retrieved by the Variable v_idx is an Integer. But because of the type checking
on the AST and th etransformations we will apply this would be reassured. The only thing we have to do is 
during the acces to the v_idx element that this value is between the boundaries of the constructed object.

Next step is to realize that what we want is actually:

  access < Variable v_cons, CFGOperand op_idx >

so that we can put a literal in the index offset if we now the value at compile-time.
The value returned by this command is an Object pointer. It would be easier to assign it directly to an 
intermediate variable at creation, so let's say we have:

This

  int :: i = 0;
  int arr[3] :: arr_3 = [1, 2, 3];
  i = arr_3[1] * 5;

Becomes:

  -
  - decl {i:1:int} :: int;
  -
  - {i:1} := 0;
  -
  - decl {arr_3:2:int arr[3]}
  -
  - access < {arr_3:2}, 0 > := 1;
  - access < {arr_3:2}, 1 > := 2;
  - access < {arr_3:2}, 2 > := 3;
  -
  - decl {i.expr.1:3:int}
  - {i.expr.1:3:int} := acces < {arr_3:2}, 1 >;
  -
  - {i:1} := {i.expr.1:3:int} * 5;
  - 
  - undecl {i.expr.1:3};
  -

Technically we have two types of acces: the basic variable acces and the constructed variable acces:

access < Variable v >  ---> 
  retrieve the Object pointer at the MemTableIndex mapped to the Variable v;

access < Variable v_cons, CFGOperand op_idx > --->
  retrieve the Object pointer at the op_idx_ith poistion of the list of objects that make up the Object
  linked with the MemTableIndex mapped to the Variable v_cons

The important think is that on the first parameter we provide a MemTableIdx. So this would work also 
for pointer, since the value of a pointer is a MemTableIdx value. For semplicity we could have a new
type of acces for pointers:

  acces ptr < Variable v_ptr >

that knows not to take the MemTableIdx mapped with the Variable v_ptr, but the value inside the Object
mapped to v_ptr. So it has to do more steps:
  - get the MemTableIdx mapped to v_ptr, i.e. v_ptr_idx;
  - get the Object pointer mapped with v_ptr_idx, i.e. Object *ptr_obj;
  - get the MemTableIdx value inside ptr_obj, i.e. pointed_idx;
  - get the Opject pointer mapped to pointed_idx;

While usually with a normal access "acces < Variable v >" we have:
  - get the MemTableIdx mapped to v, i.e. v_idx;
  - get the Object pointer mapped with v_idx;

Or with a indexed access "access < Variable v_cons, CFGOperand op_idx >":
  - get the MemTableIdx mapped to v_cons, i.e. v_cons_idx;
  - get the Object pointer mapped with v_cons_idx, i.e. Object *cons_obj;
  - evaluate the integer value of op_idx (it could be Literal or Variable, for the second case the process
    is similar to the basic one operando access), that gives op_idx_int;
  - get the op_idx_int_ith element from the list of objects that constitute cons_obj;

And in the end we could have a indexed pointer acces that take two arguments and functions just like having
in sequence the acces ptr and indexed acces:

  acces ptr < Variable v_ptr, CFGOperand op_idx >

In synthesis we have four types of access:

  access < Variable v >
  access < Variable v_cons, CFGOperand op_idx > 
  acces ptr < Variable v_ptr >
  acces ptr < Variable v_ptr, CFGOperand op_idx >

A thing to realize and to think about is that any of these types of acces could be on the left or right side
of an assignment, cause they all return an Object pointer. 

*/