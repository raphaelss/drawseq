/***
* letdraw: command-line drawing tool
*   - see README.mdown for instructions
* Copyright (C) 2015  Raphael Santos, http://www.raphaelss.com/
* MIT License
***/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <getopt.h>
#include "draw.h"
#include "position.h"

void usage(void);
const char *read_opts(struct draw_dev_conf *conf, struct position_stack *st,
                      const char **infile, int argc, char **argv);

int
main(int argc, char **argv)
{
  struct position_stack st;
  struct draw_dev_conf conf;
  draw_dev_conf_default(&conf);
  const char *infile = NULL;
  FILE *fin = stdin;
  const char *filepath = read_opts(&conf, &st, &infile, argc, argv);
  if (!filepath) {
    usage();
    return 1;
  }
  if (infile) {
    if (!(fin = fopen(infile, "r"))) {
      fprintf(stderr, "Error opening file: %s\n", infile);
      return 1;
    }
  }
  if (init_position_stack(&st)) {
    fputs("Memory allocation error while initializing position stack", stderr);
    if (infile) {
      fclose(fin);
    }
    return 1;
  }
  struct draw_dev *dr = draw_dev_init(&conf);
  if (!dr) {
    fputs("Error initializing drawing system", stderr);
    release_position_stack(&st);
    if (infile) {
      fclose(fin);
    }
    return 1;
  }
  int ch;
  struct position s = {0};
  unsigned d_count = 0, repeat_count = 1;
  while ((ch = getc(fin)) != EOF && do_char(ch, &st, dr));
  update_position_draw(&s, dr, d_count);
  if (draw_finish(dr, filepath)) {
    fprintf(stderr, "Error writing image (%s)\n", filepath);
  }
  release_position_stack(&st);
  if (infile) {
    fclose(fin);
  }
  return 0;
}

void
usage(void)
{
  puts(
    "Usage: letdraw --out=FILE [OPTION]\n"
    "  -h            Display this message\n"
    "  -o            Output file (required)\n"
    "  -i            Input file containing sequence of characters\n"
    "                  If no file is specified, letdraw reads from\n"
    "                  STDIN\n"
    "  -w NATURAL    Width of image canvas (default: 800)\n"
    "  -H NATURAL    Height of image canvas (default: 600)\n"
    "  -x REAL       X of starting point (default: width/2)\n"
    "  -y REAL       Y of starting point (default: height/2)\n"
    "  -s REAL>0     Scale drawing lines (default: 1.0)\n"
    "  -l REAL>0     Width of line stroke (default: 2.0)\n"
    "  -c (normal|round|square) Line end shape (default: normal)\n"
    "Letdraw reads characters and treats some of them as instructions for a\n"
    " drawing machine while ignoring the others.\n"
    "Supported characters:\n"
    "  d : Move forward drawing line\n"
    "  u : Move forward without drawing\n"
    "  < : Turn 15 degrees counterclockwise\n"
    "  > : Turn 15 degrees clockwise\n"
    "  [ : Push position (position and direction) into stack\n"
    "  ] : Pop position (position and direction) from stack\n"
    "  o : Move to origin without drawing\n"
    "  r : Move to origin without drawing and reset angle to 0 degrees\n"
    "  # : Execute next instruction # times\n"
    "# = any single digit number.\n"
    "# instruction is cumulative. Ex.: 2d = dd, 3d = ddd, 23d = 6d.\n"
    "Stack usage must be balanced (can't pop an empty stack)."
  );
}

