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

CellController cellController;

int ticksPerDraw;
int currTick;
boolean draw;

void setup() {
  fullScreen();
  frameRate(10000000);

  noStroke();
  rectMode(CORNER);

  cellController = new CellController();

  ticksPerDraw = 10;
  currTick     = 0;
  draw         = true;
}

void draw() {
  // Handling next world tick
  cellController.act();

  // Rendering cells if needed
  if (draw && currTick % ticksPerDraw == 0) {
    cellController.render();

    currTick = 0;
  }
  ++currTick;
}

void keyReleased() {
  // If spacebar has been released
  if (key == ' ') {
    // Toggle render state
    draw = !draw;
  }
}
