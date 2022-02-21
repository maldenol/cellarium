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

// Position offsets per direction
final int[][] DIRECTIONS = {
  {0, -1},
  {1, -1},
  {1, 0},
  {1, 1},
  {0, 1},
  {-1, 1},
  {-1, 0},
  {-1, -1},
};

class CellController {
  // Random seed
  private long seed = 1234567890;

  // Space dimensions
  private int rows    = height / 4;
  private int columns = width / 4;

  // Genom machine and environment properties
  private int genomSize                     = 64;
  private int maxInstructionsPerTick        = 16;
  private int minChildEnergy                = 40;
  private int maxEnergy                     = 800;
  private int dayDurationInTicks            = 240;
  private int seasonDurationInDays          = 92;
  private int maxPhotosynthesisEnergy       = 30;
  private int maxPhotosynthesisDepth        = 750;
  private int maxMineralEnergy              = 15;
  private int maxMineralDepth               = 500;
  private float budEfficiency               = 0.95f;
  private float foodEfficiency              = 0.95f;
  private float randomMutationChance        = 0.002f;
  private float budMutationChance           = 0.05f;
  private float gammaFlashPeriodInDays      = 720f;
  private float gammaFlashMaxMutationsCount = 3f;

  // Linked list of cells for quick consequent access
  private LinkedList<Cell> cells;
  // Two-dimensional array of same cells for quick access by coordinates
  private Cell[][]         cellsByCoords;

  // World time in ticks
  private int ticksNumber;
  private int daysNumber;
  private int yearsNumber;

  // Rendering properties
  private float cellSideLength;
  private float renderWidth;
  private float renderHeight;

  public CellController() {
    randomSeed(this.seed);

    this.cells         = new LinkedList<Cell>();
    this.cellsByCoords = new Cell[this.columns][this.rows];

    int[] genom = new int[this.genomSize];
    for (int i = 0; i < this.genomSize; ++i) {
      genom[i] = 3;
    }
    this.addCell(new Cell(
        genom,
        this.minChildEnergy,
        2,
        this.columns / 2,
        2
      )
    );

    this.ticksNumber = 0;
    this.daysNumber  = 0;
    this.yearsNumber = 0;

    this.cellSideLength = min(width / this.columns, height / this.rows);
    this.renderWidth    = this.columns * this.cellSideLength - 1f;
    this.renderHeight   = this.rows * this.cellSideLength - 1f;
  }

  public void act() {
    // Updating world time
    this.updateTime();

    // Going through all cells sequently
    LinkedList<Cell>.ListIterator iter = this.cells.listIterator();
    while (iter.hasNext()) {
      Cell cell = iter.next();

      // Moving cell if it is dead
      if (!cell.isAlive) {
        this.move(cell);
        continue;
      }

      // Updating cell energy
      cell.energy--;
      // Removing cell if its energy is less than one
      if (cell.energy <= 0) {
        this.removeCell(cell);
        continue;
      }
      // Making cell bud if its energy greater or equals to maximal
      if (cell.energy >= this.maxEnergy - 1) {
        this.bud(cell);
        continue;
      }

      // Applying random mutation
      if (random(1f) < this.randomMutationChance) {
        this.mutateRandomGen(cell);
      }

      // Executing genom machine instructions with maximum instructions per tick limit
      for (int i = 0; i < this.maxInstructionsPerTick; ++i) {
        // Getting current instruction from genom
        int instuction = cell.genom[cell.counter];

        // Performing appropriate action
        switch (instuction) {
          // Turning
          case 1:
            this.turn(cell);
            this.incrementGenomCounter(cell);

            break;
          // Moving (no more instructions permitted)
          case 2:
            this.move(cell);
            this.incrementGenomCounter(cell);

            i = this.maxInstructionsPerTick;

            break;
          // Getting energy from photosynthesis (no more instructions permitted)
          case 3:
            this.getEnergyFromPhotosynthesis(cell);
            this.incrementGenomCounter(cell);

            i = this.maxInstructionsPerTick;

            break;
          // Getting energy from minerals (no more instructions permitted)
          case 4:
            this.getEnergyFromMinerals(cell);
            this.incrementGenomCounter(cell);

            i = this.maxInstructionsPerTick;

            break;
          // Getting energy from food (no more instructions permitted)
          case 5:
            this.getEnergyFromFood(cell);
            this.incrementGenomCounter(cell);

            i = this.maxInstructionsPerTick;

            break;
          // Budding (no more instructions permitted)
          case 6:
            this.bud(cell);
            this.incrementGenomCounter(cell);

            i = this.maxInstructionsPerTick;

            break;
          // Making random gen mutate (no more instructions permitted)
          case 7:
            this.mutateRandomGen(cell);

            this.incrementGenomCounter(cell);

            i = this.maxInstructionsPerTick;

            break;
          // Dummy instruction
          default:
            this.addToCounter(cell);

            break;
        }
      }
    }

    // Performing gamma flash if the time has come
    if (this.ticksNumber == 0 && (this.daysNumber + this.yearsNumber * this.seasonDurationInDays * 4) % this.gammaFlashPeriodInDays == 0) {
      // For each cell
      iter = this.cells.listIterator();
      while (iter.hasNext()) {
        Cell cell = iter.next();

        // Ignoring if cell is dead
        if (!cell.isAlive) {
          continue;
        }

        int mutationsCount = ceil(this.gammaFlashMaxMutationsCount * random(1f));
        for (int i = 0; i < mutationsCount; ++i) {
          this.mutateRandomGen(cell);
        }
      }
    }
  }

