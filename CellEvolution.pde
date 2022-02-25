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

int currTick = 0;

int cellRenderingMode  = 0;
int ticksPerDraw       = 1;
boolean drawBackground = true;
boolean draw           = true;
boolean pause          = false;

void settings() {
  size(displayWidth, displayHeight - 95, P2D);
}

void setup() {
  frameRate(1000);

  noStroke();
  rectMode(CORNER);

  cellController = new CellController();
}

void draw() {
  boolean drawCurrentStep = draw && currTick % ticksPerDraw == 0;

  // Handling next simulation tick
  if (!pause) {
    cellController.act(drawCurrentStep && drawBackground);
  }

  // Rendering the simulation
  if (drawCurrentStep) {
    cellController.render(drawBackground, cellRenderingMode);

    currTick = 0;
  }
  ++currTick;
}

boolean keyIsPressed = false;

void keyPressed() {
  if (keyIsPressed) {
    return;
  }

  keyIsPressed = true;

  if (key == 'm') {
    // Switch cell rendering mode
    ++cellRenderingMode;
  } else if (key == '-') {
    // Decrease ticks per draw
    ticksPerDraw = max(ticksPerDraw - 1, 1);
  } else if (key == '+') {
    // Increase ticks per draw
    ++ticksPerDraw;
  } else if (key == 'b') {
    // Toggle draw background flag state
    drawBackground = !drawBackground;
  } else if (key == 'd') {
    // Toggle draw flag state
    draw = !draw;
  } else if (key == 'p') {
    // Toggle draw background flag state
    pause = !pause;
  }
}

void keyReleased() {
  keyIsPressed = false;
}
