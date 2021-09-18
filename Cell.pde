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

class Cell {
  private int[] genom;
  private int counter;
  private int energy;
  private int direction;
  private int colorR, colorG, colorB;
  private boolean isAlive;


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