  private void updateTime() {
    // Updating ticks
    ++this.ticksNumber;

    // Updating ticks and days if ticks overflow
    if (this.ticksNumber == this.dayDurationInTicks) {
      this.ticksNumber = 0;
      ++this.daysNumber;

      // Updating days and years if days overflow
      if (this.daysNumber == this.seasonDurationInDays * 4) {
        this.daysNumber = 0;
        ++this.yearsNumber;
      }
    }
  }

  private float calculateTransparencyCoefficient(int column, int row) {
    float sumOfTransparencyCoefficients = 0f;

    float topTransparency       = 1f;
    float topCornerTransparency = 1f / cos(PI / 4f);
    float sideTransparency      = 1f / cos(PI / 3f);

    float maxTransparencyCoefficient = 1f * topTransparency + 2f * topCornerTransparency + 2f * sideTransparency;

    int[] targetCoordinates;

    // Top coefficient
    targetCoordinates = this.calculateCoordinatesByDirection(column, row, 0);
    if (targetCoordinates[1] == -1 || this.cellsByCoords[targetCoordinates[0]][targetCoordinates[1]] == null) {
      sumOfTransparencyCoefficients += topTransparency;
    }

    // Top-right coefficient
    targetCoordinates = this.calculateCoordinatesByDirection(column, row, 1);
    if (targetCoordinates[1] == -1 || this.cellsByCoords[targetCoordinates[0]][targetCoordinates[1]] == null) {
      sumOfTransparencyCoefficients += topCornerTransparency;
    }

    // Top-left coefficient
    targetCoordinates = this.calculateCoordinatesByDirection(column, row, 7);
    if (targetCoordinates[1] == -1 || this.cellsByCoords[targetCoordinates[0]][targetCoordinates[1]] == null) {
      sumOfTransparencyCoefficients += topCornerTransparency;
    }

    // Right coefficient
    targetCoordinates = this.calculateCoordinatesByDirection(column, row, 2);
    if (this.cellsByCoords[targetCoordinates[0]][targetCoordinates[1]] == null) {
      sumOfTransparencyCoefficients += sideTransparency;
    }

    // Left coefficient
    targetCoordinates = this.calculateCoordinatesByDirection(column, row, 6);
    if (this.cellsByCoords[targetCoordinates[0]][targetCoordinates[1]] == null) {
      sumOfTransparencyCoefficients += sideTransparency;
    }

    // Calculating coefficient adding all coefficients and dividing by maximum of their sum
    float transparencyCoefficient = sumOfTransparencyCoefficients / maxTransparencyCoefficient;

    return transparencyCoefficient;
  }

  private float calculateSeasonCoefficient() {
    // Calculating coefficient in range from -1 to 1
    float seasonCoefficient = sin(
      map(
        (float)this.daysNumber / this.seasonDurationInDays,
        0f,
        4f,
        0f,
        2f * PI
      )
    );
    // Reducing to range from 0.5 to 2
    seasonCoefficient       = seasonCoefficient * 0.75f + 1.25f;

    return seasonCoefficient;
  }

  private float calculateDayCoefficient(int column, int row) {
    // Calculating noon (sun) position on X-axis
    float noonPositionX         = map(
      this.ticksNumber,
      0,
      this.dayDurationInTicks - 1,
      0f,
      this.columns - 1f
    );
    // Calculating smaller ot larger distance to noon on X-axis
    float possibleNoonDistanceX = abs(noonPositionX - column);
    // Calculating smaller distance to noon on X-axis
    float noonDistanceX         = min(possibleNoonDistanceX, this.columns - possibleNoonDistanceX);
    // Calculating smaller distance to noon
    float noonDistance          = sqrt(noonDistanceX * noonDistanceX + row * row);
    // Calculating coefficient
    float dayCoefficient        = pow(
      map(
        noonDistance,
        0,
        (this.columns - 1) / 2,
        1f,
        0f
      ),
    2);

    return dayCoefficient;
  }

