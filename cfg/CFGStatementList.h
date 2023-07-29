#pragma once

#include "CFGStatement.h"

#define FOR_EACH_CFGSTMNT(name, list)    \
  for (CFGStatementList *name = list;    \
  !CFGStatementList_Empty(name);         \
  name = name->next)

struct CFGStatementList;
typedef struct CFGStatementList CFGStatementList;

CFGStatementList *CFGStatementList_CreateEmpty();
CFGStatementList *CFGStatementList_Create(CFGStatement *statement);

void CFGStatementList_Destroy(CFGStatementList *list);

void CFGStatementList_Append(CFGStatementList *list, CFGStatement *statement);
CFGStatementList *CFGStatementList_Concat(CFGStatementList *list1, CFGStatementList *list2);

bool CFGStatementList_Empty(CFGStatementList *list);

CFGStatementList *CFGStatementList_GetAt(CFGStatementList *list, size_t index);
CFGStatementList *CFGStatementList_GetLast(CFGStatementList *list);

void CFGStatementList_Print(CFGStatementList *list, FILE *file);

// IMPLEMENTATION

struct CFGStatementList {
  CFGStatementList *next;
  CFGStatement *statement;
};

CFGStatementList *CFGStatementList_CreateEmpty() {
  CFGStatementList *list = (CFGStatementList *) malloc(sizeof(CFGStatementList));
  list->next = NULL;
  list->statement = NULL;
  return list;
}

CFGStatementList *CFGStatementList_Create(CFGStatement *statement) {
  CFGStatementList *list = (CFGStatementList *) malloc(sizeof(CFGStatementList));
  list->next = NULL;
  list->statement = statement;
  return list;
}

void CFGStatementList_Destroy(CFGStatementList *list) {
  // fprintf(stdout, "starting CFGStatementList destroy\n");
  if (list == NULL)
    return;
  if (CFGStatementList_Empty(list)) {
    free(list);
  }
  CFGStatementList_Destroy(list->next);
  CFGStatement_Destroy(list->statement);
  free(list);
}

void __CFGStatementList_ShallowDestroy(CFGStatementList *list) {
  assert(list != NULL);
  if (CFGStatementList_Empty(list)) {
    free(list);
  }
  __CFGStatementList_ShallowDestroy(list->next);
  free(list);
}

CFGStatementList *CFGStatementList_GetAt(CFGStatementList *list, size_t index) {
  TODO();
  (void) list;
  (void) index;
  return NULL;
}

CFGStatementList *CFGStatementList_GetLast(CFGStatementList *list) {
  assert(list != NULL);
  if (CFGStatementList_Empty(list)) {
    return list;
  } 
  else if (list->next == NULL) {
    return list;
  }
  return CFGStatementList_GetLast(list->next);
}

void CFGStatementList_Append(CFGStatementList *list, CFGStatement *statement) {
  assert(list != NULL);
  assert(statement != NULL);

  if (CFGStatementList_Empty(list)) {
    list->statement = statement;
    return;
  }

  CFGStatementList *last = CFGStatementList_GetLast(list);
  last->next = CFGStatementList_Create(statement);
}


CFGStatementList *CFGStatementList_Concat(CFGStatementList *l1, CFGStatementList *l2) {
  assert(l1 != NULL);
  assert(l2 != NULL);
  assert(l1 != l2);

  if (CFGStatementList_Empty(l1)) {
    // fprintf(stdout, "l1 is empty\n");
    // CFGStatementList_Destroy(l1);
    return l2;
  }
  if (CFGStatementList_Empty(l2)) {
    // fprintf(stdout, "l2 is empty\n");
    // CFGStatementList_Destroy(l2);
    return l1;
  }

  CFGStatementList *last = CFGStatementList_GetLast(l1);
  last->next = l2;
  return l1;
}

bool CFGStatementList_Empty(CFGStatementList *list) {
  return list == NULL || (list->next == NULL && list->statement == NULL); 
}

void CFGStatementList_Print(CFGStatementList *list, FILE *file) {
  if (list == NULL)
    fprintf(file, "NULL\n\n");
  if (CFGStatementList_Empty(list))
    fprintf(file, "Empty list\n\n");
    
  FOR_EACH_CFGSTMNT(stmnt_it, list) {
    CFGStatement_Print(stmnt_it->statement, file);
    fprintf(file, "\n");
  }
  fprintf(file, "\n");
}