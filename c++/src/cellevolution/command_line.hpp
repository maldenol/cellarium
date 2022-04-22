// This file is part of cellevolution.
//
// cellevolution is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// cellevolution is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with cellevolution.  If not, see <https://www.gnu.org/licenses/>.

#ifndef COMMAND_LINE_HPP
#define COMMAND_LINE_HPP

// Controls
#include "./controls.hpp"

// CellController
#include "./cell_controller.hpp"

// Generates default configuration file
int generateDefaultConfigurationFile();

// Processes command line arguments updating CellController::Params and Controls
int processCommandLineArguments(int argc, char *argv[], const std::string &title, Controls &controls, CellEvolution::CellController::Params &cellControllerParams);

#endif
