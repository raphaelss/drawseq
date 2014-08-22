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

void draw_dev_conf_default(struct draw_dev_conf* c);
struct draw_dev* draw_init(const struct draw_dev_conf* c);
void draw_line(struct draw_dev* d, double x1, double y1, double x2, double y2);
void draw_line_to(struct draw_dev* d, double x, double y);
void draw_move_to(struct draw_dev* d, double x, double y);
int draw_finish(struct draw_dev* d, const char* filepath);

#endif
