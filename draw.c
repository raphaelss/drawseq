#include "draw.h"
#include <cairo-pdf.h>
#include <string.h>
#include <stdlib.h>

int draw_init(struct draw_dev *d, const char *filepath, double width,
              double height, double line_width, int line_cap)
{
  unsigned length = strlen(filepath);
  if(length < 5) {
    return 1;
  }
  const char *ending = filepath + length - 4;
  if(strstr(filepath, ".pdf") == ending) {
    d->surface = cairo_pdf_surface_create(filepath, width, height);
    d->type = DRAW_DEV_PDF;
  } else if(strstr(filepath, ".png") == ending) {
    d->surface =
          cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
    d->type = DRAW_DEV_PNG;
  } else {
    return 1;
  }
  if(cairo_surface_status(d->surface) != CAIRO_STATUS_SUCCESS) {
    return 1;
  }
  d->cr = cairo_create(d->surface);
  if(cairo_status(d->cr) != CAIRO_STATUS_SUCCESS) {
    cairo_surface_destroy(d->surface);
    return 1;
  }
  cairo_set_source_rgb(d->cr, 1, 1, 1);
  cairo_paint(d->cr);
  cairo_set_source_rgb(d->cr, 0, 0, 0);
  cairo_set_line_width(d->cr, line_width);
  switch(line_cap) {
  case 0:
    cairo_set_line_cap(d->cr, CAIRO_LINE_CAP_BUTT);
    break;
  case 1:
    cairo_set_line_cap(d->cr, CAIRO_LINE_CAP_ROUND);
    break;
  case 2:
    cairo_set_line_cap(d->cr, CAIRO_LINE_CAP_SQUARE);
  }
  d->filepath = malloc((length+1)*sizeof(char));
  if(!d->filepath) {
    cairo_surface_destroy(d->surface);
    cairo_destroy(d->cr);
    return 1;
  }
  strcpy(d->filepath,filepath);
  return 0;
}

void draw_line_to(struct draw_dev *d, double x, double y)
{
  cairo_line_to(d->cr, x, y);
}

void draw_move_to(struct draw_dev *d, double x, double y)
{
  cairo_move_to(d->cr, x, y);
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
}

