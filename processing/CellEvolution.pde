// Copyright 2021 Malovanyi Denys Olehovych (maldenol)
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

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
