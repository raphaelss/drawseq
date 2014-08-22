/***
*  letdraw: command-line drawing tool
*    - see README.mdown for instructions
*
*  Copyright (C) 2014  Raphael Sousa Santos, http://www.raphaelss.com/
*
*  This file is part of letdraw.
*
*  Letdraw is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
***/

#include "draw.h"
#include <stdlib.h>
#include <wand/magick_wand.h>

struct draw_dev {
  MagickWand* mw;
  DrawingWand* dw;
  PixelWand* pw;
};

void draw_dev_conf_default(struct draw_dev_conf* c)
{
  c->width = 800;
  c->height = 600;
  c->origin_x = 400;
  c->origin_y = 300;
  c->scale = 1.0;
  c->line_width = 2.0;
  c->line_cap = DRAW_DEV_CAP_BUTT;
}

struct draw_dev* draw_init(const struct draw_dev_conf *c)
{
  MagickWandGenesis();
  struct draw_dev* d = malloc(sizeof *d);
  if(!d) {
    return NULL;
  }
  d->mw = NewMagickWand();
  d->dw = NewDrawingWand();
  d->pw = NewPixelWand();
  PixelSetColor(d->pw, "white");
  if(MagickNewImage(d->mw, c->width, c->height, d->pw) == MagickFalse) {
    return NULL;
  }
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
}

void draw_line(struct draw_dev* d, double x1, double y1, double x2, double y2)
{
  DrawLine(d->dw, x1, y1, x2, y2);
}

void draw_line_to(struct draw_dev* d, double x, double y)
{
  DrawPathLineToAbsolute(d->dw, x, y);
}

void draw_move_to(struct draw_dev* d, double x, double y)
{
  DrawPathMoveToAbsolute(d->dw, x, y);
}

int draw_finish(struct draw_dev* d, const char* filepath)
{
  //DrawPathFinish(d->dw);
  MagickDrawImage(d->mw, d->dw);
  if(MagickWriteImage(d->mw, filepath) == MagickFalse) {
    return 1;
  }
  DestroyPixelWand(d->pw);
  DestroyDrawingWand(d->dw);
  DestroyMagickWand(d->mw);
  MagickWandTerminus();
  free(d);
  return 0;
}
