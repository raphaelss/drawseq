#include "state.h"
#include <stdio.h>
#include <stdlib.h>

#define STACK_ALLOC 20

int
init_state_stack(struct state_stack *sst)
{
  if (!(sst->stack = malloc(sizeof(struct state) * STACK_ALLOC))) {
    return 1;
  }
  sst->current.x = 0;
  sst->current.y = 0;
  sst->current.angle = 0;
  sst->stack_n = 0;
  sst->stack_max = STACK_ALLOC;
  sst->d_count = 0;
  sst->repeat_count = 1;
  return 0;
}

void
release_state_stack(struct state_stack *sst)
{
  free(sst->stack);
}

int
push_state(struct state_stack *sst)
{
  if (sst->stack_n == sst->stack_max) {
    struct state *tmp = realloc(sst->stack, sizeof(*tmp)*sst->stack_max*2);
    if (tmp) {
      sst->stack = tmp;
      sst->stack_max *= 2;
    } else {
      fputs("Memory allocation error while expanding stack\n", stderr);
      return 1;
    }
  }
  sst->stack[sst->stack_n++] = sst->current;
  return 0;
}

int
pop_state(struct state_stack *sst)
{
  if (sst->stack_n > 0) {
    sst->current = sst->stack[--sst->stack_n];
    return 0;
  }
  fputs("Stack error", stderr);
  return 1;
}

