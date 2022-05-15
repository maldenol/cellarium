// This file is part of cellarium.
//
// cellarium is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// cellarium is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with cellarium.  If not, see <https://www.gnu.org/licenses/>.

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