  private int calculateEnergyFromPhotosynthesis(int column, int row) {
    // Getting base energy from photosynthesis
    float energy = map(
      row,
      0,
      this.maxPhotosynthesisDepth,
      this.maxPhotosynthesisEnergy,
      0f
    );

    // Applying transparency coefficient
    energy *= this.calculateTransparencyCoefficient(column, row);
    // Applying season coefficient
    energy *= this.calculateSeasonCoefficient();
    // Applying day coefficient
    energy *= this.calculateDayCoefficient(column, row);

    return (int)energy;
  }

  private int calculateEnergyFromMinerals(int row) {
    // Getting energy from minerals
    float energy = map(
      row,
      this.rows - 1,
      this.rows - 1 - this.maxMineralDepth,
      this.maxMineralEnergy,
      0f
    );

    return (int)energy;
  }

  private int[] calculateCoordinatesByDirection(int column, int row, int direction) {
    // Calculating column at given direction with overflow handling
    int c = (column + DIRECTIONS[direction][0] + this.columns) % this.columns;

    // Calculating row at given direction with overflow handling
    int r = row + DIRECTIONS[direction][1];
    if (r > this.rows - 1 || r < 0) {
      r = -1;
    }

    return new int[]{c, r};
  }

  private int getNextNthGenomCell(Cell cell, int n) {
    // Getting (counter + n)'th gen
    return cell.genom[(cell.counter + n) % this.genomSize];
  }

  private void incrementGenomCounter(Cell cell) {
    // Incrementing instruction counter with overflow handling
    cell.counter = (cell.counter + 1) % this.genomSize;
  }

  private void turn(Cell cell) {
    // Updating direction with overflow handling
    int deltaDirection = this.getNextNthGenomCell(cell, 1);
    cell.direction     = (cell.direction + deltaDirection) % 8;
  }

  private void move(Cell cell) {
    // Calculating coordinates by current direction
    int[] targetCoordinates = this.calculateCoordinatesByDirection(cell.posX, cell.posY, cell.direction);

    // If coordinates are beyond simulation world (above top or below bottom)
    if (targetCoordinates[1] == -1) {
      return;
    }

    // If space at this direction is empty
    if (this.cellsByCoords[targetCoordinates[0]][targetCoordinates[1]] == null) {
      this.cellsByCoords[targetCoordinates[0]][targetCoordinates[1]] = cell;
      this.cellsByCoords[cell.posX][cell.posY]                       = null;

      cell.posX = targetCoordinates[0];
      cell.posY = targetCoordinates[1];
    }
  }

  private void getEnergyFromPhotosynthesis(Cell cell) {
    // Calculating energy from photosynthesis at current position
    int deltaEnergy = calculateEnergyFromPhotosynthesis(cell.posX, cell.posY);

    // If energy from photosynthesis is positive
    if (deltaEnergy > 0f) {
      // Increasing energy level
      cell.energy += deltaEnergy;

      // Making cell color more green
      ++cell.colorG;
    }
  }

  private void getEnergyFromMinerals(Cell cell) {
    // Calculating energy from minerals at current position
    int deltaEnergy = calculateEnergyFromMinerals(cell.posY);

    // If energy from minerals is positive
    if (deltaEnergy > 0f) {
      // Increasing energy level
      cell.energy += deltaEnergy;

      // Making cell color more blue
      ++cell.colorB;
    }
  }

  private void getEnergyFromFood(Cell cell) {
    // Calculating coordinates by current direction
    int[] targetCoordinates = this.calculateCoordinatesByDirection(cell.posX, cell.posY, cell.direction);

    // If coordinates are beyond simulation world (above top or below bottom)
    if (targetCoordinates[1] == -1) {
      return;
    }

    // Getting cell at this direction
    Cell targetCell = this.cellsByCoords[targetCoordinates[0]][targetCoordinates[1]];

    // If there is cell or food
    if (targetCell != null) {
      // Increasing energy level
      cell.energy += targetCell.energy * foodEfficiency;

      // Making cell color more red
      ++cell.colorR;

      // Removing prey or food
      this.removeCell(targetCell);
    }
  }

