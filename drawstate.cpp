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

#include "drawstate.hpp"
#include <cmath>

DrawState::DrawState(double x_, double y_, int angle_)
  :x(x_), y(y_), angle(angle_) {}

void DrawState::move(double n) {
  double radians = angle * std::acos(-1) / 12.0;
  x -= n * std::sin(radians);
  y -= n * std::cos(radians);
}

void DrawState::turn(int n) {
  angle = (angle + n) % 24;
  if (angle < 0) {
    angle += 24;
  }
}
