/***
* letdraw: command-line drawing tool
*   - see README.mdown for instructions
* Copyright (C) 2015  Raphael Santos, http://www.raphaelss.com/
* MIT License
***/

#ifndef DRAW_H_INCLUDED__
#define DRAW_H_INCLUDED__
struct draw_dev;

enum draw_line_cap {
  DRAW_DEV_CAP_BUTT,
  DRAW_DEV_CAP_ROUND,
  DRAW_DEV_CAP_SQUARE
};

struct draw_dev_conf {
  unsigned width, height;
  double scale, line_width, origin_x, origin_y;
  enum draw_line_cap line_cap;
};

void draw_dev_conf_default(struct draw_dev_conf *c);
struct draw_dev *draw_init(const struct draw_dev_conf *c);
void draw_line(struct draw_dev *d, double x1, double y1, double x2, double y2);
int draw_finish(struct draw_dev *d, const char *filepath);

#endif
