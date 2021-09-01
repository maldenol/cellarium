float cellSideLength;
CellController cellController;

boolean render;


void setup() {
  fullScreen();
  frameRate(10000000);
  noCursor();

  cellController = new CellController();

  render = true;
}

void draw() {
  cellController.act();

  if (render) {
    background(0);
    cellController.render();
  }
}


void keyReleased() {
  if (key == ' ') {
    render = !render;
  }
}
