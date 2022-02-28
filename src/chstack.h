#ifndef CHSTACK_H
#define CHSTACK_H

struct ch_stack {
  int size;
  int capacity;
  char *elem;
};

typedef struct ch_stack ch_stack;

ch_stack *cs_init(int capacity);
void cs_destroy(ch_stack *s);
int cs_push(ch_stack *s, char val);
int cs_pop(ch_stack *s);
int cs_peek(ch_stack *s);
void cs_reset(ch_stack *s);
int cs_isempty(ch_stack *s);

#endif
