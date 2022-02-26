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

  // Space scale
  public int scale = 8;

  // Space dimensions
  public int rows    = height / scale;
  public int columns = width / scale;

  // Genom machine and environment properties
  public int genomSize                      = 64;
  public int maxInstructionsPerTick         = 16;
  public int maxAkinGenomDifference         = 4;
  public int minChildEnergy                 = 40;
  public int maxEnergy                      = 800;
  public int maxPhotosynthesisEnergy        = 30;
  public int maxPhotosynthesisDepth         = 700 / scale;
  public float summerDaytimeToWholeDayRatio = 0.6f;
  public boolean enableDaytimes             = true;
  public boolean enableSeasons              = true;
  public int maxMineralEnergy               = 15;
  public int maxMineralHeight               = 700 / scale;
  public int maxFoodEnergy                  = 50;
  public float randomMutationChance         = 0.01f;
  public float budMutationChance            = 0.25f;
  public int dayDurationInTicks             = 240;
  public int seasonDurationInDays           = 92;
  public int gammaFlashPeriodInDays         = 46;
  public int gammaFlashMaxMutationsCount    = 8;

  // Flags for enabling/disabling cell genom instructions and other parameters
  public boolean enableTurnInstruction                          = true;
  public boolean enableMoveInstruction                          = true;
  public boolean enablePhotosynthesisEnergyInstruction          = true;
  public boolean enableMineralEnergyInstruction                 = true;
  public boolean enableFoodEnergyInstruction                    = true;
  public boolean enableBudInstruction                           = true;
  public boolean enableMutateInstruction                        = true;
  public boolean enableShareEnergyInstruction                   = true;
  public boolean enableLookInstruction                          = true;
  public boolean enableDetermineEnergyLevelInstruction          = true;
  public boolean enableDetermineDepthInstruction                = true;
  public boolean enableDeterminePhotosynthesisEnergyInstruction = true;
  public boolean enableDetermineMineralEnergyInstruction        = true;
  public boolean enableDeadCellPinningOnSinking                 = true;

  // Linked list of cells for quick consequent access
  private LinkedList<Cell> cells;
  // Two-dimensional array of same cells for quick access by coordinates
  private Cell[][] cellsByCoords;

  // World time in ticks
  private int ticksNumber;
  private int yearsNumber;

  // Rendering properties
  private float cellSideLength;

  // Photosynthesis and mineral energy buffers for optimization
  private int[][] surgeOfPhotosynthesisEnergy;
  private int[] surgeOfMineralEnergy;
  private boolean drawBackgroundAtCurrentStep;

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
        this.minChildEnergy * 3,
        2,
        this.columns / 2,
        2
      )
    );

    this.ticksNumber = 0;
    this.yearsNumber = 0;

    this.cellSideLength = min(width / this.columns, height / this.rows);

    this.surgeOfPhotosynthesisEnergy = new int[this.columns][this.maxPhotosynthesisDepth];
    this.surgeOfMineralEnergy        = new int[this.maxMineralHeight];
  }

  public void act(boolean drawBackgroundAtCurrentStep) {
    this.drawBackgroundAtCurrentStep = drawBackgroundAtCurrentStep;

    // Updating world time
    this.updateTime();

    // Updating photosynthesis and mineral energy buffers if this step must be drown
    if (this.drawBackgroundAtCurrentStep) {
      this.updateEnergy();
    }

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

      // Fading last energy share
      cell.lastEnergyShare *= 0.99f;

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
            if (this.enableTurnInstruction) {
              this.turn(cell);
            }

            this.incrementGenomCounter(cell);

            break;
          // Moving (no more instructions permitted)
          case 2:
            if (this.enableMoveInstruction) {
              this.move(cell);

              i = this.maxInstructionsPerTick;
            }

            this.incrementGenomCounter(cell);

            break;
          // Getting energy from photosynthesis (no more instructions permitted)
          case 3:
            if (this.enablePhotosynthesisEnergyInstruction) {
              this.getEnergyFromPhotosynthesis(cell);

              i = this.maxInstructionsPerTick;
            }

            this.incrementGenomCounter(cell);

            break;
          // Getting energy from minerals (no more instructions permitted)
          case 4:
            if (this.enableMineralEnergyInstruction) {
              this.getEnergyFromMinerals(cell);

              i = this.maxInstructionsPerTick;
            }

            this.incrementGenomCounter(cell);

            break;
          // Getting energy from food (no more instructions permitted)
          case 5:
            if (this.enableFoodEnergyInstruction) {
              this.getEnergyFromFood(cell);

              i = this.maxInstructionsPerTick;
            }

            this.incrementGenomCounter(cell);

            break;
          // Budding (no more instructions permitted)
          case 6:
            if (this.enableBudInstruction) {
              this.bud(cell);

              i = this.maxInstructionsPerTick;
            }

            this.incrementGenomCounter(cell);

            break;
          // Making random gen mutate (no more instructions permitted)
          case 7:
            if (this.enableMutateInstruction) {
              this.mutateRandomGen(cell);

              i = this.maxInstructionsPerTick;
            }

            this.incrementGenomCounter(cell);

            break;
          // Sharing energy (no more instructions permitted)
          case 8:
            if (this.enableShareEnergyInstruction) {
              this.shareEnergy(cell);

              i = this.maxInstructionsPerTick;
            }

            this.incrementGenomCounter(cell);

            break;
          // Looking forward (conditional instruction)
          case 9:
            if (this.enableLookInstruction) {
              this.lookForward(cell);
            } else {
              this.incrementGenomCounter(cell);
            }

            break;
          // Determining own energy level (conditional instruction)
          case 10:
            if (this.enableDetermineEnergyLevelInstruction) {
              this.determineEnergyLevel(cell);
            } else {
              this.incrementGenomCounter(cell);
            }

            break;
          // Determining own depth (conditional instruction)
          case 11:
            if (this.enableDetermineDepthInstruction) {
              this.determineDepth(cell);
            } else {
              this.incrementGenomCounter(cell);
            }

            break;
          // Determining energy surge from photosynthesis (conditional instruction)
          case 12:
            if (this.enableDeterminePhotosynthesisEnergyInstruction) {
              this.determinePhotosynthesisEnergy(cell);
            } else {
              this.incrementGenomCounter(cell);
            }

            break;
          // Determining energy surge from minerals (conditional instruction)
          case 13:
            if (this.enableDetermineMineralEnergyInstruction) {
              this.determineMineralEnergy(cell);
            } else {
              this.incrementGenomCounter(cell);
            }

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

    // Updating ticks and years if ticks overflow
    if (this.ticksNumber == this.dayDurationInTicks * this.seasonDurationInDays * 4) {
      this.ticksNumber = 0;
      ++this.yearsNumber;
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
        this.surgeOfPhotosynthesisEnergy[x][y] = 0;

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
    if (
      this.ticksNumber % this.dayDurationInTicks == 0 &&
      (this.ticksNumber / this.dayDurationInTicks + this.yearsNumber * this.seasonDurationInDays * 4) % this.gammaFlashPeriodInDays == 0
    ) {
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
    // If given cell is alive
    if (cell.isAlive) {
      int deltaDirection = this.getNextNthGen(cell, 1);
      targetDirection = (cell.direction + deltaDirection) % 8;
    }
    // If given cell is dead
    else {
      // If given cell is pinned
      if (enableDeadCellPinningOnSinking && cell.pinned) {
        return;
      }

      // Setting direction to 4 so it will be moving down (sinking) each step if cell is dead and not pinned yet
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
    // If there is an obstalce and given cell is dead
    else if (!cell.isAlive) {
      cell.pinned = true;
    }
  }

  private void getEnergyFromPhotosynthesis(Cell cell) {
    // Calculating energy from photosynthesis at current position
    int deltaEnergy = (int)getPhotosynthesisEnergy(cell.posX, cell.posY);

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
      int deltaEnergy = min(targetCell.energy, this.maxFoodEnergy);

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
    if (cell.energy < this.minChildEnergy * 2f) {
      return;
    }

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

  private void shareEnergy(Cell cell) {
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
      // Calculating energy to share
      int deltaEnergy = (int)(cell.energy * this.getNextNthGen(cell, 1) / (float)this.genomSize);

      // Sharing energy
      cell.energy       -= deltaEnergy;
      targetCell.energy += deltaEnergy;

      // Updating energy share balances
      cell.energyShareBalance       += deltaEnergy;
      targetCell.energyShareBalance -= deltaEnergy;

      // Updating last energy shares
      cell.lastEnergyShare       = 1f;
      targetCell.lastEnergyShare = -1f;
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
      // If it is a live cell
      if (targetCell.isAlive) {
        // If it is an akin cell
        if (this.areAkin(cell, targetCell)) {
          this.jumpCounter(cell, this.getNextNthGen(cell, 3));
        }
        // If it is a strange cell
        else {
          this.jumpCounter(cell, this.getNextNthGen(cell, 4));
        }
      }
      // If it is food
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
    int surgeOfEnergy = (int)getPhotosynthesisEnergy(cell.posX, cell.posY);

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

  private boolean areAkin(Cell a, Cell b) {
    int diff = 0;

    for (int i = 0; i < this.genomSize; ++i) {
      if (a.genom[i] != b.genom[i]) {
        ++diff;
        if (diff > this.maxAkinGenomDifference) {
          return false;
        }
      }
    }

    return true;
  }

  private int getPhotosynthesisEnergy(int column, int row) {
    // If depth is greater than maximal
    if (row >= this.maxPhotosynthesisDepth) {
      return 0;
    }

    int energy;

    // If this step must not be drown
    if (!this.drawBackgroundAtCurrentStep) {
      energy = this.calculatePhotosynthesisEnergy(column, row);
    }
    // If this step must be drown
    else {
      energy = this.surgeOfPhotosynthesisEnergy[column][row];
    }

    return energy;
  }

  private int getMineralEnergy(int row) {
    // If depth is less than minimal
    if (row < this.rows - this.maxMineralHeight) {
      return 0;
    }

    // If this step must not be drown
    if (!this.drawBackgroundAtCurrentStep) {
      return this.calculateMineralEnergy(row);
    }

    return this.surgeOfMineralEnergy[this.rows - 1 - row];
  }

  private int calculatePhotosynthesisEnergy(int column, int row) {
    // Calculating daytime coefficient if needed
    float daytimeCoefficient = 1f;
    if (this.enableDaytimes) {
      // Calculation sun epicenter position along X-axis
      int sunPosition = (int)map(
        this.ticksNumber % this.dayDurationInTicks,
        0f,
        this.dayDurationInTicks - 1f,
        0f,
        this.columns - 1f
      );
      // Calculation minimal or maximal distance to sun epicenter along X-axis
      int distanceToSun = abs(sunPosition - column);
      // Calculation minimal distance to sun epicenter along X-axis
      int minDistanceToSun = min(distanceToSun, this.columns - 1 - distanceToSun);
      // Calculation sunny area width based on current season if needed
      float daytimeWidthRatio = 1f - this.summerDaytimeToWholeDayRatio;
      if (this.enableSeasons) {
        daytimeWidthRatio = map(
          sin(
            map(
              this.ticksNumber / this.dayDurationInTicks / this.seasonDurationInDays,
              0f,
              4f,
              0f,
              TWO_PI
            )
          ),
          -1f,
          1f,
          1f - this.summerDaytimeToWholeDayRatio,
          this.summerDaytimeToWholeDayRatio
        );
      }
      // Calculating daytime coefficient
      daytimeCoefficient = (minDistanceToSun < (this.columns - 1) / 2f * daytimeWidthRatio) ? 1f : 0f;

      // If it is night here
      if (daytimeCoefficient < 1f) {
        return 0;
      }
    }

    // Calculating depth coefficient
    float depthCoefficient = map(
      row,
      0f,
      this.maxPhotosynthesisDepth,
      1f,
      0f
    );

    // Getting energy from photosynthesis
    return (int)(this.maxPhotosynthesisEnergy * depthCoefficient * daytimeCoefficient);
  }

  private int calculateMineralEnergy(int row) {
    // Getting energy from minerals
    return (int)map(
      row,
      this.rows - 1f,
      this.rows - 1f - this.maxMineralHeight,
      this.maxMineralEnergy,
      0f
    );
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

  public void render(boolean renderBackground, int cellRenderingMode) {
    // Rendering background
    if (renderBackground) {
      this.renderBackground();
    } else {
      background(255f);
    }

    // Rendering each cell
    LinkedList<Cell>.ListIterator iter = this.cells.listIterator();
    while (iter.hasNext()) {
      Cell cell = iter.next();

      this.renderCell(cell, cellRenderingMode);
    }
  }

  private void renderBackground() {
    // Drawing white background
    background(255f);

    // Rendering photosynthesis energy density
    for (int x = 0; x < this.columns; ++x) {
      for (int y = 0; y < this.maxPhotosynthesisDepth; ++y) {
        float colorA = map(this.getPhotosynthesisEnergy(x, y), 0f, this.maxPhotosynthesisEnergy, 0f, 127f);

        // Break if photosynthesis energy is almost zero so underneath cells have no energy at all
        if (colorA < 1f) {
          break;
        }

        fill(255f, 255f, 0f, colorA);
        rect(x * this.cellSideLength, y * this.cellSideLength, this.cellSideLength, this.cellSideLength);
        noFill();
      }
    }

    // Rendering minerals energy density
    for (int y = this.rows - this.maxMineralHeight; y < this.rows; ++y) {
      float colorA = map(this.getMineralEnergy(y), 0f, this.maxMineralEnergy, 0f, 127f);

      fill(0f, 0f, 255f, colorA);
      rect(0f, y * this.cellSideLength, width, this.cellSideLength);
      noFill();
    }
  }

  private void renderCell(Cell cell, int cellRenderingMode) {
    float colorR, colorG, colorB;

    // If cell is alive
    if (cell.isAlive) {
      // Diet mode
      if (cellRenderingMode % 4 == 0) {
        // Normalizing color and reducing it to range from 0 to 255
        colorR = cell.colorR;
        colorG = cell.colorG;
        colorB = cell.colorB;

        float colorVectorLength = sqrt(colorR * colorR + colorG * colorG + colorB * colorB);

        colorR *= 255f / colorVectorLength;
        colorG *= 255f / colorVectorLength;
        colorB *= 255f / colorVectorLength;
      }
      // Energy level mode
      else if (cellRenderingMode % 4 == 1) {
        colorR = 255f;
        colorG = map(cell.energy, 0f, this.maxEnergy, 255f, 0f);
        colorB = 0f;
      }
      // Energy sharing balance mode
      else if (cellRenderingMode % 4 == 2) {
        colorR = map(cell.energyShareBalance, -this.maxEnergy, this.maxEnergy, 255f, 0f);
        colorG = map(cell.energyShareBalance, -this.maxEnergy, this.maxEnergy, 127f, 255f);
        colorB = map(cell.energyShareBalance, -this.maxEnergy, this.maxEnergy, 0f, 255f);
      }
      // Last energy share mode
      else {
        colorR = map(cell.lastEnergyShare, -1f, 1f, 255f, 0f);
        colorG = map(cell.lastEnergyShare, -1f, 1f, 127f, 255f);
        colorB = map(cell.lastEnergyShare, -1f, 1f, 0f, 255f);
      }
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
