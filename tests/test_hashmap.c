#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../src/collections/hashmap.h"
#define NO_START_LINE 1
#include "test.h"

long str_equals(const void* a, const void* b) { return strcmp(a, b) == 0; }

unsigned long str_hash(const void* a) {
  unsigned long i = 0;
  long j          = 0;
  const char* ptr = a;
  while (ptr[j]) {
    i *= 17 + ptr[j];
    j++;
  }
  return i;
}

void itora(int n, char* str, int base) {
  if (n < 0)
    return;
  *str = (n % base) + '0';
  if (n >= base)
    itora(n / base, str + 1, base);
}

void str_rev(char* s) {
  int i, j;
  char c;
  for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
    c    = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

#define DEFAULT_CONST -49725
#define TESTS 10
#define ELEMENTS 50

void* default_const_alloc() {
  int* some = malloc(sizeof(int));
  *some     = DEFAULT_CONST;
  return some;
}

int main(int argc, char* argv[]) {
  TESTS_INIT();
  struct hashmap* map = hashmap_alloc(3, &str_hash, &str_equals);
  TESTCASE(hashmap_capacity(map) == 16, "Minimal Capacity");

  // Let's put str, x where atoi(str) == x
  for (int i = 0; i < 10; i++) {
    char* str = malloc(sizeof(char) * 2);
    itora(i, str, 10);
    str_rev(str);
    int* j = malloc(sizeof(int));
    *j     = i;
    hashmap_put(map, str, j);
  }
  TESTCASE(hashmap_size(map) == 10, "Size 1");
  // Initialize random
  srand(time(NULL));
  char buffer[50] = {};
  TESTCASE_COMPOUND("Contains+Get Key Validation");
  for (int i = 0; i < TESTS; i++) {
    int random = rand() % 10;
    itora(random, buffer, 10);
    str_rev(buffer);
    TESTCASE_COMPOUND_AND(hashmap_contains_key(map, buffer) &&
                          *((int*)hashmap_get(map, buffer)) == random);
  }
  TESTCASE_COMPOUND_END();

  TESTCASE_RUN(hashmap_capacity(map) == 32, hashmap_expand(map),
               "Expand Capacity");
  TESTCASE(hashmap_size(map) == 10, "Size Unchanged by Expand");

  // Let's add more elements
  for (int i = 0; i < ELEMENTS; i++) {
    memset(buffer, 0, 50);
    int random = (rand() % (i * 10 + 1)) + 10;
    itora(random, buffer, 10);
    str_rev(buffer);
    char* str = malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(str, buffer);
    int* j = malloc(sizeof(int));
    *j     = random;
    hashmap_put(map, str, j);
  }
  TESTCASE(hashmap_capacity(map) >= 64, "Auto Expand");
  TESTCASE(hashmap_size(map) > 10 && hashmap_size(map) <= 10 + ELEMENTS,
           "Size 2 : %ld", hashmap_size(map));

  // Let's test defaults
  TESTCASE(hashmap_get(map, "Plouf") == NULL, "Get NULL");
  TESTCASE(*((char*)hashmap_get_or_default(map, "Plouf", buffer)) == buffer[0],
           "Get Default");
  int obtained =
      (*(int*)hashmap_lget_or_default(map, buffer, &default_const_alloc));
  TESTCASE(obtained == atoi(buffer), "Get Lazy : %s == %d", buffer, obtained);
  // Let's remove the last element
  hashmap_remove(map, buffer);
  TESTCASE((*(int*)hashmap_lget_or_default(
               map, buffer, &default_const_alloc)) == DEFAULT_CONST,
           "Remove + Get Lazy Default");

  // Let's test iterator
  struct hashmap_iterator* it = hashmap_it_alloc();
  TESTCASE(it != NULL, "Iterator");
  TESTCASE(!hashmap_it_has_next(it) && !hashmap_it_next(it, NULL, NULL),
           "No next when no map");
  hashmap_it_set_map(it, map);
  TESTCASE(hashmap_it_has_next(it), "Next when non-empty map");
  void** key   = malloc(sizeof(char*));
  void** value = malloc(sizeof(int*));
  int count    = 0;
  TESTCASE_COMPOUND("While has next then successful next");
  while (hashmap_it_has_next(it)) {
    TESTCASE_COMPOUND_AND(hashmap_it_next(it, key, value));
    count++;
  }
  TESTCASE_COMPOUND_END();
  TESTCASE(!hashmap_it_next(it, key, value), "Next fails when no next");
  TESTCASE(count == hashmap_size(map), "Iterated over all elements");
  hashmap_it_reset(it);
  TESTCASE(hashmap_it_has_next(it), "Reset");
  TESTCASE_RUN(count == hashmap_size(map), count = 0;
               while (hashmap_it_next(it, key, value)) { count++; },
               "While successful next");
  TESTCASE(!hashmap_it_skip(it, NULL, NULL, 4 * hashmap_size(map)),
           "Skip to 4*size fails");
  TESTCASE(!hashmap_it_has_next(it), "Has next too");
  hashmap_it_reset(it);
  TESTCASE(hashmap_it_skip(it, NULL, NULL, hashmap_size(map)),
           "Skip to size success");
  hashmap_clear(map);
  TESTCASE(hashmap_size(map) == 0, "Remove");
  free(key);
  free(value);
  free(it);
  free_hashmap(map);
  map = hashmap_alloc(33, &str_hash, &str_equals);
  TESTCASE(hashmap_capacity(map) == 64, "Allocation 2");
  TESTS_REPORT();
  return 0;
}
