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

#include "draw_state.hpp"
#include <cmath>

draw_state::draw_state(double x, double y, int angle)
    : _x(x), _y(y), _angle(angle) {}

void draw_state::move(double n) {
  double radians = _angle * std::acos(-1) / 12.0;
  _x -= n * std::sin(radians);
  _y -= n * std::cos(radians);
}

void draw_state::turn(int n) {
  _angle = (_angle + n) % 24;
  if (_angle < 0) {
    _angle += 24;
  }
}

void draw_state::move_to_origin() {
  _x = _y = 0;
}

void draw_state::reset() {
  move_to_origin();
  _angle = 0;
}

double draw_state::x() const {
  return _x;
}

double draw_state::y() const {
  return _y;
}

