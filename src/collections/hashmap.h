struct hashmap;
struct hashmap_iterator;

struct hashmap* hashmap_alloc(const long initial_size,
                              unsigned long (*hash)(const void*),
                              long (*equals)(const void*, const void*));

void free_hashmap(struct hashmap* map);
char hashmap_contains_key(const struct hashmap* map, const void* key);
void* hashmap_get(const struct hashmap* map, const void* key);
void* hashmap_get_or_default(const struct hashmap* map, const void* key,
                             void* def);
void* hashmap_lget_or_default(const struct hashmap* map, const void* key,
                              void* (*def)());
void hashmap_remove(struct hashmap* map, const void* key);
void hashmap_clear(struct hashmap* map);
long hashmap_load_factor(const struct hashmap* map);
void hashmap_put(struct hashmap* map, void* key, void* data);
long hashmap_capacity(const struct hashmap* map);
long hashmap_size(const struct hashmap* map);
void hashmap_expand(struct hashmap* map);
struct hashmap_iterator* hashmap_it_alloc();
void hashmap_it_reset(struct hashmap_iterator* it);
void hashmap_it_set_map(struct hashmap_iterator* it, struct hashmap* map);
char hashmap_it_has_next(struct hashmap_iterator* const it);
char hashmap_it_next(struct hashmap_iterator* it, void** key, void** value);
char hashmap_it_skip(struct hashmap_iterator* it, void** key, void** value,
                     long n);
