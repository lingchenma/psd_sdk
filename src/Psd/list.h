
//
// list.h
//
// Copyright (c) 2010 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef LIST_H
#define LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

// Library version

#define LIST_VERSION "0.0.5"

// Memory management macros

#ifndef LIST_MALLOC
#define LIST_MALLOC malloc
#endif

#ifndef LIST_FREE
#define LIST_FREE free
#endif

/*
 * list_t iterator direction.
 */

typedef enum {
    LIST_HEAD
  , LIST_TAIL
} list_direction_t;

/*
 * list_t node struct.
 */

typedef struct list_node {
  struct list_node *prev;
  struct list_node *next;
  void *val;
} list_node_t;

/*
 * list_t struct.
 */

typedef struct {
  list_node_t *head;
  list_node_t *tail;
  unsigned int len;
  void (*free)(void *val, void* allocator);
  int (*match)(void *a, void *b);
} list_t;

/*
 * list_t iterator struct.
 */

typedef struct {
  list_node_t *next;
  list_direction_t direction;
} list_iterator_t;

// Node prototypes.

list_node_t *
list_node_new2(void *val);

// list_t prototypes.

list_t *
list_new2();

list_node_t *
list_rpush2(list_t *self, list_node_t *node);

list_node_t *
list_lpush2(list_t *self, list_node_t *node);

list_node_t *
list_find2(list_t *self, void *val);

list_node_t *
list_at2(list_t *self, int index);

list_node_t *
list_rpop2(list_t *self);

list_node_t *
list_lpop2(list_t *self);

void
list_remove2(list_t *self, list_node_t *node, void* allocator);

void
list_destroy2(list_t *self, void* allocator);

// list_t iterator prototypes.

list_iterator_t *
list_iterator_new2(list_t *list, list_direction_t direction);

list_iterator_t *
list_iterator_new_from_node2(list_node_t *node, list_direction_t direction);

list_node_t *
list_iterator_next2(list_iterator_t *self);

void
list_iterator_destroy2(list_iterator_t *self);

#ifdef __cplusplus
}
#endif

#endif /* LIST_H */
