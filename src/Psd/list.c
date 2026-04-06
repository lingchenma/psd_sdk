
//
// list.c
//
// Copyright (c) 2010 TJ Holowaychuk <tj@vision-media.ca>
//

#include "list.h"

#pragma warning(disable: 4706)

/*
 * Allocate a new list_t. NULL on failure.
 */

list_t*
list_new2() {
	list_t* self;
	if (!(self = LIST_MALLOC(sizeof(list_t))))
		return NULL;
	self->head = NULL;
	self->tail = NULL;
	self->free = NULL;
	self->match = NULL;
	self->len = 0;
	return self;
}

/*
 * Free the list.
 */

void
list_destroy2(list_t *self, void* allocator) {
  unsigned int len = self->len;
  list_node_t *next;
  list_node_t *curr = self->head;

  while (len--) {
    next = curr->next;
    if (self->free) self->free(curr->val, allocator);
    LIST_FREE(curr);
    curr = next;
  }

  LIST_FREE(self);
}

/*
 * Append the given node to the list
 * and return the node, NULL on failure.
 */

list_node_t *
list_rpush2(list_t *self, list_node_t *node) {
  if (!node) return NULL;

  if (self->len) {
    node->prev = self->tail;
    node->next = NULL;
    self->tail->next = node;
    self->tail = node;
  } else {
    self->head = self->tail = node;
    node->prev = node->next = NULL;
  }

  ++self->len;
  return node;
}

/*
 * Return / detach the last node in the list, or NULL.
 */

list_node_t *
list_rpop2(list_t *self) {
  if (!self->len) return NULL;

  list_node_t *node = self->tail;

  if (--self->len) {
    (self->tail = node->prev)->next = NULL;
  } else {
    self->tail = self->head = NULL;
  }

  node->next = node->prev = NULL;
  return node;
}

/*
 * Return / detach the first node in the list, or NULL.
 */

list_node_t *
list_lpop2(list_t *self) {
  if (!self->len) return NULL;

  list_node_t *node = self->head;

  if (--self->len) {
    (self->head = node->next)->prev = NULL;
  } else {
    self->head = self->tail = NULL;
  }

  node->next = node->prev = NULL;
  return node;
}

/*
 * Prepend the given node to the list
 * and return the node, NULL on failure.
 */

list_node_t *
list_lpush2(list_t *self, list_node_t *node) {
  if (!node) return NULL;

  if (self->len) {
    node->next = self->head;
    node->prev = NULL;
    self->head->prev = node;
    self->head = node;
  } else {
    self->head = self->tail = node;
    node->prev = node->next = NULL;
  }

  ++self->len;
  return node;
}

/*
 * Return the node associated to val or NULL.
 */

list_node_t *
list_find2(list_t *self, void *val) {
  list_iterator_t *it = list_iterator_new2(self, LIST_HEAD);
  list_node_t *node;

  while ((node = list_iterator_next2(it))) {
    if (self->match) {
      if (self->match(val, node->val)) {
        list_iterator_destroy2(it);
        return node;
      }
    } else {
      if (val == node->val) {
        list_iterator_destroy2(it);
        return node;
      }
    }
  }

  list_iterator_destroy2(it);
  return NULL;
}

/*
 * Return the node at the given index or NULL.
 */

list_node_t *
list_at2(list_t *self, int index) {
  list_direction_t direction = LIST_HEAD;

  if (index < 0) {
    direction = LIST_TAIL;
    index = ~index;
  }

  if ((unsigned)index < self->len) {
    list_iterator_t *it = list_iterator_new2(self, direction);
    list_node_t *node = list_iterator_next2(it);
    while (index--) node = list_iterator_next2(it);
    list_iterator_destroy2(it);
    return node;
  }

  return NULL;
}

/*
 * Remove the given node from the list, freeing it and it's value.
 */

void
list_remove2(list_t *self, list_node_t *node, void* allocator) {
  node->prev
    ? (node->prev->next = node->next)
    : (self->head = node->next);

  node->next
    ? (node->next->prev = node->prev)
    : (self->tail = node->prev);

  if (self->free) self->free(node->val, allocator);

  LIST_FREE(node);
  --self->len;
}
