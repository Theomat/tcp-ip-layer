#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

struct hashmap_element {
  char used;
  void* key;
  void* value;
};

struct hashmap {
  struct hashmap_element* array;
  long capacity; // Must be a power of 2
  long size;     // The number of elements in use
  unsigned long (*hash)(const void*);
  long (*equals)(const void*, const void*);
};

static long highest_bit(const long i) {
  long ret = 1;
  while (ret < i) {
    ret <<= 1;
  }
  return ret / 2;
}

struct hashmap* hashmap_alloc(const long initial_size,
                              unsigned long (*hash)(const void*),
                              long (*equals)(const void*, const void*)) {
  assert(hash != NULL);
  assert(equals != NULL);

  struct hashmap* map = malloc(sizeof(struct hashmap));
  long target_size    = highest_bit(initial_size) << 1;
  if (target_size < 16)
    target_size = 16;
  map->array = calloc(target_size, sizeof(struct hashmap_element));
  // capacity should be a power of two so
  map->capacity = target_size;
  map->size     = 0;
  map->hash     = hash;
  map->equals   = equals;
  return map;
}

void free_hashmap(struct hashmap* map) {

  assert(map != NULL);

  free(map->array);
  free(map);
}

static long hashmap_indexof(const struct hashmap* map, const void* key) {

  assert(map != NULL);

  long index                 = (map->hash(key)) & (map->capacity - 1);
  struct hashmap_element* el = &(map->array[index]);
  while (el->used) {
    if (map->equals(key, el->key)) {
      break;
    } else {
      index = (index + 1) & (map->capacity - 1);
      el    = &(map->array[index]);
    }
  }
  return index;
}

char hashmap_contains_key(const struct hashmap* map, const void* key) {

  assert(map != NULL);

  const long index                 = hashmap_indexof(map, key);
  const struct hashmap_element* el = &(map->array[index]);
  return el->used;
}

void* hashmap_get(const struct hashmap* map, const void* key) {
  return hashmap_get_or_default(map, key, NULL);
}

void* hashmap_get_or_default(const struct hashmap* map, const void* key,
                             void* def) {

  assert(map != NULL);

  const long index                 = hashmap_indexof(map, key);
  const struct hashmap_element* el = &(map->array[index]);
  if (el->used)
    return el->value;
  return def;
}

void* hashmap_lget_or_default(const struct hashmap* map, const void* key,
                              void* (*def)()) {

  assert(map != NULL);

  const long index                 = hashmap_indexof(map, key);
  const struct hashmap_element* el = &(map->array[index]);
  if (el->used)
    return el->value;
  return def();
}

void hashmap_remove(struct hashmap* map, const void* key) {

  assert(map != NULL);

  const long index           = hashmap_indexof(map, key);
  struct hashmap_element* el = &(map->array[index]);
  el->used                   = 0;
  map->size--;
}

void hashmap_clear(struct hashmap* map) {

  assert(map != NULL);

  struct hashmap_element* el = NULL;
  for (unsigned long i = 0; i < map->capacity; i++) {
    el       = &(map->array[i]);
    el->used = 0;
  }
  map->size = 0;
}

long hashmap_load_factor(const struct hashmap* map) {

  assert(map != NULL);

  return (map->size) * 100 / (map->capacity);
}

void hashmap_put(struct hashmap* map, void* key, void* data) {

  assert(map != NULL);

  if (hashmap_load_factor(map) > 70) {
    hashmap_expand(map);
  }
  const long index           = hashmap_indexof(map, key);
  struct hashmap_element* el = &(map->array[index]);
  el->value                  = data;
  if (!(el->used)) {
    el->key  = key;
    el->used = 1;
    map->size++;
  }
}

long hashmap_capacity(const struct hashmap* map) {

  assert(map != NULL);

  return map->capacity;
}

long hashmap_size(const struct hashmap* map) {

  assert(map != NULL);

  return map->size;
}

void hashmap_expand(struct hashmap* map) {

  assert(map != NULL);

  struct hashmap_element* array =
      calloc(map->capacity * 2, sizeof(struct hashmap_element));
  const long old_capacity     = map->capacity;
  struct hashmap_element* old = map->array;
  struct hashmap_element* element;
  map->array    = array;
  map->size     = 0;
  map->capacity = 2 * map->capacity;
  for (long i = 0; i < old_capacity; i++) {
    element = &old[i];
    if (element->used) {
      hashmap_put(map, element->key, element->value);
    }
  }
  free(old);
}

struct hashmap_iterator {
  long index;
  char consumed; // whether or not the currrent index has been consumed
  struct hashmap* map;
};

struct hashmap_iterator* hashmap_it_alloc() {

  struct hashmap_iterator* it = malloc(sizeof(struct hashmap_iterator));
  it->index                   = 0;
  it->consumed                = 0;
  it->map                     = NULL;
  return it;
}
void hashmap_it_reset(struct hashmap_iterator* it) {

  assert(it != NULL);

  it->index    = 0;
  it->consumed = 0;
}
void hashmap_it_set_map(struct hashmap_iterator* it, struct hashmap* map) {

  assert(it != NULL);
  assert(map != NULL);

  it->map = map;
  hashmap_it_reset(it);
}
char hashmap_it_has_next(struct hashmap_iterator* it) {

  assert(it != NULL);

  if (it->map == NULL || it->index >= it->map->capacity)
    return 0;
  if (it->consumed) {
    it->index++;
    it->consumed = 0;
  }
  const long capacity        = it->map->capacity;
  struct hashmap_element* el = &(it->map->array[it->index]);
  while (!(el->used) && (it->index) < capacity) {
    it->index++;
    el = &(it->map->array[it->index]);
  }
  if ((it->index) < capacity) {

    assert(el->used);

    return 1;
  }
  return 0;
}
char hashmap_it_next(struct hashmap_iterator* it, void** key, void** value) {

  assert(it != NULL);

  if (it->map == NULL || it->index >= it->map->capacity)
    return 0;
  if (it->consumed) {
    it->index++;
    it->consumed = 0;
  }
  const long capacity        = it->map->capacity;
  struct hashmap_element* el = &(it->map->array[it->index]);
  while (!(el->used) && (it->index) < capacity) {
    it->index++;
    el = &(it->map->array[it->index]);
  }
  if ((it->index) < capacity) {

    assert(el->used);

    if (key != NULL)
      *key = el->key;
    if (value != NULL)
      *value = el->value;
    it->consumed = 1;
    return 1;
  }
  return 0;
}
char hashmap_it_skip(struct hashmap_iterator* it, void** key, void** value,
                     long n) {

  assert(it != NULL);

  while (n > 0 && hashmap_it_has_next(it)) {
    hashmap_it_next(it, key, value);
    n--;
  }
  return n == 0;
}
