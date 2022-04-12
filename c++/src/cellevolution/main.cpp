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

int gCurrTick = 0;

int  gCellRenderingMode = 0;
int  gTicksPerDraw      = 1;
bool gDrawBackground    = true;
bool gDraw              = true;
bool gPause             = false;

int main(int argc, char *argv[]) {
  CellController cellController{};

  while (true) {
    bool drawCurrTick = gDraw && (gCurrTick % gTicksPerDraw == 0);

    // Handling next simulation tick
    if (!gPause) {
      cellController.act(drawCurrTick && gDrawBackground);
    }

    // Rendering the simulation
    if (drawCurrTick) {
      gCurrTick = 0;

      //cellController.render(drawBackground, cellRenderingMode);
    }

    ++gCurrTick;
  }

  return 0;
}