const char *
read_opts(struct draw_dev_conf *conf, struct position_stack *st,
          const char **infile, int argc, char **argv)
{
  const char *filepath = NULL;
  int c;
  int x_unset = 1, y_unset = 1;
  int tmp;
  while ((c = getopt(argc,argv,"ho:i:w:H:s:x:y:l:c:")) != -1) {
    switch(c) {
    case 'h':
    case 1:
      return NULL;
    case 'o':
    case 2:
      filepath = optarg;
      break;
    case 'i':
    case 3:
      *infile = optarg;
      break;
    case 'w':
    case 4:
      //width
      tmp = atoi(optarg);
      if (tmp <= 0) {
        fprintf(stderr, "Invalid width: %s\n", optarg);
        return NULL;
      }
      conf->width = (unsigned) tmp;
      break;
    case 'H':
    case 5:
      //height
      tmp = atoi(optarg);
      if (tmp <= 0) {
        fprintf(stderr, "Invalid height: %s\n", optarg);
        return NULL;
      }
      conf->height = tmp;
      break;
    case 's':
    case 6:
      conf->scale = atof(optarg);
      if (conf->scale <= 0) {
        fprintf(stderr, "Invalid scale: %s\n", optarg);
        return NULL;
      }
      break;
    case 'x':
    case 7:
      conf->origin_x = atof(optarg);
      x_unset = 0;
      break;
    case 'y':
    case 8:
      conf->origin_y = atof(optarg);
      y_unset = 0;
      break;
    case 'l':
    case 9:
      conf->line_width = atof(optarg);
      if (conf->line_width <= 0) {
        fprintf(stderr, "Invalid line_width: %s\n", optarg);
        return NULL;
      }
      break;
    case 'c':
    case 10:
      if (!(strncmp("normal", optarg, 6))) {
        conf->line_cap = DRAW_DEV_CAP_BUTT;
      } else if (!(strncmp("round", optarg, 5))) {
        conf->line_cap = DRAW_DEV_CAP_ROUND;
      } else if (!(strncmp("square", optarg, 5))) {
        conf->line_cap = DRAW_DEV_CAP_SQUARE;
      } else {
        fprintf(stderr, "Invalid line_cap: %s "
                "(possible values: normal round square)\n",
                optarg);
        return NULL;
      }
      break;
    }
  }
  if (x_unset) {
    conf->origin_x = conf->width/2.0;
  }
  if (y_unset) {
    conf->origin_y = conf->height/2.0;
  }
  return filepath;
}

void
update_position_draw(struct position *p, struct draw_dev *dr, unsigned d_count)
{
  if (d_count) {
    double sx = s->x, sy = s->y;
    update_position(s, d_count);
    draw_line(dr, sx, sy, s->x, s->y);
  }
}

int
do_char(int ch, struct position_stack *st, struct draw_dev *dr, unsigned d_count,
        unsigned repeat_count)
{
  if (isdigit(ch)) {
    repeat_count *= ch - '0';
    return 1;
  }
  int return_val = 1;
  switch(ch) {
  case 'd':
    return_val = op_line(st, s, dr);
    break;
  case 'u':
    return_val = op_move(st, dr);
    break;
  case 'r':
    return_val = op_reset(st, dr);
    break;
  case 'o':
    return_val = op_move_to_origin(st, dr);
    break;
  case '[':
    return_val = op_push_stack(st, dr);
    break;
  case ']':
    return_val =  op_pop_stack(st, dr);
    break;
  case '<':
    return_val =  op_15deg_counterclockwise(st, dr);
    break;
  case '>':
    return_val =  op_15deg_clockwise(st, dr);
    break;
  default:
    return 1;
  }
  st->repeat_count = 1;
  return return_val;
}

int
op_line(struct position_stack *st, struct draw_dev *dr)
{
  st->d_count += st->repeat_count;
  return 1;
}

int
op_move(struct position_stack *st, struct draw_dev *dr)
{
  update_position_draw(st, dr);
  update_position(&st->current, st->repeat_count);
  return 1;
}

int
op_reset(struct position_stack *st, struct draw_dev *dr)
{
  update_position_draw(st, dr);
  st->current.angle = 0;
  st->current.x = 0;
  st->current.y = 0;
  return 1;
}

int
op_move_to_origin(struct position_stack *st, struct draw_dev *dr)
{
  update_position_draw(st, dr);
  st->current.x = 0;
  st->current.y = 0;
  return 1;
}

int
op_push_stack(struct position_stack *st, struct draw_dev *dr)
{
  update_position_draw(st, dr);
  while (st->repeat_count--) {
    if (push_position(st)) {
      return 0;
    }
  }
  return 1;
}

int
op_pop_stack(struct position_stack *st, struct draw_dev *dr)
{
  update_position_draw(st, dr);
  while (st->repeat_count--) {
    if (pop_position(st)) {
      return 0;
    }
  }
  return 1;
}

int
op_15deg_counterclockwise(struct position_stack *st, struct draw_dev *dr)
{
  update_position_draw(st, dr);
  st->current.angle = (st->current.angle + st->repeat_count) % 24;
  return 1;
}

int
op_15deg_clockwise(struct position_stack *st, struct draw_dev *dr)
{
  update_position_draw(st, dr);
  if (st->current.angle >= st->repeat_count) {
    st->current.angle -= st->repeat_count;
  } else {
    st->current.angle = 24 - (st->repeat_count - st->current.angle);
  }
  return 1;
}
