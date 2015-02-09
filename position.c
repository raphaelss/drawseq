#include "position.h"
#include <stdio.h>
#include <stdlib.h>

#define STACK_ALLOC 20

int
init_position_stack(struct position_stack *st)
{
  if (!(st->stack = malloc(sizeof(struct position) * STACK_ALLOC)))
    return 1;
  st->n = 0;
  st->max = STACK_ALLOC;
  return 0;
}

void
release_position_stack(struct position_stack *st)
{
  if (st) {
    free(st->stack);
    st->stack = NULL;
  }
}

int
push_position(struct position_stack *st, const struct position *p)
{
  if (st->n == st->max) {
    struct position *tmp = realloc(st->stack, sizeof(*tmp)*st->max*2);
    if (tmp) {
      st->stack = tmp;
      st->max *= 2;
    } else {
      (void) fputs("Memory allocation error while expanding stack\n", stderr);
      return 1;
    }
  }
  st->stack[st->n++] = *p;
  return 0;
}

int
pop_position(struct position_stack *st, struct position *p)
{
  if (st->n > 0) {
    *p = st->stack[--st->n];
    return 0;
  }
  (void) fputs("Stack error", stderr);
  return 1;
}

void
update_position(struct position *p, double dist)
{
  static double trig15 [] = {0, 0.25882, 0.5, 0.70711, 0.86603, 0.96593, 1};
  if (p->angle < 7) {
    p->x -= dist * trig15[p->angle];
    p->y -= dist * trig15[6 - p->angle];
  } else if (p->angle < 13) {
    p->x -= dist * trig15[12 - p->angle];
    p->y += dist * trig15[p->angle - 6];
  } else if (p->angle < 19) {
    p->x += dist * trig15[p->angle - 12];
    p->y += dist * trig15[18 - p->angle];
  } else {
    p->x += dist * trig15[24 - p->angle];
    p->y -= dist * trig15[p->angle - 18];
  }
}

