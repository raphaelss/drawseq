/***
* letdraw: command-line drawing tool
*   - see README.md for instructions
* Copyright (C) 2015  Raphael Santos, http://www.raphaelss.com/
* MIT License
***/

#include "draw.h"
#include <stdlib.h>
#include <wand/magick_wand.h>

struct draw_dev {
  MagickWand *mw;
  DrawingWand *dw;
  PixelWand *pw;
};

void
draw_dev_conf_default(struct draw_dev_conf *c)
{
  c->width = 800;
  c->height = 600;
  c->origin_x = 400;
  c->origin_y = 300;
  c->scale = 1.0;
  c->line_width = 2.0;
  c->line_cap = DRAW_DEV_CAP_BUTT;
}

struct draw_dev *
draw_init(const struct draw_dev_conf *c)
{
  MagickWandGenesis();
  struct draw_dev *d = malloc(sizeof *d);
  if (!d)
    goto exit;
  d->mw = NewMagickWand();
  d->dw = NewDrawingWand();
  d->pw = NewPixelWand();
  PixelSetColor(d->pw, "white");
  if (MagickNewImage(d->mw, c->width, c->height, d->pw) == MagickFalse)
    goto clean_up;
  PixelSetColor(d->pw, "black");
  DrawSetStrokeAntialias(d->dw, 1);
  DrawSetStrokeOpacity(d->dw, 1);
  DrawSetFillOpacity(d->dw, 0);
  DrawSetStrokeWidth(d->dw, c->line_width);
  DrawSetStrokeColor(d->dw, d->pw);
  switch(c->line_cap) {
  case DRAW_DEV_CAP_BUTT:
    DrawSetStrokeLineCap(d->dw, ButtCap);
    break;
  case DRAW_DEV_CAP_ROUND:
    DrawSetStrokeLineCap(d->dw, RoundCap);
    break;
  case DRAW_DEV_CAP_SQUARE:
    DrawSetStrokeLineCap(d->dw, SquareCap);
    break;
  }
  DrawTranslate(d->dw, c->origin_x, c->origin_y);
  DrawScale(d->dw, c->scale, c->scale);
  return d;
clean_up:
  DestroyMagickWand(d->mw);
  DestroyDrawingWand(d->dw);
  DestroyPixelWand(d->pw);
  free(d);
exit:
  return NULL;
}

void
draw_line(struct draw_dev *d, double x1, double y1, double x2, double y2)
{
  DrawLine(d->dw, x1, y1, x2, y2);
}

int
draw_finish(struct draw_dev *d, const char *filepath)
{
  if (!d) {
    return 0;
  }
  int ret_val = 0;
  MagickDrawImage(d->mw, d->dw);
  if (MagickWriteImage(d->mw, filepath) == MagickFalse) {
    ret_val = 1;
  }
  DestroyPixelWand(d->pw);
  DestroyDrawingWand(d->dw);
  DestroyMagickWand(d->mw);
  MagickWandTerminus();
  free(d);
  return ret_val;
}
