#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include "draw.h"
#include <getopt.h>

#define STACK_ALLOC 20

struct state {
  double x, y;
  unsigned angle;
};

struct global_state {
  struct state *stack;
  struct state current;
  unsigned stack_n, stack_max, d_count, u_count;
};

const char* read_options(struct draw_dev_conf* conf, int argc, char **argv,
                         double* scale);
int init_global_state(struct global_state *gs);
void release_global_state(struct global_state *gs);
int push_state(struct global_state *gs);
int pop_state(struct global_state *gs);
void update_state(struct state *s, double dist);
void update_state_draw(struct global_state *gs, struct draw_dev *dr,
                       double scale);
int do_char(int ch, struct global_state *gs, struct draw_dev *dr,
            double scale);

int main(int argc, char **argv)
{
  struct draw_dev_conf conf;
  draw_dev_conf_default(&conf);
  double scale = 2;
  const char* filepath = read_options(&conf, argc, argv, &scale);
  if(!filepath) {
    puts("Usage:");
    return 1;
  }
  struct global_state gs;
  if(init_global_state(&gs)) {
    fputs("Memory allocation error while creating global state", stderr);
    return 1;
  }
  struct draw_dev* dr = draw_init(filepath, &conf);
  if(!dr) {
    fputs("Error initializing drawing system", stderr);
    release_global_state(&gs);
    return 1;
  }
  int ch;
  while((ch = getchar()) != EOF) {
    if(do_char(ch, &gs, dr, scale)) {
      break;
    }
  }
  update_state_draw(&gs, dr, scale);
  draw_release(dr);
  release_global_state(&gs);
  //Cleans up some valgrind still-reachable errors
  //cairo_debug_reset_static_data();
  return 0;
}

int init_global_state(struct global_state *gs)
{
  if(!(gs->stack = malloc(sizeof(struct state) * STACK_ALLOC))) {
    return 1;
  }
  gs->current.x = 0;
  gs->current.y = 0;
  gs->current.angle = 0;
  gs->stack_n = 0;
  gs->stack_max = STACK_ALLOC;
  gs->d_count = 0;
  gs->u_count = 0;
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

void update_state_draw(struct global_state *gs, struct draw_dev *dr,
                       double scale)
{
  if(gs->d_count > 0) {
    update_state(&gs->current, gs->d_count * scale);
    draw_line_to(dr, gs->current.x, gs->current.y);
    gs->d_count = 0;
  } else if (gs->u_count > 0) {
    update_state(&gs->current, gs->u_count * scale);
    draw_move_to(dr, gs->current.x, gs->current.y);
    gs->u_count = 0;
  }
}

int do_char(int ch, struct global_state *gs, struct draw_dev *dr,
            double scale)
{
  switch(ch) {
  case 'd':
    if(gs->u_count != 0) {
      update_state_draw(gs, dr, scale);
    }
    ++gs->d_count;
    return 0;
  case 'u':
    if(gs->d_count != 0) {
      update_state_draw(gs, dr, scale);
    }
    ++gs->u_count;
    return 0;
  }
  if(isspace(ch)) {
    return 0;
  }
  update_state_draw(gs, dr, scale);
  switch(ch) {
  case 'r':
    gs->current.angle = 0; //fall through intentional
  case 'o':
    gs->current.x = 0;
    gs->current.y = 0;
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

const char* read_options(struct draw_dev_conf* conf, int argc, char **argv,
                         double* scale)
{
  static struct option opts [] = {
    {"help", no_argument, NULL, 1},
    {"o", required_argument, NULL, 2},
    {"width", required_argument, NULL, 3},
    {"height", required_argument, NULL, 4},
    {"scale", required_argument, NULL, 5},
    {"origin_x", required_argument, NULL, 6},
    {"origin_y", required_argument, NULL, 7},
    {"line_width", required_argument, NULL, 8},
    {"line_cap", required_argument, NULL, 9},
    {0,0,0,0}
  };
  const char* filepath = NULL;
  int c;
  while((c = getopt_long_only(argc, argv, "", opts, NULL)) != -1) {
    switch(c) {
    case 1:
      return NULL;
    case 2:
      filepath = optarg;
      break;
    case 3:
      conf->width = atoi(optarg);
      if(!conf->width) {
        puts("Invalid width");
        return NULL;
      }
      break;
    case 4:
      conf->height = atoi(optarg);
      if(!conf->height) {
        puts("Invalid height");
        return NULL;
      }
      break;
    case 5:
      *scale = atof(optarg);
      if(!*scale) {
        puts("Invalid scale");
        return NULL;
      }
      break;
    case 6:
      conf->origin_x = atof(optarg);
      break;
    case 7:
      conf->origin_y = atof(optarg);
      break;
    case 8:
      conf->line_width = atof(optarg);
      if(!conf->line_width) {
        puts("Invalid line_width");
        return NULL;
      }
      break;
    case 9:
      if(!(strncmp("normal", optarg, 6))) {
        conf->line_cap = DRAW_DEV_CAP_BUTT;
      } else if(!(strncmp("round", optarg, 5))) {
        conf->line_cap = DRAW_DEV_CAP_ROUND;
      } else if(!(strncmp("square", optarg, 5))) {
        conf->line_cap = DRAW_DEV_CAP_SQUARE;
      } else {
        puts("Invalid line_cap (possible values are: normal round square");
        return NULL;
      }
    }
  }
  return filepath;
}
