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

#include <cmath>

#include <QGuiApplication>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cellevolution/cell_controller.hpp>

// Controls constants
static constexpr int  kCellRenderingMode{0};
static constexpr int  kTicksPerRender{1};
static constexpr bool kEnableRenderingBackground{true};
static constexpr bool kEnableRendering{true};
static constexpr bool kEnablePause{false};

int main(int argc, char *argv[]) {
  // Initializing simulation
  CellController cellController{};

  // Initializing ticks left value
  int ticksLeft{};

  // Render cycle
  while (true) {
    // Checking if current tick should be rendered
    bool renderCurrTick = kEnableRendering && (ticksLeft % kTicksPerRender == 0);

    // If simulation is not paused
    if (!kEnablePause) {
      // Computing next simulation tick
      cellController.act(renderCurrTick && kEnableRenderingBackground);
    }

    // If current tick should be rendern
    if (renderCurrTick) {
      // Zeroing ticks left value
      ticksLeft = 0;

      // Rendering the simulation
    }

    // Updating ticks left value
    ++ticksLeft;
  }

  return 0;
}
