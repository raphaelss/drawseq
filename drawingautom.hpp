// This file is part of letdraw, a command line drawing tool
// Copyright (C) 2014 Raphael Sousa Santos, http://www.raphaelss.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef DRAWINGAUTOM_HPP
#define DRAWINGAUTOM_HPP
#include "autom.hpp"
#include "drawstate.hpp"
#include <Magick++.h>

class DrawingAutom : public Autom<DrawingAutom, DrawState> {
public:
  enum LineCap {DEFAULT_CAP, ROUND_CAP, SQUARE_CAP};

  DrawingAutom(double width, double height, double originX,
               double originY, double scale, double line_width,
               LineCap cap);
  bool instruction(int ch);
  void line();
  void move();
  void turnCounterclockwise();
  void turnClockwise();
  void moveToOrigin();
  void reset();

  void render(const std::string& path);

  static void initDrawingSystem();

private:
  void flushDraw();

  static Magick::LineCap convertCap(LineCap cap);

  unsigned dCount;
  Magick::Image img;
  std::list<Magick::Drawable> drawInstructions;
};

#endif // DRAWINGAUTOM_HPP
