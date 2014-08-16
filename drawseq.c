#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include "draw.h"

#define STACK_ALLOC 20

struct state {
  double x, y;
  unsigned angle;
};

struct global_state {
  struct state *stack;
  struct state current;
  unsigned stack_n, stack_max, d_count, u_count;
  double origin_x, origin_y, scale;
};

int init_global_state(struct global_state *gs, double x, double y,
                      double scale);
void release_global_state(struct global_state *gs);
int push_state(struct global_state *gs);
int pop_state(struct global_state *gs);
void update_state(struct state *s, double dist);
void update_state_draw(struct global_state *gs, struct draw_dev *dr);
int do_char(int ch, struct global_state *gs, struct draw_dev *dr);

int main(int argc, char **argv)
{
  if(argc != 9) {
    fprintf(stderr, "Usage: %s output_file width height origin_x origin_y "
                    "scale line_width line_cap\n", argv[0]);
    return 1;
  }
  double width = atof(argv[2]);
  if(width <= 0) {
    fputs("Invalid width\n", stderr);
    return 1;
  }
  double height = atof(argv[3]);
  if(height <= 0) {
    fputs("Invalid height\n", stderr);
    return 1;
  }
  double origin_x = atof(argv[4]);
  if(origin_x < 0) {
    fputs("Invalid origin_x\n", stderr);
    return 1;
  }
  double origin_y = atof(argv[5]);
  if(origin_y < 0) {
    fputs("Invalid origin_y\n", stderr);
    return 1;
  }
  double scale = atof(argv[6]);
  if(scale < 0) {
    fputs("Invalid scale\n", stderr);
    return 1;
  }
  double line_width = atof(argv[7]);
  if(line_width <= 0) {
    fputs("Invalid line_width\n", stderr);
    return 1;
  }
  int line_cap = atoi(argv[8]);
  if(line_cap < 0 || line_cap > 2) {
    fputs("Invalid line_cap\n", stderr);
    return 1;
  }

  struct global_state gs;
  if(init_global_state(&gs, origin_x, origin_y, scale)) {
    fputs("Memory allocation error while creating global state", stderr);
    return 1;
  }
  struct draw_dev dr;
  if(draw_init(&dr, argv[1], width, height, line_width, line_cap)) {
    fputs("Error initializing drawing system", stderr);
    release_global_state(&gs);
    return 1;
  }
  draw_move_to(&dr, origin_x, origin_y);
  int ch;
  while((ch = getchar()) != EOF) {
    if(do_char(ch, &gs, &dr)) {
      break;
    }
  }
  update_state_draw(&gs, &dr);
  draw_release(&dr);
  release_global_state(&gs);
  return 0;
}

int init_global_state(struct global_state *gs, double x, double y,
                      double scale)
{
  if(!(gs->stack = malloc(sizeof(struct state) * STACK_ALLOC))) {
    return 1;
  }
  gs->origin_x = x;
  gs->origin_y = y;
  gs->current.x = x;
  gs->current.y = y;
  gs->current.angle = 0;
  gs->stack_n = 0;
  gs->stack_max = STACK_ALLOC;
  gs->d_count = 0;
  gs->u_count = 0;
  gs->scale = scale;
  return 0;
}

void release_global_state(struct global_state *gs)
{
  free(gs->stack);
}

int push_state(struct global_state *gs)
{
  if(gs->stack_n == gs->stack_max) {
    struct state *tmp =
      realloc(gs->stack, sizeof(struct state) * gs->stack_max * 2 );
    if(tmp) {
      gs->stack = tmp;
      gs->stack_max *= 2;
    } else {
      fputs("Memory allocation error while expanding global state stack",
            stderr);
      return 1;
    }
  }
  gs->stack[gs->stack_n++] = gs->current;
  return 0;
}

int pop_state(struct global_state *gs)
{
  if(gs->stack_n > 0) {
    gs->current = gs->stack[--gs->stack_n];
    return 0;
  }
  fputs("Stack error", stderr);
  return 1;
}

void update_state(struct state *s, double dist)
{
  static double trig15 [] = {0, 0.25882, 0.5, 0.70711, 0.86603, 0.96593, 1};
  if(s->angle < 7) {
    s->x -= dist * trig15[s->angle];
    s->y -= dist * trig15[6-s->angle];
  } else if (s->angle < 13) {
    s->x -= dist * trig15[12-s->angle];
    s->y += dist * trig15[s->angle-6];
  } else if (s->angle < 19) {
    s->x += dist * trig15[s->angle-12];
    s->y += dist * trig15[18-s->angle];
  } else {
    s->x += dist * trig15[24-s->angle];
    s->y -= dist * trig15[s->angle-18];
  }
  return;
}

void update_state_draw(struct global_state *gs, struct draw_dev *dr)
{
  if(gs->d_count > 0) {
    update_state(&gs->current, gs->d_count * gs->scale);
    draw_line_to(dr, gs->current.x, gs->current.y);
    gs->d_count = 0;
  } else if (gs->u_count > 0) {
    update_state(&gs->current, gs->u_count * gs->scale);
    draw_move_to(dr, gs->current.x, gs->current.y);
    gs->u_count = 0;
  }
}

int do_char(int ch, struct global_state *gs, struct draw_dev *dr)
{
  switch(ch) {
  case 'd':
    if(gs->u_count != 0) {
      update_state_draw(gs, dr);
    }
    ++gs->d_count;
    return 0;
  case 'u':
    if(gs->d_count != 0) {
      update_state_draw(gs, dr);
    }
    ++gs->u_count;
    return 0;
  }
  if(isspace(ch)) {
    return 0;
  }
  update_state_draw(gs, dr);
  switch(ch) {
  case 'r':
    gs->current.angle = 0; //fall through intentional
  case 'o':
    gs->current.x = gs->origin_x;
    gs->current.y = gs->origin_y;
    draw_move_to(dr, gs->current.x, gs->current.y);
    break;
  case '[':
    if(push_state(gs)) {
      return 1;
    }
    draw_move_to(dr, gs->current.x, gs->current.y);
    break;
  case ']':
    if(pop_state(gs)) {
      return 1;
    }
    draw_move_to(dr, gs->current.x, gs->current.y);
    break;
  case '<':
    gs->current.angle = (gs->current.angle+1)%24;
    break;
  case '>':
    gs->current.angle = gs->current.angle? (gs->current.angle-1)%24 : 23;
    break;
  }
  return 0;
}

