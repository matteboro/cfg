#pragma once

#include <stdint.h>
#include "CFGOperand.h"


/*

///////////////////////////////////////////////////////////

Given:
  structs = [ 
    S:{
      size = 16, 
      attributes = [int a, int b] 
    }, 
    ...,
  ]

This:

  {
    s :: S;
    s.a = 10;
  }

Becomes:

  decl s :: S
  decl m.0 :: Offset;

  < m.0 > = RealtivePosition(S, a)
  < s, m.0 > = int(10)

  undecl m.0
  undecl s

That becomes:

  decl {s:1:S} :: S
  decl {m.0:2:Offset} :: Offset;

  < {m.0:2:Offset} > = RealtivePosition(S, a)
  < {s:1:S}, {m.0:2:Offset} > = int(10)

  undecl {m.0:2:Offset}
  undecl {s:1:S}

That becomes:

  decl {s:1:S} :: S

  < {s:1:S}, 0 > = int(10)

  undecl {s:1:S}

///////////////////////////////////////////////////////////

*/

/*

This: 
  {
    int arr[10] a;
    a[5] = 10;
  }

Becomes:

  decl a:{a:1:int[10]} :: int[10]            

    now a has been added to the VariableTable and it mapped to 
    a new MemoryTable entry that points to to a MemoryPiece
    holding space for 10 integer

*/