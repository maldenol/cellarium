// Copyright 2021 Malovanyi Denys Olehovych (maldenol)
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef COMMAND_LINE_HPP
#define COMMAND_LINE_HPP

// Controls
#include "./controls.hpp"

// CellController
#include "./cell_controller.hpp"

// Generates default configuration file
int generateDefaultConfigurationFile();

// Processes command line arguments updating CellController::Params and Controls
int processCommandLineArguments(int argc, char *argv[], const std::string &title, Controls &controls, cellarium::CellController::Params &cellControllerParams);

#endif
