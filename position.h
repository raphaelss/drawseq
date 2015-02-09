/***
* letdraw: command-line drawing tool
*   - see README.mdown for instructions
* Copyright (C) 2015  Raphael Santos, http://www.raphaelss.com/
* MIT License
***/

#ifndef POSITION_H_INCLUDED
#define POSITION_H_INCLUDED

struct position {
  double x, y;
  unsigned angle;
};

struct position_stack {
  struct position *stack;
  unsigned n, max;
};

int init_position_stack(struct position_stack *st);
void release_position_stack(struct position_stack *st);
int push_position(struct position_stack *st, const struct position *p);
int pop_position(struct position_stack *st, struct position *p);
void update_position(struct position *p, double dist);


#endif
