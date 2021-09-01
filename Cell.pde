class Cell {
  public int[] genom;
  public int counter;
  public int energy;
  public int direction;
  public int colorR, colorG, colorB;
  public boolean isAlive;

  public Cell(int[] genom, int energy, int direction) {
    this.genom = new int[genom.length];
    for (int i = 0; i < genom.length; i++) {
      this.genom[i] = genom[i];
    }
    this.counter = 0;
    this.energy = energy;
    this.direction = direction;
    this.colorR = 0;
    this.colorG = 0;
    this.colorB = 0;
    this.isAlive = true;
  }
}
