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

boolean render;

void setup() {
  fullScreen();
  frameRate(10000000);

  noStroke();
  rectMode(CORNER);

  cellController = new CellController();

  render = true;
}

void draw() {
  // Handling next world tick
  cellController.act();

  // Rendering cells if needed
  if (render) {
    cellController.render();
  }
}

void keyReleased() {
  // If spacebar has been released
  if (key == ' ') {
    // Toggle render state
    render = !render;
  }
}
