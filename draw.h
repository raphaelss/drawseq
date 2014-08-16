#ifndef DRAW_H_INCLUDED__
#define DRAW_H_INCLUDED__
#include <cairo.h>

struct draw_dev;

struct draw_dev_conf {
  unsigned width, height;
  double origin_x, origin_y, line_width;
  enum {
    DRAW_DEV_CAP_BUTT,
    DRAW_DEV_CAP_ROUND,
    DRAW_DEV_CAP_SQUARE
  } line_cap;
};

void draw_dev_conf_default(struct draw_dev_conf* c);
struct draw_dev* draw_init(const char* filepath, struct draw_dev_conf* c);
void draw_line_to(struct draw_dev* d, double x1, double y1);
void draw_move_to(struct draw_dev* d, double x1, double y1);
void draw_release(struct draw_dev* d);


#endif
