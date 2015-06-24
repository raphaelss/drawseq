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

#ifndef DRAW_STATE_HPP_INCLUDED
#define DRAW_STATE_HPP_INCLUDED

class draw_state {
public:
  draw_state(double x = 0, double y = 0, int angle = 0);

  void move(double n);
  void turn(int n);

  void move_to_origin();
  void reset();

  double x() const;
  double y() const;

private:
  double _x;
  double _y;
  int _angle;
};

#endif // DRAWSTATE_HPP
