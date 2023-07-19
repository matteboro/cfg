#pragma once

#include "assert.h"

#define LIST_ALLOC malloc
#define LIST_FREE free
#define if_null_print(ptr, file) if (ptr == NULL) { fprintf(file, "NULL"); return; }
#define FOR_EACH(type, name, list) for (type *name = list; (name != NULL && name->node != NULL); name = name->next)
#define FOR_EACH_ENUM(type, name, list, counter) \
  size_t counter = 0;                            \
  for (type *name = list;                        \
      (name != NULL && name->node != NULL);      \
      name = name->next, ++counter)


#define DEFAULT_LIST_IMPLEMENTATION(type_name)  \
typedef struct s_##type_name##List{             \
  struct s_##type_name##List *next;             \
  type_name *node;                              \
} type_name##List; 


#define DEFAULT_LIST_CREATE_EMPTY(prefix, type_name)                                \
type_name##List * prefix##_list_create_empty() {                                    \
  type_name##List *list = (type_name##List *)LIST_ALLOC(sizeof(type_name##List));   \
  list->next = NULL;                                                                \
  list->node = NULL;                                                                \
  return list;                                                                      \
}

#define DEFAULT_LIST_CREATE(prefix, type_name)                                      \
type_name##List * prefix##_list_create(type_name *node) {                           \
  type_name##List *list = (type_name##List *)LIST_ALLOC(sizeof(type_name##List));   \
  list->next = NULL;                                                                \
  list->node = node;                                                                \
  return list;                                                                      \
}

#define DEFAULT_LIST_APPEND(prefix, type_name)                      \
void prefix##_list_append(type_name##List *list, type_name *node) { \
  if (list->next == NULL) {                                         \
    if (list->node == NULL) {                                       \
      list->node = node;                                            \
    } else {                                                        \
      list->next = prefix##_list_create(node);                      \
    }                                                               \
    return;                                                         \
  }                                                                 \
  type_name##List *n = list->next;                                  \
  while(n->next != NULL)                                            \
    n = n->next;                                                    \
  n->next = prefix##_list_create(node);                             \
  return;                                                           \
}

#define DEFAULT_LIST_DEALLOC(prefix, type_name, dealloc_func)     \
void prefix##_list_dealloc(type_name##List *list) {               \
  if (list == NULL)                                               \
    return;                                                       \
  if (list->next != NULL)                                         \
    prefix##_list_dealloc(list->next);                            \
  if (list->node != NULL)                                         \
    dealloc_func(list->node);                                     \
  LIST_FREE(list);                                                \
}

#define DEFAULT_LIST_SIZE(prefix, type_name)          \
size_t prefix##_list_size(type_name##List *list) {    \
  if (list->next != NULL)                             \
    return prefix##_list_size(list->next) + 1;        \
  if (list->node != NULL)                             \
    return 1;                                         \
  return 0;                                           \
}

#define DEFAULT_LIST_GET_AT(prefix, type_name)                          \
type_name *prefix##_list_get_at(type_name##List *list, size_t index) {  \
  if (list->next == NULL && list->node == NULL && index == 0)           \
    return NULL;                                                        \
  assert(index < prefix##_list_size(list));                             \
  type_name##List *n = list;                                            \
  for (size_t i=0; i<index; ++i)                                        \
    n = n->next;                                                        \
  return n->node;                                                       \
}

#define DEFAULT_LIST_PRINT(prefix, type_name, print_func)       \
void prefix##_list_print(type_name##List *list, FILE *file) {   \
  if_null_print(list, file);                                    \
  if (list->node != NULL)                                       \
    print_func(list->node, file);                               \
  if (list->next != NULL){                                      \
    fprintf(file, ", ");                                        \
    prefix##_list_print(list->next, file);                      \
  }                                                             \
}

#define DEFAULT_LIST_POP_LAST(prefix, type_name)             \
type_name *prefix##_list_pop_last(type_name##List *list) {   \
  if (list == NULL)                                          \
    return NULL;                                             \
  if(list->next == NULL) {                                   \
    if (list->node == NULL) {                                \
      return NULL;                                           \
    } else {                                                 \
      type_name* ret_val = list->node;                       \
      list->node = NULL;                                     \
      return ret_val;                                        \
    }                                                        \
  }                                                          \
  if (list->next->next == NULL) {                            \
    type_name##List *last_node = list->next;                 \
    type_name *ret_val = last_node->node;                    \
    list->next = NULL;                                       \
    free(last_node);                                         \
    return ret_val;                                          \
  }                                                          \
  return prefix##_list_pop_last(list->next);                 \
}

#define DEFAULT_LIST_IS_EMPTY(prefix, type_name)      \
bool prefix##_list_is_empty(type_name##List *list) {  \
  return prefix##_list_size(list) == 0;               \
}

#define DEFAULT_LIST_GET_LAST(prefix, type_name)              \
type_name *prefix##_list_get_last(type_name##List *list) {    \
  if (prefix##_list_is_empty(list))                           \
    return NULL;                                              \
  size_t list_size = prefix##_list_size(list);                \
  return prefix##_list_get_at(list, list_size-1);             \
}
#define DEFAULT_LIST_GET_FIRST(prefix, type_name)             \
type_name *prefix##_list_get_first(type_name##List *list) {   \
  if (prefix##_list_is_empty(list))                           \
    return NULL;                                              \
  return prefix##_list_get_at(list, 0);                       \
}

#define DEFAULT_LIST_MERGED_FILE_INFO(prefix, type_name)                              \
FileInfo prefix##_list_merged_file_info(type_name##List *list) {                      \
  if (prefix##_list_is_empty(list))                                                   \
    return file_info_create_null();                                                   \
  FileInfo file_info = prefix##_list_get_first(list)->file_info;                      \
  file_info = file_info_merge(file_info, prefix##_list_get_last(list)->file_info);    \
  return file_info;                                                                   \
}    


#define LIST(prefix, type_name, dealloc_func, print_func) \
DEFAULT_LIST_IMPLEMENTATION(type_name)                    \
DEFAULT_LIST_CREATE_EMPTY(prefix, type_name)              \
DEFAULT_LIST_CREATE(prefix, type_name)                    \
DEFAULT_LIST_APPEND(prefix, type_name)                    \
DEFAULT_LIST_DEALLOC(prefix, type_name, dealloc_func)     \
DEFAULT_LIST_SIZE(prefix, type_name)                      \
DEFAULT_LIST_GET_AT(prefix, type_name)                    \
DEFAULT_LIST_PRINT(prefix, type_name, print_func)         \
DEFAULT_LIST_POP_LAST(prefix, type_name)                  \
DEFAULT_LIST_IS_EMPTY(prefix, type_name)                  \
DEFAULT_LIST_GET_FIRST(prefix, type_name)                 \
DEFAULT_LIST_GET_LAST(prefix, type_name)


void print_int(int *val, FILE *file) {
  fprintf(file, "%d", *val);
}

LIST(int, int, free, print_int)