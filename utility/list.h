#ifndef LIST_IMPLEMENTATION
#define LIST_IMPLEMENTATION

#include "assert.h"

#define LIST_ALLOC malloc
#define LIST_FREE free
#define if_null_print(ptr, file) if (ptr == NULL) { fprintf(file, "NULL"); return; }
#define FOR_EACH(type, name, list) for (type *name = list; (name != NULL && name->node != NULL); name = name->next)

#define DEFAULT_LIST_IMPLEMENTATION(type_name)  \
typedef struct s_##type_name##List{             \
  struct s_##type_name##List *next;             \
  type_name *node;                              \
} type_name##List; 


#define DEFAULT_LIST_CREATE_EMPTY(prefix, type_name)                                \
type_name##List * prefix##_list_create_empty() {                                   \
  type_name##List *list = (type_name##List *)LIST_ALLOC(sizeof(type_name##List));   \
  list->next = NULL;                                                                \
  list->node = NULL;                                                                \
  return list;                                                                      \
}

#define DEFAULT_LIST_CREATE(prefix, type_name)                                      \
type_name##List * prefix##_list_create(type_name *node) {                          \
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

#define LIST(prefix, type_name, dealloc_func, print_func) \
DEFAULT_LIST_IMPLEMENTATION(type_name)                    \
DEFAULT_LIST_CREATE_EMPTY(prefix, type_name)              \
DEFAULT_LIST_CREATE(prefix, type_name)                    \
DEFAULT_LIST_APPEND(prefix, type_name)                    \
DEFAULT_LIST_DEALLOC(prefix, type_name, dealloc_func)     \
DEFAULT_LIST_SIZE(prefix, type_name)                      \
DEFAULT_LIST_GET_AT(prefix, type_name)                    \
DEFAULT_LIST_PRINT(prefix, type_name, print_func)     


#endif // end LIST_IMPLMENTATION