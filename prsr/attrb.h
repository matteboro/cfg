#pragma once

#include "../expr/nt_bind.h"

#define if_null_print(ptr, file) if (ptr == NULL) { fprintf(file, "NULL"); return; }

typedef struct {
  NameTypeBinding *nt_bind;
} Attribute;

Attribute *attrb_create(NameTypeBinding *nt_bind) {
  Attribute *attribute = (Attribute *) malloc(sizeof(Attribute));
  attribute->nt_bind = nt_bind;
  return attribute;
}

void attrb_print(Attribute *attribute, FILE *file) {
  nt_bind_print(attribute->nt_bind, file);
}

void attrb_dealloc(Attribute *attribute) {
  nt_bind_dealloc(attribute->nt_bind);
  free(attribute);
}

// ATTRIBUTE LIST

typedef struct AttributeList_s{
  struct AttributeList_s *next;
  Attribute *node;
} AttributeList;

//// CREATE EMPTY

AttributeList *attrb_list_create_empty() {
  AttributeList *list = (AttributeList *) malloc(sizeof(AttributeList));
  list->next = NULL;
  list->node = NULL;
  return list;
  // return NULL;
}

AttributeList *attrb_list_create(Attribute *attribute) {
  AttributeList *list = (AttributeList *) malloc(sizeof(AttributeList));
  list->next = NULL;
  list->node = attribute;
  return list;
}

//// APPEND

void attrb_list_append(AttributeList *list, Attribute *attribute) {
  if (list->next == NULL) {
    if (list->node == NULL) {
      list->node = attribute;
    } else {
      list->next = attrb_list_create(attribute);
    }
    return;
  }

  AttributeList *n = list->next;
  while(n->next != NULL)
    n = n->next;

  n->next = attrb_list_create(attribute);
  return;
}

//// DEALLOC

void attrb_list_dealloc(AttributeList *list) {
  if (list == NULL)
    return;
  if (list->next != NULL)
    attrb_list_dealloc(list->next);
  
  if (list->node != NULL)
    attrb_dealloc(list->node);
  
  free(list);
}

//// SIZE

size_t attrb_list_size(AttributeList *list) {
  if (list->next != NULL)
    return attrb_list_size(list->next) + 1;
  
  if (list->node != NULL)
    return 1;
  
  return 0;
}

//// GET AT

Attribute *attrb_list_get_at(AttributeList *list, size_t index) {
  if (list->next == NULL && list->node == NULL && index == 0)
    return NULL;
  assert(index < attrb_list_size(list));
  AttributeList *n = list;
  for (size_t i=0; i<index; ++i)
    n = n->next;
  return n->node;
}

//// PRINT

void attrb_list_print(AttributeList *list, FILE *file) {
  if_null_print(list, file);
  if (list->node != NULL)
    attrb_print(list->node, file);
  
  if (list->next != NULL){
    fprintf(file, ", ");
    attrb_list_print(list->next, file);
  }
}

// END ATTRIBUTE LIST
