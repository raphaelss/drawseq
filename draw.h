#ifndef DRAW_H_INCLUDED__
#define DRAW_H_INCLUDED__
#include <cairo.h>

struct draw_dev {
  cairo_surface_t *surface;
  cairo_t *cr;
  enum {DRAW_DEV_PDF, DRAW_DEV_PNG} type;
  char *filepath;
};

int draw_init(struct draw_dev *d, const char *filepath, double width,
              double height, double line_width, int line_cap);
void draw_line_to(struct draw_dev *d, double x1, double y1);
void draw_move_to(struct draw_dev *d, double x1, double y1);
void draw_release(struct draw_dev *d);


#endif
