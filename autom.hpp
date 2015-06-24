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

#ifndef AUTOM_HPP_INCLUDED
#define AUTOM_HPP_INCLUDED
#include <stack>
#include <cctype>
#include <algorithm>
#include <istream>
#include <iterator>

template <class T, class State>
class autom {
public:
  autom():repeatCount(1) {}

  State& state() {
    return current;
  }

  void push1() {
    stack.push(current);
  }

  void pop1() {
    current = stack.top();
    stack.pop();
  }

  void push() {
    while (repeatCount--) {
      push1();
    }
  }

  void pop() {
    for (; repeatCount != 1; --repeatCount) {
      stack.pop();
    }
    pop1();
  }

  void operator()(int ch) {
    if (std::isdigit(ch)) {
      repeatCount *= ch - '0';
    } else if(static_cast<T*>(this)->instruction(ch)) {
      repeatCount = 1;
    }
  }

  void processStream(std::istream& in) {
    std::for_each(std::istream_iterator<char>(in),
                  std::istream_iterator<char>(),
                  [this](int ch) {this->operator()(ch);});
  }

  unsigned repeatCount;

private:
  State current;
  std::stack<State> stack;
};

#endif
