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
  public long seed = 1234567890;

  // Space dimensions
  public int rows    = height / 4;
  public int columns = width / 4;

  // Genom machine and environment properties
  public int genomSize                      = 64;
  public int maxInstructionsPerTick         = 16;
  public int minChildEnergy                 = 40;
  public int maxEnergy                      = 800;
  public int maxPhotosynthesisEnergy        = 30;
  public int maxPhotosynthesisDepth         = 500 / 4;
  public float winterDaytimeToWholeDayRatio = 0.4f;
  public float baseTransparencyCoefficient  = 1f;
  public int maxMineralEnergy               = 15;
  public int maxMineralHeight               = 500 / 4;
  public int maxFoodEnergy                  = 50;
  public float foodEfficiency               = 0.95f;
  public float budEfficiency                = 0.95f;
  public float randomMutationChance         = 0.02f;
  public float budMutationChance            = 0.05f;
  public int dayDurationInTicks             = 240;
  public int seasonDurationInDays           = 92;
  public float gammaFlashPeriodInDays       = 720f;
  public float gammaFlashMaxMutationsCount  = 3f;

  // Linked list of cells for quick consequent access
  private LinkedList<Cell> cells;
  // Two-dimensional array of same cells for quick access by coordinates
  private Cell[][]         cellsByCoords;

  // World time in ticks
  private int ticksNumber;
  private int daysNumber;
  private int yearsNumber;

  // Rendering properties
  public boolean drawBackground = true;
  private float cellSideLength;
  private float renderWidth;
  private float renderHeight;

  // Photosynthesis and mineral energy buffers for optimization
  private float[][] surgeOfPhotosynthesisEnergy;
  private float[] surgeOfMineralEnergy;

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

    this.surgeOfPhotosynthesisEnergy = new float[this.columns][this.maxPhotosynthesisDepth];
    this.surgeOfMineralEnergy        = new float[this.maxMineralHeight];
  }

  public void act() {
    // Updating world time
    this.updateTime();

    // Updating ppotosynthesis and mineral energy buffers
    this.updateEnergy();

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
          // Do nothing
          case 0:
            this.incrementGenomCounter(cell);

            break;
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
          // Transferring energy (no more instructions permitted)
          case 8:
            this.transferEnergy(cell);
            this.incrementGenomCounter(cell);

            i = this.maxInstructionsPerTick;

            break;
          // Looking forward (conditional instruction)
          case 9:
            this.lookForward(cell);

            break;
          // Determining own energy level (conditional instruction)
          case 10:
            this.determineEnergyLevel(cell);

            break;
          // Determining own depth (conditional instruction)
          case 11:
            this.determineDepth(cell);

            break;
          // Determining energy surge from photosynthesis (conditional instruction)
          case 12:
            this.determinePhotosynthesisEnergy(cell);

            break;
          // Determining energy surge from minerals (conditional instruction)
          case 13:
            this.determineMineralEnergy(cell);

            break;
          // Determining functional organization (conditional instruction)
          case 14:
            this.determineFunctionalOrganization(cell);

            break;
          // Unconditional jump
          default:
            this.addGenToCounter(cell);

            break;
        }
      }
    }

    // Performing gamma flash
    this.gammaFlash();
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

  private void updateEnergy() {
    // Calculating photosynthesis energy
    for (int x = 0; x < this.columns; ++x) {
      int y = 0;

      // Filling with values until they are low
      while (y < this.maxPhotosynthesisDepth) {
        this.surgeOfPhotosynthesisEnergy[x][y] = this.calculatePhotosynthesisEnergy(x, y);

        // Break if photosynthesis energy is almost zero so underneath cells have no energy at all
        if (this.surgeOfPhotosynthesisEnergy[x][y] < 1f) {
          break;
        }

        ++y;
      }

      // Filling with zeroes
      while (y < this.maxPhotosynthesisDepth) {
        this.surgeOfPhotosynthesisEnergy[x][y] = 0f;

        ++y;
      }
    }

    // Calculating mineral energy
    for (int y = this.rows - this.maxMineralHeight; y < this.rows; ++y) {
      this.surgeOfMineralEnergy[this.rows - 1 - y] = this.calculateMineralEnergy(y);
    }
  }

  private void gammaFlash() {
    // If the time has come
    if (this.ticksNumber == 0 && (this.daysNumber + this.yearsNumber * this.seasonDurationInDays * 4) % this.gammaFlashPeriodInDays == 0) {
      // For each cell
      LinkedList<Cell>.ListIterator iter = this.cells.listIterator();
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

  private void turn(Cell cell) {
    // Updating direction with overflow handling
    int deltaDirection = this.getNextNthGen(cell, 1);
    cell.direction     = (cell.direction + deltaDirection) % 8;
  }

  private void move(Cell cell) {
    // Calculating coordinates by target direction
    int targetDirection;
    if (cell.isAlive) {
      int deltaDirection = this.getNextNthGen(cell, 1);
      targetDirection = (cell.direction + deltaDirection) % 8;
    } else {
      // Setting direction to 4 so it will be moving down (sinking) each step if cell is dead
      targetDirection = 4;
    }
    int[] targetCoordinates = this.calculateCoordinatesByDirection(cell.posX, cell.posY, targetDirection);

    // If coordinates are beyond simulation world (above top or below bottom)
    if (targetCoordinates[1] == -1) {
      return;
    }

    // If there is nothing at this direction
    if (this.cellsByCoords[targetCoordinates[0]][targetCoordinates[1]] == null) {
      this.cellsByCoords[targetCoordinates[0]][targetCoordinates[1]] = cell;
      this.cellsByCoords[cell.posX][cell.posY]                       = null;

      cell.posX = targetCoordinates[0];
      cell.posY = targetCoordinates[1];
    }
  }

  private void getEnergyFromPhotosynthesis(Cell cell) {
    // Calculating energy from photosynthesis at current position
    int deltaEnergy = (int)getPhotosynthesisEnergy(cell.posX, cell.posY, true);

    // If energy from photosynthesis is positive
    if (deltaEnergy > 0) {
      // Increasing energy level
      cell.energy += deltaEnergy;

      // Making cell color more green
      ++cell.colorG;
    }
  }

  private void getEnergyFromMinerals(Cell cell) {
    // Calculating energy from minerals at current position
    int deltaEnergy = (int)getMineralEnergy(cell.posY);

    // If energy from minerals is positive
    if (deltaEnergy > 0) {
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

    // If there is a cell or food
    if (targetCell != null) {
      // Calculating energy from food
      int deltaEnergy = (int)(targetCell.energy * foodEfficiency);
      if (deltaEnergy > this.maxFoodEnergy) {
        deltaEnergy = this.maxFoodEnergy;
      }

      // Increasing energy level
      cell.energy += deltaEnergy;

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

      // If there is nothing at this direction
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

  private void transferEnergy(Cell cell) {
    // Calculating coordinates by current direction
    int[] targetCoordinates = this.calculateCoordinatesByDirection(cell.posX, cell.posY, cell.direction);

    // If coordinates are beyond simulation world (above top or below bottom)
    if (targetCoordinates[1] == -1) {
      return;
    }

    // Getting cell at this direction
    Cell targetCell = this.cellsByCoords[targetCoordinates[0]][targetCoordinates[1]];

    // If there is a cell
    if (targetCell != null && targetCell.isAlive) {
      // Calculating shared energy
      int deltaEnergy = (int)(cell.energy * this.getNextNthGen(cell, 1) / (float)this.genomSize);

      // Sharing energy
      cell.energy       -= deltaEnergy;
      targetCell.energy += deltaEnergy;
    }
  }

  private void lookForward(Cell cell) {
    // Calculating coordinates by current direction
    int[] targetCoordinates = this.calculateCoordinatesByDirection(cell.posX, cell.posY, cell.direction);

    // If coordinates are beyond simulation world (above top or below bottom)
    if (targetCoordinates[1] == -1) {
      return;
    }

    // Getting cell at this direction
    Cell targetCell = this.cellsByCoords[targetCoordinates[0]][targetCoordinates[1]];

    // If there is a cell or food
    if (targetCell != null) {
      // If there is a live cell
      if (targetCell.isAlive) {
        this.jumpCounter(cell, this.getNextNthGen(cell, 3));
      }
      // If there is food
      else {
        this.jumpCounter(cell, this.getNextNthGen(cell, 2));
      }
    }
    // If there is nothing at this direction
    else {
      this.jumpCounter(cell, this.getNextNthGen(cell, 1));
    }
  }

  private void determineEnergyLevel(Cell cell) {
    // Calculating value to compare
    int valueToCompare = (int)(this.maxEnergy * this.getNextNthGen(cell, 1) / (float)this.genomSize);

    // Less
    if (cell.energy < valueToCompare) {
      this.jumpCounter(cell, this.getNextNthGen(cell, 2));
    }
    // Greater
    else if (cell.energy > valueToCompare) {
      this.jumpCounter(cell, this.getNextNthGen(cell, 4));
    }
    // Equal
    else {
      this.jumpCounter(cell, this.getNextNthGen(cell, 3));
    }
  }

  private void determineDepth(Cell cell) {
    // Calculating value to compare
    int valueToCompare = (int)(this.rows * this.getNextNthGen(cell, 1) / (float)this.genomSize);

    // Less
    if (cell.posY < valueToCompare) {
      this.jumpCounter(cell, this.getNextNthGen(cell, 2));
    }
    // Greater
    else if (cell.posY > valueToCompare) {
      this.jumpCounter(cell, this.getNextNthGen(cell, 4));
    }
    // Equal
    else {
      this.jumpCounter(cell, this.getNextNthGen(cell, 3));
    }
  }

  private void determinePhotosynthesisEnergy(Cell cell) {
    // Calculating value to compare
    int valueToCompare = (int)(this.maxPhotosynthesisEnergy * this.getNextNthGen(cell, 1) / (float)this.genomSize);

    // Calculating surge of photosynthesis energy
    int surgeOfEnergy = (int)getPhotosynthesisEnergy(cell.posX, cell.posY, true);

    // Less
    if (surgeOfEnergy < valueToCompare) {
      this.jumpCounter(cell, this.getNextNthGen(cell, 2));
    }
    // Greater
    else if (surgeOfEnergy > valueToCompare) {
      this.jumpCounter(cell, this.getNextNthGen(cell, 4));
    }
    // Equal
    else {
      this.jumpCounter(cell, this.getNextNthGen(cell, 3));
    }
  }

  private void determineMineralEnergy(Cell cell) {
    // Calculating value to compare
    int valueToCompare = (int)(this.maxMineralEnergy * this.getNextNthGen(cell, 1) / (float)this.genomSize);

    // Calculating surge of photosynthesis energy
    int surgeOfEnergy = (int)getMineralEnergy(cell.posY);

    // Less
    if (surgeOfEnergy < valueToCompare) {
      this.jumpCounter(cell, this.getNextNthGen(cell, 2));
    }
    // Greater
    else if (surgeOfEnergy > valueToCompare) {
      this.jumpCounter(cell, this.getNextNthGen(cell, 4));
    }
    // Equal
    else {
      this.jumpCounter(cell, this.getNextNthGen(cell, 3));
    }
  }

  private void determineFunctionalOrganization(Cell cell) {
    this.incrementGenomCounter(cell);
  }

  private void incrementGenomCounter(Cell cell) {
    // Incrementing instruction counter with overflow handling
    cell.counter = (cell.counter + 1) % this.genomSize;
  }

  private void addGenToCounter(Cell cell) {
    // Adding dummy instruction value to instruction counter with overflow handling
    cell.counter = (cell.counter + cell.genom[cell.counter]) % this.genomSize;
  }

  private void jumpCounter(Cell cell, int offset) {
    // Performing jump command on instruction counter with overflow handling
    cell.counter = (cell.counter + offset) % this.genomSize;
  }

  private int getNextNthGen(Cell cell, int n) {
    // Getting (counter + n)'th gen
    return cell.genom[(cell.counter + n) % this.genomSize];
  }

  private void turnIntoFood(Cell cell) {
    cell.isAlive = false;
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

  private float getPhotosynthesisEnergy(int column, int row, boolean calculateTransparency) {
    // If depth is greater than maximal
    if (row >= this.maxPhotosynthesisDepth) {
      return 0f;
    }

    float energy = this.surgeOfPhotosynthesisEnergy[column][row];

    if (calculateTransparency) {
      // Stop if photosynthesis energy is already not enough to feed a cell
      if (energy < 1f) {
        return 0f;
      }

      // Applying transparency coefficient
      energy *= this.calculateTransparencyCoefficient(column, row);
    }

    return energy;
  }

  private float getMineralEnergy(int row) {
    // If depth is less than minimal
    if (row < this.rows - this.maxMineralHeight) {
      return 0f;
    }

    return this.surgeOfMineralEnergy[this.rows - 1 - row];
  }

  private float calculatePhotosynthesisEnergy(int column, int row) {
    // Getting base energy from photosynthesis
    float energy = map(
      row,
      0f,
      this.maxPhotosynthesisDepth,
      this.maxPhotosynthesisEnergy,
      0f
    );

    // Stop if photosynthesis energy is already not enough to feed a cell
    if (energy < 1f) {
      return 0f;
    }

    // Applying day coefficient
    energy *= this.calculateDayCoefficient(column, row, energy);

    return energy;
  }

  private float calculateMineralEnergy(int row) {
    // Getting energy from minerals
    return map(
      row,
      this.rows - 1f,
      this.rows - 1f - this.maxMineralHeight,
      this.maxMineralEnergy,
      0f
    );
  }

  private float calculateDayCoefficient(int column, int row, float energyToCompare) {
    // Calculating day coefficient by Y-axis
    float dayCoefficientY = map(
      row,
      0f,
      this.maxPhotosynthesisDepth,
      1f,
      0f
    );

    // Stop if photosynthesis energy is already not enough to feed a cell
    if (dayCoefficientY * energyToCompare < 1f) {
      return 0f;
    }

    // Calculating noon (sun) position on X-axis
    float noonPositionX = map(
      this.ticksNumber,
      0f,
      this.dayDurationInTicks - 1f,
      0f,
      this.columns - 1f
    );
    // Calculating smaller ot larger distance to noon on X-axis
    float possibleNoonDistanceX = abs(noonPositionX - column);
    // Calculating smaller distance to noon on X-axis
    float noonDistanceX = min(possibleNoonDistanceX, this.columns - possibleNoonDistanceX);
    // Calculating season coefficient
    float seasonCoefficient = sin(
      map(
        (float)this.daysNumber / this.seasonDurationInDays,
        0f,
        4f,
        0f,
        2f * PI
     )
    );
    seasonCoefficient = seasonCoefficient * 0.25f + 0.25f;
    // Calculating daytime to the whole day ratio
    float daytimeToWholeDayRatio = this.winterDaytimeToWholeDayRatio + seasonCoefficient;
    daytimeToWholeDayRatio -= (int)daytimeToWholeDayRatio;
    // Calculating day coefficient by X-axis
    float dayCoefficientX = map(
      noonDistanceX,
      0f,
      (this.columns - 1f) * daytimeToWholeDayRatio,
      1f,
      0f
    );

    // Calculating day coefficient as product of coefficients by X and Y axies
    return dayCoefficientX * dayCoefficientY;
  }

  private float calculateTransparencyCoefficient(int column, int row) {
    float sumOfTransparencyCoefficients = this.baseTransparencyCoefficient;

    float topTransparency       = 1f;
    float topCornerTransparency = 1f / cos(PI / 4f);
    float sideTransparency      = 1f / cos(PI / 3f);

    float maxTransparencyCoefficient = this.baseTransparencyCoefficient + 1f * topTransparency + 2f * topCornerTransparency + 2f * sideTransparency;

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

    // Calculating transparency coefficient adding all coefficients and dividing by maximum of their sum
    return sumOfTransparencyCoefficients / maxTransparencyCoefficient;
  }

  private void addCell(Cell cell) {
    // Pushing cell to the front of the linked list
    // so it will be processed not earlier than the next step
    // and before older cells (younger cells have smaller "reaction time")
    this.cells.pushFront(cell);
    this.cellsByCoords[cell.posX][cell.posY] = cell;
  }

  private void removeCell(Cell cell) {
    this.cells.remove(cell);
    this.cellsByCoords[cell.posX][cell.posY] = null;
  }

  public void render() {
    // Rendering background
    if (this.drawBackground) {
      this.renderBackground();
    } else {
      background(255f);
    }

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

    // Rendering minerals energy density
    for (int y = this.rows - this.maxMineralHeight; y < this.rows; ++y) {
      float colorA = map(this.getMineralEnergy(y), 0f, this.maxMineralEnergy, 0f, 127f);

      fill(0f, 0f, 255f, colorA);
      rect(0f, y * this.cellSideLength, width, this.cellSideLength);
      noFill();
    }

    // Rendering photosynthesis energy density
    for (int x = 0; x < this.columns; ++x) {
      for (int y = 0; y < this.maxPhotosynthesisDepth; ++y) {
        float colorA = map(this.getPhotosynthesisEnergy(x, y, false), 0f, this.maxPhotosynthesisEnergy, 0f, 127f);

        // Break if photosynthesis energy is almost zero so underneath cells have no energy at all
        if (colorA < 1f) {
          break;
        }

        fill(255f, 255f, 0f, colorA);
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
