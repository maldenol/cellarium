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

class CellController {
  public final int rows = height, columns = width;
  public final int genomSize = 64, minChildEnergy = 40, maxEnergy = 200;
  public final int dayDurationInTicks = 240, seasonDurationInDays = 92;
  public final int maxPhotosynthesisEnergy = 30, maxPhotosynthesisDepth = 75;
  public final int maxMineralEnergy = 15, maxMineralDepth = 50;
  public final float foodEfficiency = 0.95;
  public final float randomMutationChance = 0.01, childMutationChance = 0.25, parentMutationChance = 0.05;

  public final int[][] DIRECTIONS = {
    {0, -1},
    {1, -1},
    {1, 0},
    {1, 1},
    {0, 1},
    {-1, 1},
    {-1, 0},
    {-1, -1},
  };

  public Cell[][] cells;
  public ArrayList<Cell> actedCells;

  public float cellSideLength;

  public int ticksNumber, daysNumber, yearsNumber;


  public CellController() {
    this.cells = new Cell[this.columns][this.rows];
    int[] genom = new int[this.genomSize];
    for (int i = 0; i < this.genomSize; i++) {
      genom[i] = 3;
    }
    this.cells[this.columns / 2][2] = new Cell(genom, this.minChildEnergy * 3, 2);

    this.cellSideLength = min(width / this.columns, height / this.rows);

    this.actedCells = new ArrayList<Cell>();

    this.ticksNumber = 0;
    this.daysNumber = 0;
    this.yearsNumber = 0;
  }


  public void act() {
    for (int c = 0; c < this.columns; c++) {
      for (int r = 0; r < this.rows; r++) {
        Cell cell = this.cells[c][r];

        if (cell == null || this.actedCells.contains(cell)) {
          continue;
        }

        if (!cell.isAlive) {
          this.move(cell, c, r);
          continue;
        }

        cell.energy--;
        if (cell.energy <= 0) {
          this.cells[c][r] = null;
          continue;
        }
        if (cell.energy >= this.maxEnergy - 1) {
          this.bud(cell, c, r);
          continue;
        }

        if (random(1) < this.randomMutationChance) {
          this.mutateRandomGenomCell(cell);
        }

        int action = cell.genom[cell.counter];
        switch (action) {
          case 1:
            this.turn(cell);
            this.incrementGenomCounter(cell);
            break;
          case 2:
            this.move(cell, c, r);
            this.incrementGenomCounter(cell);
            break;
          case 3:
            this.getEnergyFromPhotosynthesis(cell, c, r);
            this.incrementGenomCounter(cell);
            break;
          case 4:
            this.getEnergyFromMinerals(cell, c, r);
            this.incrementGenomCounter(cell);
            break;
          case 5:
            this.getEnergyFromFood(cell, c, r);
            this.incrementGenomCounter(cell);
            break;
          case 6:
            this.bud(cell, c, r);
            this.incrementGenomCounter(cell);
            break;
          case 7:
            this.mutateRandomGenomCell(cell);
            this.incrementGenomCounter(cell);
            break;
          case 8:
            this.jmpToGenomCell(cell);
            break;
          default:
            this.addToCounter(cell);
            break;
        }
      }
    }
    this.actedCells.clear();

    this.calculateTime();
  }


  public void calculateTime() {
    this.ticksNumber++;

    if (this.ticksNumber == this.dayDurationInTicks) {
      this.ticksNumber = 0;
      this.daysNumber++;

      if (this.daysNumber == this.seasonDurationInDays * 4) {
        this.daysNumber = 0;
        this.yearsNumber++;
      }
    }
  }