  private void bud(Cell cell) {
    // Checking and updating energy
    if (cell.energy * this.budEfficiency < this.minChildEnergy * 2f) {
      return;
    }
    cell.energy *= this.budEfficiency;

    // Checking each direction clockwise for ability to bud
    for (int i = 0; i < 8; ++i) {
      // Calculating coordinates by current direction
      int[] targetCoordinates = this.calculateCoordinatesByDirection(cell.posX, cell.posY, (cell.direction + i) % 8);

      // If coordinates are beyond simulation world (above top or below bottom)
      if (targetCoordinates[1] == -1) {
        continue;
      }

      // If space at this direction is empty
      if (this.cellsByCoords[targetCoordinates[0]][targetCoordinates[1]] == null) {
        // Creating new cell
        Cell buddedCell = new Cell(cell.genom, cell.energy / 2, cell.direction, targetCoordinates[0], targetCoordinates[1]);

        // Assigning cell color
        float colorVectorLength = sqrt(cell.colorR * cell.colorR + cell.colorG * cell.colorG + cell.colorB * cell.colorB);
        buddedCell.colorR = (int)(cell.colorR * 2f / colorVectorLength);
        buddedCell.colorG = (int)(cell.colorG * 2f / colorVectorLength);
        buddedCell.colorB = (int)(cell.colorB * 2f / colorVectorLength);

        // Applying random bud mutation to the budded cell
        if (random(1f) < this.budMutationChance) {
          this.mutateRandomGen(buddedCell);
        }

        // Applying random bud mutation to current cell
        cell.energy -= cell.energy / 2;
        if (random(1f) < this.budMutationChance) {
          this.mutateRandomGen(cell);
        }

        this.addCell(buddedCell);

        return;
      }
    }

    // Turning cell into food if it have to but not able to bud
    this.turnIntoFood(cell);
  }

  private void mutateRandomGen(Cell cell) {
    // Changing random gen on another random one
    cell.genom[floor(random(this.genomSize))] = floor(random(this.genomSize));
  }

  private void addToCounter(Cell cell) {
    // Adding dummy instruction value to instruction counter with overflow handling
    cell.counter = (cell.counter + cell.genom[cell.counter]) % this.genomSize;
  }

  private void turnIntoFood(Cell cell) {
    cell.isAlive   = false;
    // Setting direction to 4 so it will be moving down (sinking) each step
    cell.direction = 4;
  }

  private void addCell(Cell cell) {
    // Push cell to front of the linked list so it will be processed the next step
    this.cells.pushFront(cell);
    this.cellsByCoords[cell.posX][cell.posY] = cell;
  }

  private void removeCell(Cell cell) {
    this.cells.remove(cell);
    this.cellsByCoords[cell.posX][cell.posY] = null;
  }

  public void render() {
    // Rendering background
    this.renderBackground();

    // Rendering each cell
    LinkedList<Cell>.ListIterator iter = this.cells.listIterator();
    while (iter.hasNext()) {
      Cell cell = iter.next();

      this.renderCell(cell);
    }
  }

  private void renderBackground() {
    // White background
    fill(255f);
    rect(0f, 0f, this.renderWidth, this.renderHeight);
    noFill();

    // Rendering photosynthesis and minerals energy density
    for (int x = 0; x < this.columns; ++x) {
      // Photosynthesis
      for (int y = 0; y <= this.maxPhotosynthesisDepth; ++y) {
        float colorA = map(y, 0, this.maxPhotosynthesisDepth, 127f, 0f);
        colorA *= this.calculateSeasonCoefficient();
        colorA *= this.calculateDayCoefficient(x, y);

        fill(255f, 255f, 0f, colorA);
        rect(x * this.cellSideLength, y * this.cellSideLength, this.cellSideLength, this.cellSideLength);
        noFill();
      }

      // Minerals
      for (int y = this.rows - 1 - this.maxMineralDepth; y < this.rows; ++y) {
        float colorA = map(y, this.rows - 1 - this.maxMineralDepth, this.rows, 0f, 127f);

        fill(0f, 0f, 255f, colorA);
        rect(x * this.cellSideLength, y * this.cellSideLength, this.cellSideLength, this.cellSideLength);
        noFill();
      }
    }
  }

  private void renderCell(Cell cell) {
    float colorR, colorG, colorB;

    // If cell is alive
    if (cell.isAlive) {
      colorR = cell.colorR;
      colorG = cell.colorG;
      colorB = cell.colorB;

      // Normalizing color and reducing it to range from 0 to 255
      float colorVectorLength = sqrt(colorR * colorR + colorG * colorG + colorB * colorB);
      colorR *= 255f / colorVectorLength;
      colorG *= 255f / colorVectorLength;
      colorB *= 255f / colorVectorLength;
    }
    // If cell is dead
    else {
      colorR = 191f;
      colorG = 191f;
      colorB = 191f;
    }

    // Rendering cell
    fill(colorR, colorG, colorB);
    rect(cell.posX * this.cellSideLength, cell.posY * this.cellSideLength, this.cellSideLength, this.cellSideLength);
    noFill();
  }
}
