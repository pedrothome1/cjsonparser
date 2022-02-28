#include <stdlib.h>
#include "chstack.h"

ch_stack *cs_init(int capacity) {
  ch_stack *s = malloc(sizeof(ch_stack));
  
  s->elem = malloc(sizeof(char) * capacity);
  s->capacity = capacity;
  s->size = 0;

  return s;
}

void cs_destroy(ch_stack *s) {
  free(s->elem);
  free(s);
}

int cs_push(ch_stack *s, char val) {
  if (s->size < s->capacity) {
    s->elem[s->size++] = val;
  }
  else {
    s->capacity *= 2;
    s->elem = realloc(s->elem, s->capacity);
    cs_push(s, val);
  }

  return val;
}

int cs_pop(ch_stack *s) {
  if (s->size > 0) {
    return s->elem[--s->size];
  }

  return -1;
}

int cs_peek(ch_stack *s) {
  if (s->size > 0) {
    return s->elem[s->size - 1];
  }

  return -1;
}

void cs_reset(ch_stack *s) {
  s->size = 0;
}

int cs_isempty(ch_stack *s) {
  return s->size == 0;
}