  public float calculateTransparencyCoefficient(int column, int row) {
    float sumOfTransparencyCoefficients = 0;
    final float upperCornerTransparencyCoefficient = cos(PI / 4), sideTransparencyCoefficient = cos(PI / 3);
    final float maxTransparencyCoefficients = 1 + 2 / upperCornerTransparencyCoefficient + 2 / sideTransparencyCoefficient;

    int[] targetCoordinates;
    targetCoordinates = this.calculateCoordinatesByDirection(column, row, 0);
    if (this.cells[targetCoordinates[0]][targetCoordinates[1]] == null) {
      sumOfTransparencyCoefficients += 1;
    }
    targetCoordinates = this.calculateCoordinatesByDirection(column, row, 1);
    if (this.cells[targetCoordinates[0]][targetCoordinates[1]] == null) {
      sumOfTransparencyCoefficients += 1 / upperCornerTransparencyCoefficient;
    }
    targetCoordinates = this.calculateCoordinatesByDirection(column, row, 7);
    if (this.cells[targetCoordinates[0]][targetCoordinates[1]] == null) {
      sumOfTransparencyCoefficients += 1 / upperCornerTransparencyCoefficient;
    }
    targetCoordinates = this.calculateCoordinatesByDirection(column, row, 2);
    if (this.cells[targetCoordinates[0]][targetCoordinates[1]] == null) {
      sumOfTransparencyCoefficients += 1 / sideTransparencyCoefficient;
    }
    targetCoordinates = this.calculateCoordinatesByDirection(column, row, 6);
    if (this.cells[targetCoordinates[0]][targetCoordinates[1]] == null) {
      sumOfTransparencyCoefficients += 1 / sideTransparencyCoefficient;
    }

    return sumOfTransparencyCoefficients / maxTransparencyCoefficients;
  }

  public float calculateSeasonCoefficient() {
    return 1.25 + 0.75 * sin(map((float)this.daysNumber / this.seasonDurationInDays, 0, 4, 0, 2 * PI));
  }

  public float calculateDayCoefficient(int column, int row) {
    float noonPositionX = map(this.ticksNumber, 0, this.dayDurationInTicks - 1, 0, this.columns - 1);
    float possibleNoonDistanceX = abs(noonPositionX - column);
    float noonDistanceX = min(possibleNoonDistanceX, this.columns - possibleNoonDistanceX);
    float noonDistance = sqrt(noonDistanceX * noonDistanceX + row * row);
    return pow(map(noonDistance, 0, (this.columns - 1) / 2, 1, 0), 2);
  }


  public int[] calculateCoordinatesByDirection(int column, int row, int direction) {
    int c = (column + this.DIRECTIONS[direction][0] + this.columns) % this.columns;

    int r = row + this.DIRECTIONS[direction][1];
    if (r > this.rows - 1) {
      r = this.rows - 1;
    } else if (r < 0) {
      r = 0;
    }

    return new int[]{c, r};
  }

  public int getNextNthGenomCell(Cell cell, int n) {
    return cell.genom[(cell.counter + n) % this.genomSize];
  }

  public void incrementGenomCounter(Cell cell) {
    cell.counter = (cell.counter + 1) % this.genomSize;
  }

  public void turn(Cell cell) {
    cell.direction = (cell.direction + this.getNextNthGenomCell(cell, 1)) % 8;
  }

  public void move(Cell cell, int column, int row) {
    int[] targetCoordinates = this.calculateCoordinatesByDirection(column, row, cell.direction);
    if (this.cells[targetCoordinates[0]][targetCoordinates[1]] == null) {
      this.cells[targetCoordinates[0]][targetCoordinates[1]] = cell;
      this.cells[column][row] = null;
    }

    this.actedCells.add(cell);
  }

  public void getEnergyFromPhotosynthesis(Cell cell, int column, int row) {
    int deltaEnergy = round(map(row, 0, this.maxPhotosynthesisDepth, this.maxPhotosynthesisEnergy, 0));
    deltaEnergy *= this.calculateTransparencyCoefficient(column, row);
    deltaEnergy *= this.calculateSeasonCoefficient();
    deltaEnergy *= this.calculateDayCoefficient(column, row);

    if (deltaEnergy > 0) {
      cell.energy += deltaEnergy;
      if (cell.energy > this.maxEnergy) {
        cell.energy = this.maxEnergy;
      }

      cell.colorG++;
    }
  }

  public void getEnergyFromMinerals(Cell cell, int column, int row) {
    int deltaEnergy = round(map(row, this.rows - 1, this.rows - 1 - this.maxMineralDepth, this.maxMineralEnergy, 0));

    if (deltaEnergy > 0) {
      cell.energy += deltaEnergy;
      if (cell.energy > this.maxEnergy) {
        cell.energy = this.maxEnergy;
      }

      cell.colorB++;
    }
  }

