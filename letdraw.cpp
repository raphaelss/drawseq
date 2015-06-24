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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <boost/program_options.hpp>
#include "drawing_autom.hpp"

namespace po = boost::program_options;

extern const char *helpDetails;

int main(int argc, char **argv) {
  double width, height, originX, originY, scale, lineWidth;
  std::vector<std::string> input;
  std::string output, capS;
  drawing_autom::LineCap cap;
  po::options_description descr("letdraw - command-line drawing tool");
  descr.add_options()
    ("help,h", "Produce help message.")
    ("input,i", po::value<std::vector<std::string>>(&input),
     "Input file(s) containing sequence of characters. "
     "If no file is specified, letdraw reads from STDIN. "
     "Input file(s) may be specified positionally "
     "(without flags)")
    ("output,o", po::value<std::string>(&output),
     "Path to output image file. Format is detected by the "
     "file extension. (Required)")
    ("width,w", po::value<double>(&width)->default_value(800),
     "Width of image canvas.")
    ("height,H", po::value<double>(&height)->default_value(600),
     "Height of image canvas.")
    ("origin_x,x", po::value<double>(&originX),
     "X of starting point.")
    ("origin_y,y", po::value<double>(&originY),
     "Y of starting point.")
    ("scale,s", po::value<double>(&scale)->default_value(1),
     "Scale drawing lines.")
    ("line_cap,c",
     po::value<std::string>(&capS)->default_value("default"),
     "Line end shape (default, round or square).")
    ("line_width,l",
     po::value<double>(&lineWidth)->default_value(2),
     "Width of line stroke.");
  po::positional_options_description p;
  p.add("input", -1);
  po::variables_map vm;
  try {
    po::store(
      po::command_line_parser(argc, argv).options(descr)
                                    .positional(p).run(),
      vm
    );
  } catch (const po::error& e) {
    std::cout << "letdraw: " << e.what() << std::endl;
    return 1;
  }
  po::notify(vm);
  if (vm.count("help")) {
    std::cout << descr << helpDetails << std::endl;
    return 0;
  }
  if (!vm.count("output")) {
    std::cout << "letdraw: no output file specified\n";
    return 1;
  }
  if (!vm.count("origin_x")) {
    originX = width / 2;
  }
  if (!vm.count("origin_y")) {
    originY = height / 2;
  }
  if (scale <= 0) {
    std::cout << "letdraw: scale must be positive\n";
    return 1;
  }
  if (lineWidth <= 0) {
    std::cout  << "letdraw: line_width must be positive\n";
    return 1;
  }
  if (capS == "default") {
    cap = drawing_autom::DEFAULT_CAP;
  } else if (capS == "round") {
    cap = drawing_autom::ROUND_CAP;
  } else if (capS == "square") {
    cap = drawing_autom::SQUARE_CAP;
  } else {
    std::cout << "letdraw: unsupported line_cap value. Supported "
                 "values: default, round and square\n";
    return 1;
  }
  drawing_autom da(width, height, originX, originY, scale, lineWidth, cap);
  if (!vm.count("input")) {
    da.processStream(std::cin);
  } else {
    auto n = input.size();
    std::vector<std::ifstream> streams(n);
    for (decltype(n) i = 0; i < n; ++i) {
      streams[i].open(input[i]);
      if (!streams[i]) {
        std::cout << "letdraw: error opening " << input[i] << ".\n";
        return 1;
      }
    }
    for(auto& s : streams) {
      da.processStream(s);
    }
  }
  try {
    da.render(output);
  } catch (std::invalid_argument &e) {
    std::cout << "letdraw: " << e.what() << std::endl;
  };
  return 0;
}

const char *helpDetails =
  "Letdraw reads characters from input and interprets them as "
  "instructions for a drawing\nmachine. Unsupported characters "
  "are ignored.\n"
  "Supported characters:\n"
  "  d : Move forward drawing line\n"
  "  u : Move forward without drawing\n"
  "  < : Turn 15 degrees counterclockwise\n"
  "  > : Turn 15 degrees clockwise\n"
  "  [ : Push state (position and direction) into stack\n"
  "  ] : Pop state (position and direction) from stack\n"
  "  o : Move to origin without drawing\n"
  "  r : Move to origin without drawing and reset angle to 0 degrees\n"
  "  # : Repeat next instruction # times\n"
  "# == any single digit number.\n"
  "# instruction is cumulative. Ex.: 2d = dd, 3d = ddd, 23d = 6d.\n"
  "Stack usage must be balanced (can't pop an empty stack).";
