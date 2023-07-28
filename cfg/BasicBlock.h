#pragma once

#include "CFGStatementList.h"

// BASIC BLOCK

typedef struct {
  CFGStatementList *statements;
  size_t num_statements;
} BasicBlock;