  public void getEnergyFromFood(Cell cell, int column, int row) {
    int[] targetCoordinates = this.calculateCoordinatesByDirection(column, row, cell.direction);
    if (this.cells[targetCoordinates[0]][targetCoordinates[1]] != null) {
      cell.energy += this.cells[targetCoordinates[0]][targetCoordinates[1]].energy * foodEfficiency;
      if (cell.energy > this.maxEnergy) {
        cell.energy = this.maxEnergy;
      }

      cell.colorR++;

      this.cells[targetCoordinates[0]][targetCoordinates[1]] = null;
    }
  }

  public void bud(Cell cell, int column, int row) {
    int[] targetCoordinates;

    if (cell.energy < this.minChildEnergy * 2) {
      return;
    }

    for (int i = 0; i < 8; i++) {
      targetCoordinates = this.calculateCoordinatesByDirection(column, row, (cell.direction + i) % 8);

      if (this.cells[targetCoordinates[0]][targetCoordinates[1]] == null) {
        this.cells[targetCoordinates[0]][targetCoordinates[1]] = new Cell(cell.genom, cell.energy / 2, cell.direction);
        if (random(1) < this.childMutationChance) {
          this.mutateRandomGenomCell(this.cells[targetCoordinates[0]][targetCoordinates[1]]);
        }
        this.actedCells.add(this.cells[targetCoordinates[0]][targetCoordinates[1]]);

        if (random(1) < this.parentMutationChance) {
          this.mutateRandomGenomCell(cell);
        }
        cell.energy -= cell.energy / 2;

        return;
      }
    }

    this.turnIntoFood(cell, column, row);
  }

  public void mutateRandomGenomCell(Cell cell) {
    cell.genom[floor(random(this.genomSize))] = floor(random(this.genomSize));
  }

  public void jmpToGenomCell(Cell cell) {
    cell.counter = this.getNextNthGenomCell(cell, 1);
  }

  public void addToCounter(Cell cell) {
    cell.counter = (cell.counter + cell.genom[cell.counter]) % this.genomSize;
  }

  public void turnIntoFood(Cell cell, int column, int row) {
    cell.isAlive = false;
    cell.direction = 4;
  }


  public void render() {
    this.renderBackground();

    for (int c = 0; c < this.columns; c++) {
      for (int r = 0; r < this.rows; r++) {
        Cell cell = this.cells[c][r];

        if (cell != null) {
          this.renderCell(cell, c, r);
        }
      }
    }
  }

  public void renderBackground() {
    noStroke();
    fill(255);
    rect(0, 0, this.columns * this.cellSideLength - 1, this.rows * this.cellSideLength - 1);
    noFill();

    for (int x = 0; x < this.columns; x++) {
      for (int y = 0; y <= this.maxPhotosynthesisDepth; y++) {
        float a = map(y, 0, this.maxPhotosynthesisDepth, 127, 0);
        a *= this.calculateSeasonCoefficient();
        a *= this.calculateDayCoefficient(x, y);

        noStroke();
        fill(255, 255, 0, a);
        rectMode(CORNER);
        rect(x * this.cellSideLength, y * this.cellSideLength, this.cellSideLength, this.cellSideLength);
        noFill();
      }
      for (int y = this.rows - 1 - this.maxMineralDepth; y <= this.rows - 1; y++) {
        float a = map(y, this.rows - 1, this.rows - 1 - this.maxMineralDepth, 127, 0);

        noStroke();
        fill(0, 0, 255, a);
        rectMode(CORNER);
        rect(x * this.cellSideLength, y * this.cellSideLength, this.cellSideLength, this.cellSideLength);
        noFill();
      }
    }
  }

  public void renderCell(Cell cell, int column, int row) {
    noStroke();

    if (cell.isAlive) {
      float colorR = cell.colorR, colorG = cell.colorG, colorB = cell.colorB;
      float colorVectorLength = sqrt(colorR * colorR + colorG * colorG + colorB * colorB);
      colorR *= 255 / colorVectorLength;
      colorG *= 255 / colorVectorLength;
      colorB *= 255 / colorVectorLength;

      fill(colorR, colorG, colorB);
    } else {
      fill(191);
    }

    rectMode(CORNER);
    rect(column * this.cellSideLength, row * this.cellSideLength, this.cellSideLength, this.cellSideLength);
    noFill();
  }
}
