#include <assert.h>
#include <stdlib.h>

#include "linked_list.h"

struct node {
  void* content;
  struct node* next;
  struct node* prev;
};

struct linked_list {
  long size;
  struct node* head;
  struct node* tail;
};

struct linked_list* linked_list_alloc() {
  struct linked_list* list = malloc(sizeof(struct linked_list));
  list->size               = 0;
  list->head               = NULL;
  list->tail               = NULL;
  return list;
}
void free_linked_list(struct linked_list* list) {
  assert(list != NULL);
  while (!linked_list_is_empty(list)) {
    linked_list_pop(list);
  }
  free(list);
}

char linked_list_is_empty(const struct linked_list* list) {
  assert(list != NULL);
  return list->size == 0;
}
long linked_list_size(const struct linked_list* list) {
  assert(list != NULL);
  return list->size;
}

void* linked_list_pop_tail(struct linked_list* list) {
  assert(list != NULL);
  assert(!linked_list_is_empty(list));
  struct node* new_tail = list->tail->prev;
  void* content         = list->tail->content;
  free(list->tail);
  if (new_tail != NULL)
    new_tail->next = NULL;
  list->tail = new_tail;
  list->size--;
  if (list->size == 0)
    list->head = NULL;
  return content;
}
void linked_list_push_tail(struct linked_list* list, void* content) {
  assert(list != NULL);
  struct node* new_tail = malloc(sizeof(struct node));
  new_tail->content     = content;
  new_tail->next        = NULL;
  new_tail->prev        = list->tail;
  if (list->tail != NULL)
    list->tail->next = new_tail;
  if (list->head == NULL)
    list->head = new_tail;
  list->tail = new_tail;
  list->size++;
}

void* linked_list_pop_head(struct linked_list* list) {
  assert(list != NULL);
  assert(!linked_list_is_empty(list));
  struct node* new_head = list->head->next;
  void* content         = list->head->content;
  free(list->head);
  if (new_head != NULL)
    new_head->prev = NULL;
  list->head = new_head;
  list->size--;
  if (list->size == 0)
    list->tail = NULL;
  return content;
}
void linked_list_push_head(struct linked_list* list, void* content) {
  assert(list != NULL);
  struct node* new_head = malloc(sizeof(struct node));
  new_head->content     = content;
  new_head->prev        = NULL;
  new_head->next        = list->head;
  if (list->head != NULL)
    list->head->prev = new_head;
  if (list->tail == NULL)
    list->tail = new_head;
  list->head = new_head;
  list->size++;
}

void linked_list_concat(struct linked_list* a, const struct linked_list* b) {
  assert(a != NULL && b != NULL);
  if (linked_list_is_empty(a)) {
    *a = *b;
  } else if (!linked_list_is_empty(b)) {
    a->tail->next = b->head;
    a->size += b->size;
    a->tail = b->tail;
  }
}

struct linked_list_iterator {
  char consumed;
  struct node* cursor;
  struct linked_list* list;
};

struct linked_list_iterator* linked_list_it_alloc() {
  struct linked_list_iterator* it = malloc(sizeof(struct linked_list_iterator));
  it->cursor                      = NULL;
  it->consumed                    = 0;
  return it;
}
void linked_list_it_reset(struct linked_list_iterator* it) {
  assert(it != NULL);
  it->consumed = 0;
  it->cursor   = it->list->head;
}
void linked_list_it_set_list(struct linked_list_iterator* it,
                             struct linked_list* list) {
  assert(it != NULL && list != NULL);
  it->list   = list;
  it->cursor = list->head;
  linked_list_it_reset(it);
}
char linked_list_it_has_next(struct linked_list_iterator* it) {
  assert(it != NULL);
  if (it->cursor == NULL)
    return 0;
  if (it->consumed) {
    it->consumed = 0;
    it->cursor   = it->cursor->next;
  }
  return it->cursor != NULL;
}
char linked_list_it_remove(struct linked_list_iterator* it) {
  assert(it != NULL);
  if (it->cursor == NULL)
    return 0;
  if (it->consumed) {
    it->consumed = 0;
    it->cursor   = it->cursor->next;
  }
  if (it->cursor == NULL)
    return 0;
  if (it->cursor->prev != NULL)
    it->cursor->prev->next = it->cursor->next;
  else
    it->list->head = it->cursor->next;
  if (it->cursor->next == NULL)
    it->list->tail = it->cursor->prev;
  struct node* new_cursor = it->cursor->prev;
  free(it->cursor);
  it->cursor   = new_cursor;
  it->consumed = 1;
  return 1;
}
char linked_list_it_next(struct linked_list_iterator* it, void** content) {
  assert(it != NULL);
  if (it->cursor == NULL)
    return 0;
  if (it->consumed) {
    it->consumed = 0;
    it->cursor   = it->cursor->next;
  }
  if (it->cursor == NULL)
    return 0;
  it->consumed = 1;
  if (content != NULL)
    *content = it->cursor->content;
  return 1;
}
char linked_list_it_skip(struct linked_list_iterator* it, void** content,
                         long n) {
  while (n > 0 && linked_list_it_has_next(it)) {
    linked_list_it_next(it, content);
    n--;
  }
  return n == 0;
}
