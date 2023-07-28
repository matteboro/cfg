#pragma once

#include "CFGStatement.h"

#define FOR_EACH_CFGSTMNT(name, list)    \
  for (CFGStatementList *name = list; \
  !CFGStatementList_Empty(name);      \
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
  else if (CFGStatementList_Empty(list->next)) {
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

CFGStatementList *CFGStatementList_Concat(CFGStatementList *dest, CFGStatementList *appended_list) {
  assert(dest != NULL);
  assert(appended_list != NULL);
  assert(dest != appended_list);

  if (CFGStatementList_Empty(appended_list))
    return dest;
  if (CFGStatementList_Empty(dest))
    return appended_list;

  CFGStatementList *last = CFGStatementList_GetLast(dest);
  last->next = appended_list;
  return dest;
}

bool CFGStatementList_Empty(CFGStatementList *list) {
  return list == NULL || (list->next == NULL && list->statement == NULL); 
}