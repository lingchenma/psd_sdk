
//
// node.c
//
// Copyright (c) 2010 TJ Holowaychuk <tj@vision-media.ca>
//

#include "list.h"

#pragma warning(disable: 4706)

/*
 * Allocates a new list_node_t. NULL on failure.
 */

list_node_t*
list_node_new2(void* val) {
    list_node_t* self;
    if (!(self = LIST_MALLOC(sizeof(list_node_t))))
        return NULL;
    self->prev = NULL;
    self->next = NULL;
    self->val = val;
    return self;
}