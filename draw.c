#include "draw.h"
#include <cairo-pdf.h>
#include <string.h>
#include <stdlib.h>

struct draw_dev {
  cairo_surface_t *surface;
  cairo_t *cr;
  double origin_x, origin_y;
  enum {DRAW_DEV_PDF, DRAW_DEV_PNG} type;
  char *filepath;
};

void draw_dev_conf_default(struct draw_dev_conf* c)
{
  c->width = 800;
  c->height = 600;
  c->origin_x = 0;
  c->origin_y = 0;
  c->line_width = 2.0;
  c->line_cap = DRAW_DEV_CAP_BUTT;
}

struct draw_dev* draw_init(const char *filepath, struct draw_dev_conf *c)
{
  unsigned length = strlen(filepath);
  if(length < 5) {
    return NULL;
  }
  struct draw_dev* d = malloc(sizeof *d);
  if(!d) {
    return NULL;
  }
  const char *ending = filepath + length - 4;
  if(strstr(filepath, ".pdf") == ending) {
    d->surface = cairo_pdf_surface_create(filepath, c->width, c->height);
    d->type = DRAW_DEV_PDF;
  } else if(strstr(filepath, ".png") == ending) {
    d->surface =
        cairo_image_surface_create(CAIRO_FORMAT_RGB24, c->width, c->height);
    d->type = DRAW_DEV_PNG;
  } else {
    free(d);
    return NULL;
  }
  if(cairo_surface_status(d->surface) != CAIRO_STATUS_SUCCESS) {
    free(d);
    return NULL;
  }
  d->cr = cairo_create(d->surface);
  if(cairo_status(d->cr) != CAIRO_STATUS_SUCCESS) {
    cairo_surface_destroy(d->surface);
    free(d);
    return NULL;
  }
  cairo_set_source_rgb(d->cr, 1, 1, 1);
  cairo_paint(d->cr);
  cairo_set_source_rgb(d->cr, 0, 0, 0);
  cairo_set_line_width(d->cr, c->line_width);
  switch(c->line_cap) {
  case DRAW_DEV_CAP_BUTT:
    cairo_set_line_cap(d->cr, CAIRO_LINE_CAP_BUTT);
    break;
  case DRAW_DEV_CAP_ROUND:
    cairo_set_line_cap(d->cr, CAIRO_LINE_CAP_ROUND);
    break;
  case DRAW_DEV_CAP_SQUARE:
    cairo_set_line_cap(d->cr, CAIRO_LINE_CAP_SQUARE);
    break;
  default:
    cairo_surface_destroy(d->surface);
    cairo_destroy(d->cr);
    free(d);
    return NULL;
  }
  d->filepath = malloc((length+1)*sizeof(char));
  if(!d->filepath) {
    cairo_surface_destroy(d->surface);
    cairo_destroy(d->cr);
    free(d);
    return NULL;
  }
  strcpy(d->filepath,filepath);
  cairo_move_to(d->cr, c->origin_x, c->origin_y);
  d->origin_x = c->origin_x;
  d->origin_y = c->origin_y;
  return d;
}

void draw_line_to(struct draw_dev *d, double x, double y)
{
  cairo_line_to(d->cr, x+d->origin_x, y+d->origin_y);
}

void draw_move_to(struct draw_dev *d, double x, double y)
{
  cairo_move_to(d->cr, x+d->origin_x, y+d->origin_y);
}

void draw_release(struct draw_dev *d)
{
  cairo_stroke(d->cr);
  switch(d->type) {
  case DRAW_DEV_PNG:
    cairo_surface_write_to_png(d->surface, d->filepath);
    break;
  default:
    break;
  }
  cairo_surface_destroy(d->surface);
  cairo_destroy(d->cr);
  free(d->filepath);
  free(d);
}

