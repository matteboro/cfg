#pragma once

#include <stdint.h>
#include "../../expr/idf.h"
#include "../../expr/type.h"

typedef uint64_t VariableIndex;
typedef struct {
  VariableIndex var_idx;
  Identifier *name;
  Type *type;
} Variable;