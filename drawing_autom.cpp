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

#include "drawing_autom.hpp"
#include <iostream>
#include <stdexcept>

void drawing_autom::initDrawingSystem() {
  Magick::InitializeMagick(nullptr);
}

drawing_autom::drawing_autom(double width, double height, double originX,
                           double originY, double scale, double line_width,
                           LineCap cap)
  :dCount(0), img(Magick::Geometry(width,height), Magick::Color("white")),
   drawInstructions({
     Magick::DrawableStrokeAntialias(true),
     Magick::DrawableStrokeOpacity(1),
     Magick::DrawableStrokeWidth(line_width),
     Magick::DrawableStrokeColor(Magick::Color("black")),
     Magick::DrawableStrokeLineCap(Magick::RoundCap),
     Magick::DrawableTranslation(originX, originY),
     Magick::DrawableScaling(scale,scale),
     Magick::DrawableStrokeLineCap(convertCap(cap))
   }) {}

bool drawing_autom::instruction(int ch) {
  switch(ch) {
  case 'd':
    line();
    break;
  case 'u':
    flushDraw();
    move();
    break;
  case '[':
    flushDraw();
    push();
    break;
  case ']':
    flushDraw();
    pop();
    break;
  case '<':
    flushDraw();
    turnCounterclockwise();
    break;
  case '>':
    flushDraw();
    turnClockwise();
    break;
  case 'o':
    flushDraw();
    moveToOrigin();
    break;
  case 'r':
    flushDraw();
    reset();
    break;
  default:
    return false;
  }
  return true;
}


void drawing_autom::line() {
  dCount += repeatCount;
}

void drawing_autom::move() {
  state().move(repeatCount);
}

void drawing_autom::turnCounterclockwise() {
  state().turn(repeatCount);
}

void drawing_autom::turnClockwise() {
  state().turn(-repeatCount);
}

void drawing_autom::moveToOrigin() {
  state().move_to_origin();
}

void drawing_autom::reset() {
  state().reset();
}

void drawing_autom::render(const std::string& path) {
  flushDraw();
  img.draw(drawInstructions);
  try {
    img.write(path);
  } catch (Magick::Exception &e) {
    throw std::invalid_argument(e.what());
  }
}

void drawing_autom::flushDraw() {
  if (dCount) {
    draw_state &st = state();
    const double x1 = st.x(), y1 = st.y();
    st.move(dCount);
    drawInstructions.push_back(
      Magick::DrawableLine(x1, y1, st.x(), st.y())
    );
    dCount = 0;
  }
}

Magick::LineCap drawing_autom::convertCap(drawing_autom::LineCap cap) {
  switch (cap) {
  case ROUND_CAP:
    return Magick::RoundCap;
  case SQUARE_CAP:
    return Magick::SquareCap;
  case DEFAULT_CAP:
  default:
    return Magick::ButtCap;
  }
}

