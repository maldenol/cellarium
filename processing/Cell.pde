// Copyright 2021 Malovanyi Denys Olehovych (maldenol)
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

class Cell {
  public int[] genom;
  public int counter;

  public int energy;

  public int direction;
  public int posX;
  public int posY;

  public int colorR;
  public int colorG;
  public int colorB;

  public int energyShareBalance;
  public float lastEnergyShare;

  public boolean isAlive;

  public boolean pinned;

  public Cell(
    int[] genom,
    int energy,
    int direction,
    int posX,
    int posY
  ) {
    this.genom = new int[genom.length];
    for (int i = 0; i < genom.length; ++i) {
      this.genom[i] = genom[i];
    }
    this.counter = 0;

    this.energy = energy;

    this.direction = direction;

    this.posX = posX;
    this.posY = posY;

    this.colorR = 0;
    this.colorG = 0;
    this.colorB = 0;

    this.energyShareBalance = 0;
    this.lastEnergyShare    = 0;

    this.isAlive = true;

    this.pinned = false;
  }
}
