#ifndef LINKED_LIST_H
#define LINKED_LIST_H
struct node;
struct linked_list;
struct linked_list* linked_list_alloc();
void free_linked_list(struct linked_list* list);
char linked_list_is_empty(const struct linked_list* list);
long linked_list_size(const struct linked_list* list);
void* linked_list_pop_tail(struct linked_list* list);
void linked_list_push_tail(struct linked_list* list, void* content);
void* linked_list_pop_head(struct linked_list* list);
void linked_list_push_head(struct linked_list* list, void* content);
void linked_list_concat(struct linked_list* a, const struct linked_list* b);
struct linked_list_iterator;
struct linked_list_iterator* linked_list_it_alloc();
void linked_list_it_reset(struct linked_list_iterator* it);
void linked_list_it_set_list(struct linked_list_iterator* it,
                             struct linked_list* list);
char linked_list_it_has_next(struct linked_list_iterator* it);
char linked_list_it_next(struct linked_list_iterator* it, void** content);
char linked_list_it_skip(struct linked_list_iterator* it, void** content,
                         long n);

#endif
