/***
* letdraw: command-line drawing tool
*   - see README.mdown for instructions
* Copyright (C) 2015  Raphael Santos, http://www.raphaelss.com/
* MIT License
***/

#ifndef STATE_H_INCLUDED__
#define STATE_H_INCLUDED__

struct state {
  double x, y;
  unsigned angle;
};

struct state_stack {
  struct state *stack;
  struct state current;
  unsigned stack_n, stack_max, d_count, repeat_count;
};

int init_state_stack(struct state_stack *sst);
void release_state_stack(struct state_stack *sst);
int push_state(struct state_stack *sst);
int pop_state(struct state_stack *sst);


#endif
