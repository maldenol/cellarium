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

#include <cmath>
#include <cstdlib>

#include <cellevolution/cell_controller.hpp>

// Position offsets per direction
static constexpr short kDirections[8][2] = {
    { 0, -1},
    { 1, -1},
    { 1,  0},
    { 1,  1},
    { 0,  1},
    {-1,  1},
    {-1,  0},
    {-1, -1},
};
// Mathematical constant
static constexpr float kTwoPi{6.28318530f};

// Lerp function
float map(float value, float inMin, float inMax, float outMin, float outMax) {
  return outMin + (outMax - outMin) * ((value - inMin) / (inMax - inMin));
}

CellController::CellController() : CellController{Params{}} {}

CellController::CellController(const Params& params)
    : _seed{params.seed},
      _width{params.width},
      _height{params.height},
      _scale{params.scale},
      _rows{params.rows},
      _columns{params.columns},
      _genomSize{params.genomSize},
      _maxInstructionsPerTick{params.maxInstructionsPerTick},
      _maxAkinGenomDifference{params.maxAkinGenomDifference},
      _minChildEnergy{params.minChildEnergy},
      _maxEnergy{params.maxEnergy},
      _maxPhotosynthesisEnergy{params.maxPhotosynthesisEnergy},
      _maxPhotosynthesisDepth{params.maxPhotosynthesisDepth},
      _summerDaytimeToWholeDayRatio{params.summerDaytimeToWholeDayRatio},
      _enableDaytimes{params.enableDaytimes},
      _enableSeasons{params.enableSeasons},
      _maxMineralEnergy{params.maxMineralEnergy},
      _maxMineralHeight{params.maxMineralHeight},
      _maxFoodEnergy{params.maxFoodEnergy},
      _randomMutationChance{params.randomMutationChance},
      _budMutationChance{params.budMutationChance},
      _dayDurationInTicks{params.dayDurationInTicks},
      _seasonDurationInDays{params.seasonDurationInDays},
      _gammaFlashPeriodInDays{params.gammaFlashPeriodInDays},
      _gammaFlashMaxMutationsCount{params.gammaFlashMaxMutationsCount},
      _enableTurnInstruction{params.enableTurnInstruction},
      _enableMoveInstruction{params.enableMoveInstruction},
      _enablePhotosynthesisEnergyInstruction{
          params.enablePhotosynthesisEnergyInstruction},
      _enableMineralEnergyInstruction{params.enableMineralEnergyInstruction},
      _enableFoodEnergyInstruction{params.enableFoodEnergyInstruction},
      _enableBudInstruction{params.enableBudInstruction},
      _enableMutateInstruction{params.enableMutateInstruction},
      _enableShareEnergyInstruction{params.enableShareEnergyInstruction},
      _enableLookInstruction{params.enableLookInstruction},
      _enableDetermineEnergyLevelInstruction{
          params.enableDetermineEnergyLevelInstruction},
      _enableDetermineDepthInstruction{params.enableDetermineDepthInstruction},
      _enableDeterminePhotosynthesisEnergyInstruction{
          params.enableDeterminePhotosynthesisEnergyInstruction},
      _enableDetermineMineralEnergyInstruction{
          params.enableDetermineMineralEnergyInstruction},
      _enableDeadCellPinningOnSinking{params.enableDeadCellPinningOnSinking} {
  srand(_seed);

  _cellBuffer.assign(_columns * _rows, Cell{});

  std::vector<short> genom;
  genom.assign(_genomSize, 3);
  addCell(std::make_shared<Cell>(genom, static_cast<short>(_minChildEnergy * 3),
                                 2, _columns * 5 / 2));

  _surgeOfPhotosynthesisEnergy.assign(_columns * _maxPhotosynthesisDepth, 0);
  _surgeOfMineralEnergy.assign(_maxMineralHeight, 0);
}

CellController::~CellController() {}

void CellController::act(bool drawBackground) noexcept {
  _drawBackground = drawBackground;

  // Updating world time
  updateTime();

  // Updating photosynthesis and mineral energy buffers if this tick must be drown
  if (_drawBackground) {
    updateEnergy();
  }

  // Going through all cells sequently
  LinkedList<std::shared_ptr<Cell>>::Iterator iter = _cellList.getIterator();
  while (iter.hasNext()) {
    Cell& cell = *iter.next();

    // Moving cell if it is dead
    if (!cell._isAlive) {
      move(cell);
      continue;
    }

    // Updating cell energy
    cell._energy--;
    // Removing cell if its energy is less than one
    if (cell._energy <= 0) {
      removeCell(std::make_shared<Cell>(cell));
      continue;
    }
    // Making cell bud if its energy greater or equals to maximal
    if (cell._energy >= _maxEnergy - 1) {
      bud(cell);
      continue;
    }

    // Fading last energy share
    cell._lastEnergyShare *= 0.99f;

    // Applying random mutation
    if (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) <
        _randomMutationChance) {
      mutateRandomGen(cell);
    }

    // Executing genom machine instructions with maximum instructions per tick limit
    for (short i = 0; i < _maxInstructionsPerTick; ++i) {
      // Getting current instruction from genom
      short instuction = cell._genom[cell._counter];

      // Performing appropriate action
      switch (instuction) {
        // Do nothing
        case 0:
          incrementGenomCounter(cell);
          break;
        // Turning
        case 1:
          if (_enableTurnInstruction) {
            turn(cell);
          }
          incrementGenomCounter(cell);
          break;
        // Moving (no more instructions permitted)
        case 2:
          if (_enableMoveInstruction) {
            i = _maxInstructionsPerTick;
            move(cell);
          }
          incrementGenomCounter(cell);
          break;
        // Getting energy from photosynthesis (no more instructions permitted)
        case 3:
          if (_enablePhotosynthesisEnergyInstruction) {
            i = _maxInstructionsPerTick;
            getEnergyFromPhotosynthesis(cell);
          }
          incrementGenomCounter(cell);
          break;
        // Getting energy from minerals (no more instructions permitted)
        case 4:
          if (_enableMineralEnergyInstruction) {
            i = _maxInstructionsPerTick;
            getEnergyFromMinerals(cell);
          }
          incrementGenomCounter(cell);
          break;
        // Getting energy from food (no more instructions permitted)
        case 5:
          if (_enableFoodEnergyInstruction) {
            i = _maxInstructionsPerTick;
            getEnergyFromFood(cell);
          }
          incrementGenomCounter(cell);
          break;
        // Budding (no more instructions permitted)
        case 6:
          if (_enableBudInstruction) {
            i = _maxInstructionsPerTick;
            bud(cell);
          }
          incrementGenomCounter(cell);
          break;
        // Making random gen mutate (no more instructions permitted)
        case 7:
          if (_enableMutateInstruction) {
            i = _maxInstructionsPerTick;
            mutateRandomGen(cell);
          }
          incrementGenomCounter(cell);
          break;
        // Sharing energy (no more instructions permitted)
        case 8:
          if (_enableShareEnergyInstruction) {
            i = _maxInstructionsPerTick;
            shareEnergy(cell);
          }
          incrementGenomCounter(cell);
          break;
        // Looking forward (conditional instruction)
        case 9:
          if (_enableLookInstruction) {
            lookForward(cell);
          } else {
            incrementGenomCounter(cell);
          }
          break;
        // Determining own energy level (conditional instruction)
        case 10:
          if (_enableDetermineEnergyLevelInstruction) {
            determineEnergyLevel(cell);
          } else {
            incrementGenomCounter(cell);
          }
          break;
        // Determining own depth (conditional instruction)
        case 11:
          if (_enableDetermineDepthInstruction) {
            determineDepth(cell);
          } else {
            incrementGenomCounter(cell);
          }
          break;
        // Determining energy surge from photosynthesis (conditional instruction)
        case 12:
          if (_enableDeterminePhotosynthesisEnergyInstruction) {
            determinePhotosynthesisEnergy(cell);
          } else {
            incrementGenomCounter(cell);
          }
          break;
        // Determining energy surge from minerals (conditional instruction)
        case 13:
          if (_enableDetermineMineralEnergyInstruction) {
            determineMineralEnergy(cell);
          } else {
            incrementGenomCounter(cell);
          }
          break;
        // Unconditional jump
        default:
          addGenToCounter(cell);
          break;
      }
    }
  }

  // Performing gamma flash
  gammaFlash();
}

void CellController::updateTime() noexcept {
  // Updating ticks
  ++_ticksNumber;

  // Updating ticks and years if ticks overflow
  if (_ticksNumber == _dayDurationInTicks * _seasonDurationInDays * 4) {
    _ticksNumber = 0;
    ++_yearsNumber;
  }
}

void CellController::updateEnergy() noexcept {
  // Calculating photosynthesis energy
  for (short x = 0; x < _columns; ++x) {
    short y = 0;

    // Filling with values until they are low
    while (y < _maxPhotosynthesisDepth) {
      _surgeOfPhotosynthesisEnergy[y * _columns + x] =
          calculatePhotosynthesisEnergy(y * _columns + x);

      // Break if photosynthesis energy is almost zero so underneath cells have no energy at all
      if (_surgeOfPhotosynthesisEnergy[y * _columns + x] < 1.0f) {
        break;
      }

      ++y;
    }

    // Filling with zeroes
    while (y < _maxPhotosynthesisDepth) {
      _surgeOfPhotosynthesisEnergy[y * _columns + x] = 0;

      ++y;
    }
  }

  // Calculating mineral energy
  for (short y = _rows - _maxMineralHeight; y < _rows; ++y) {
    _surgeOfMineralEnergy[_rows - 1 - y] = calculateMineralEnergy(y);
  }
}

void CellController::gammaFlash() noexcept {
  // If the time has come
  if (_ticksNumber % _dayDurationInTicks == 0 &&
      (_ticksNumber / _dayDurationInTicks +
       _yearsNumber * _seasonDurationInDays * 4) %
              _gammaFlashPeriodInDays ==
          0) {
    // For each cell
    LinkedList<std::shared_ptr<Cell>>::Iterator iter = _cellList.getIterator();
    while (iter.hasNext()) {
      Cell& cell = *iter.next();

      // Ignoring if cell is dead
      if (!cell._isAlive) {
        continue;
      }

      short mutationsCount =
          std::ceil(_gammaFlashMaxMutationsCount * static_cast<float>(rand()) /
                    static_cast<float>(RAND_MAX));
      for (short i = 0; i < mutationsCount; ++i) {
        mutateRandomGen(cell);
      }
    }
  }
}

void CellController::turn(Cell& cell) const noexcept {
  // Updating direction with overflow handling
  short deltaDirection = getNextNthGen(cell, 1);
  cell._direction      = (cell._direction + deltaDirection) % 8;
}

void CellController::move(Cell& cell) noexcept {
  // Calculating coordinates by target direction
  short targetDirection{};
  // If given cell is alive
  if (cell._isAlive) {
    short deltaDirection = getNextNthGen(cell, 1);
    targetDirection      = (cell._direction + deltaDirection) % 8;
  }
  // If given cell is dead
  else {
    // If given cell is pinned
    if (_enableDeadCellPinningOnSinking && cell._pinned) {
      return;
    }

    // Setting direction to 4 so it will be moving down (sinking) each tick if cell is dead and not pinned yet
    targetDirection = 4;
  }
  int targetIndex = calculateIndexByDirection(cell._index, targetDirection);

  // If coordinates are beyond simulation world (above top or below bottom)
  if (targetIndex == -1) {
    return;
  }

  // If there is nothing at this direction
  if (_cellBuffer[targetIndex] == Cell{}) {
    _cellBuffer[targetIndex] = cell;
    _cellBuffer[cell._index] = Cell{};

    cell._index = targetIndex;
  }
  // If there is an obstalce and given cell is dead
  else if (!cell._isAlive) {
    cell._pinned = true;
  }
}

void CellController::getEnergyFromPhotosynthesis(Cell& cell) noexcept {
  // Calculating energy from photosynthesis at current position
  short deltaEnergy = getPhotosynthesisEnergy(cell._index);

  // If energy from photosynthesis is positive
  if (deltaEnergy > 0) {
    // Increasing energy level
    cell._energy += deltaEnergy;

    // Making cell color more green
    ++cell._colorG;
  }
}

void CellController::getEnergyFromMinerals(Cell& cell) noexcept {
  // Calculating energy from minerals at current position
  short deltaEnergy = getMineralEnergy(cell._index);

  // If energy from minerals is positive
  if (deltaEnergy > 0) {
    // Increasing energy level
    cell._energy += deltaEnergy;

    // Making cell color more blue
    ++cell._colorB;
  }
}

void CellController::getEnergyFromFood(Cell& cell) noexcept {
  // Calculating coordinates by current direction
  int targetIndex = calculateIndexByDirection(cell._index, cell._direction);

  // If coordinates are beyond simulation world (above top or below bottom)
  if (targetIndex == -1) {
    return;
  }

  // Getting cell at this direction
  Cell& targetCell = _cellBuffer[targetIndex];

  // If there is a cell or food
  if (targetCell != Cell{}) {
    // Calculating energy from food
    short deltaEnergy = std::min(targetCell._energy, _maxFoodEnergy);

    // Increasing energy level
    cell._energy += deltaEnergy;

    // Making cell color more red
    ++cell._colorR;

    // Removing prey or food
    removeCell(std::make_shared<Cell>(targetCell));
  }
}

void CellController::bud(Cell& cell) noexcept {
  // Checking and updating energy
  if (cell._energy < _minChildEnergy * 2.0f) {
    return;
  }

  // Checking each direction clockwise for ability to bud
  for (short i = 0; i < 8; ++i) {
    // Calculating coordinates by current direction
    int targetIndex =
        calculateIndexByDirection(cell._index, (cell._direction + i) % 8);

    // If coordinates are beyond simulation world (above top or below bottom)
    if (targetIndex == -1) {
      continue;
    }

    // If there is nothing at this direction
    if (_cellBuffer[targetIndex] == Cell{}) {
      // Creating new cell
      std::shared_ptr<Cell> buddedCell{std::make_shared<Cell>(
          cell._genom, static_cast<short>(cell._energy / 2), cell._direction,
          targetIndex)};

      // Assigning cell color
      float colorVectorLength =
          sqrt(cell._colorR * cell._colorR + cell._colorG * cell._colorG +
               cell._colorB * cell._colorB);
      buddedCell->_colorR =
          static_cast<short>((cell._colorR * 2.0f / colorVectorLength));
      buddedCell->_colorG =
          static_cast<short>((cell._colorG * 2.0f / colorVectorLength));
      buddedCell->_colorB =
          static_cast<short>((cell._colorB * 2.0f / colorVectorLength));

      // Applying random bud mutation to the budded cell
      if (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) <
          _budMutationChance) {
        mutateRandomGen(*buddedCell);
      }

      // Applying random bud mutation to current cell
      cell._energy -= cell._energy / 2;
      if (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) <
          _budMutationChance) {
        mutateRandomGen(cell);
      }

      addCell(buddedCell);

      return;
    }
  }

  // Turning cell into food if it have to but not able to bud
  turnIntoFood(cell);
}

void CellController::mutateRandomGen(Cell& cell) const noexcept {
  // Changing random gen on another random one
  cell._genom[std::floor(static_cast<float>(rand()) /
                         static_cast<float>(RAND_MAX) *
                         static_cast<float>(_genomSize))] =
      std::floor(static_cast<float>(rand()) / static_cast<float>(RAND_MAX) *
                 static_cast<float>(_genomSize));
}

void CellController::shareEnergy(Cell& cell) noexcept {
  // Calculating coordinates by current direction
  int targetIndex = calculateIndexByDirection(cell._index, cell._direction);

  // If coordinates are beyond simulation world (above top or below bottom)
  if (targetIndex == -1) {
    return;
  }

  // Getting cell at this direction
  Cell& targetCell = _cellBuffer[targetIndex];

  // If there is a cell
  if (targetCell != Cell{} && targetCell._isAlive) {
    // Calculating energy to share
    short deltaEnergy =
        static_cast<short>((cell._energy * getNextNthGen(cell, 1) /
                            static_cast<float>(_genomSize)));

    // Sharing energy
    cell._energy -= deltaEnergy;
    targetCell._energy += deltaEnergy;

    // Updating energy share balances
    cell._energyShareBalance += deltaEnergy;
    targetCell._energyShareBalance -= deltaEnergy;

    // Updating last energy shares
    cell._lastEnergyShare       = 1.0f;
    targetCell._lastEnergyShare = -1.0f;
  }
}

void CellController::lookForward(Cell& cell) noexcept {
  // Calculating coordinates by current direction
  int targetIndex = calculateIndexByDirection(cell._index, cell._direction);

  // If coordinates are beyond simulation world (above top or below bottom)
  if (targetIndex == -1) {
    return;
  }

  // Getting cell at this direction
  Cell& targetCell = _cellBuffer[targetIndex];

  // If there is a cell or food
  if (targetCell != Cell{}) {
    // If it is a live cell
    if (targetCell._isAlive) {
      // If it is an akin cell
      if (areAkin(cell, targetCell)) {
        jumpCounter(cell, getNextNthGen(cell, 3));
      }
      // If it is a strange cell
      else {
        jumpCounter(cell, getNextNthGen(cell, 4));
      }
    }
    // If it is food
    else {
      jumpCounter(cell, getNextNthGen(cell, 2));
    }
  }
  // If there is nothing at this direction
  else {
    jumpCounter(cell, getNextNthGen(cell, 1));
  }
}

void CellController::determineEnergyLevel(Cell& cell) const noexcept {
  // Calculating value to compare
  short valueToCompare = static_cast<short>(
      (_maxEnergy * getNextNthGen(cell, 1) / static_cast<float>(_genomSize)));

  // Less
  if (cell._energy < valueToCompare) {
    jumpCounter(cell, getNextNthGen(cell, 2));
  }
  // Greater
  else if (cell._energy > valueToCompare) {
    jumpCounter(cell, getNextNthGen(cell, 4));
  }
  // Equal
  else {
    jumpCounter(cell, getNextNthGen(cell, 3));
  }
}

void CellController::determineDepth(Cell& cell) const noexcept {
  // Calculating values to compare
  short row            = calculateRow(cell._index);
  short valueToCompare = static_cast<short>(
      (_rows * getNextNthGen(cell, 1) / static_cast<float>(_genomSize)));

  // Less
  if (row < valueToCompare) {
    jumpCounter(cell, getNextNthGen(cell, 2));
  }
  // Greater
  else if (row > valueToCompare) {
    jumpCounter(cell, getNextNthGen(cell, 4));
  }
  // Equal
  else {
    jumpCounter(cell, getNextNthGen(cell, 3));
  }
}

void CellController::determinePhotosynthesisEnergy(Cell& cell) noexcept {
  // Calculating value to compare
  short valueToCompare =
      static_cast<short>((_maxPhotosynthesisEnergy * getNextNthGen(cell, 1) /
                          static_cast<float>(_genomSize)));

  // Calculating surge of photosynthesis energy
  short surgeOfEnergy = getPhotosynthesisEnergy(cell._index);

  // Less
  if (surgeOfEnergy < valueToCompare) {
    jumpCounter(cell, getNextNthGen(cell, 2));
  }
  // Greater
  else if (surgeOfEnergy > valueToCompare) {
    jumpCounter(cell, getNextNthGen(cell, 4));
  }
  // Equal
  else {
    jumpCounter(cell, getNextNthGen(cell, 3));
  }
}

void CellController::determineMineralEnergy(Cell& cell) noexcept {
  // Calculating value to compare
  short valueToCompare =
      static_cast<short>((_maxMineralEnergy * getNextNthGen(cell, 1) /
                          static_cast<float>(_genomSize)));

  // Calculating surge of photosynthesis energy
  short surgeOfEnergy = getMineralEnergy(cell._index);

  // Less
  if (surgeOfEnergy < valueToCompare) {
    jumpCounter(cell, getNextNthGen(cell, 2));
  }
  // Greater
  else if (surgeOfEnergy > valueToCompare) {
    jumpCounter(cell, getNextNthGen(cell, 4));
  }
  // Equal
  else {
    jumpCounter(cell, getNextNthGen(cell, 3));
  }
}

void CellController::incrementGenomCounter(Cell& cell) const noexcept {
  // Incrementing instruction counter with overflow handling
  cell._counter = (cell._counter + 1) % _genomSize;
}

void CellController::addGenToCounter(Cell& cell) const noexcept {
  // Adding dummy instruction value to instruction counter with overflow handling
  cell._counter = (cell._counter + cell._genom[cell._counter]) % _genomSize;
}

void CellController::jumpCounter(Cell& cell, short offset) const noexcept {
  // Performing jump command on instruction counter with overflow handling
  cell._counter = (cell._counter + offset) % _genomSize;
}

short CellController::getNextNthGen(Cell& cell, short n) const noexcept {
  // Getting (counter + n)'th gen
  return cell._genom[(cell._counter + n) % _genomSize];
}

void CellController::turnIntoFood(Cell& cell) const noexcept {
  cell._isAlive = false;
}

bool CellController::areAkin(Cell& cell1, Cell& cell2) const noexcept {
  short diff{};

  for (short i = 0; i < _genomSize; ++i) {
    if (cell1._genom[i] != cell2._genom[i]) {
      ++diff;
      if (diff > _maxAkinGenomDifference) {
        return false;
      }
    }
  }

  return true;
}

short CellController::getPhotosynthesisEnergy(int index) const noexcept {
  // Calculating cell row
  short row = calculateRow(index);

  // If depth is greater than maximal
  if (row >= _maxPhotosynthesisDepth) {
    return 0;
  }

  short energy{};

  // If this tick must not be drown
  if (!_drawBackground) {
    energy = calculatePhotosynthesisEnergy(index);
  }
  // If this tick must be drown
  else {
    energy = _surgeOfPhotosynthesisEnergy[index];
  }

  return energy;
}

short CellController::getMineralEnergy(int index) const noexcept {
  // Calculating cell row
  short row = calculateRow(index);

  // If depth is less than minimal
  if (row < _rows - _maxMineralHeight) {
    return 0;
  }

  // If this tick must not be drown
  if (!_drawBackground) {
    return calculateMineralEnergy(row);
  }

  return _surgeOfMineralEnergy[_rows - 1 - row];
}

short CellController::calculatePhotosynthesisEnergy(int index) const noexcept {
  // Calculating cell column and row
  short column = calculateColumn(index);
  short row    = calculateRow(index);

  // Calculating daytime coefficient if needed
  float daytimeCoefficient = 1.0f;
  if (_enableDaytimes) {
    // Calculation sun epicenter position along X-axis
    int sunPosition = static_cast<int>(map(_ticksNumber % _dayDurationInTicks,
                                           0.0f, _dayDurationInTicks - 1.0f,
                                           0.0f, _columns - 1.0f));
    // Calculation minimal or maximal distance to sun epicenter along X-axis
    int distanceToSun = std::abs(sunPosition - column);
    // Calculation minimal distance to sun epicenter along X-axis
    int minDistanceToSun =
        std::min(distanceToSun, _columns - 1 - distanceToSun);
    // Calculation sunny area width based on current season if needed
    float daytimeWidthRatio = 1.0f - _summerDaytimeToWholeDayRatio;
    if (_enableSeasons) {
      daytimeWidthRatio = map(std::sin(map(_ticksNumber / _dayDurationInTicks /
                                               _seasonDurationInDays,
                                           0.0f, 4.0f, 0.0f, kTwoPi)),
                              -1.0f, 1.0f, 1.0f - _summerDaytimeToWholeDayRatio,
                              _summerDaytimeToWholeDayRatio);
    }
    // Calculating daytime coefficient
    daytimeCoefficient =
        (minDistanceToSun < (_columns - 1) / 2.0f * daytimeWidthRatio) ? 1.0f
                                                                       : 0.0f;

    // If it is night here
    if (daytimeCoefficient < 1.0f) {
      return 0;
    }
  }

  // Calculating depth coefficient
  float depthCoefficient = map(row, 0.0f, _maxPhotosynthesisDepth, 1.0f, 0.0f);

  // Getting energy from photosynthesis
  return static_cast<short>(
      (_maxPhotosynthesisEnergy * depthCoefficient * daytimeCoefficient));
}

short CellController::calculateMineralEnergy(int index) const noexcept {
  // Calculating cell row
  short row = calculateRow(index);

  // Getting energy from minerals
  return static_cast<short>(map(row, _rows - 1.0f,
                                _rows - 1.0f - _maxMineralHeight,
                                _maxMineralEnergy, 0.0f));
}

short CellController::calculateColumn(int index) const noexcept {
  return static_cast<short>(index - index / _columns * _columns);
}

short CellController::calculateRow(int index) const noexcept {
  return static_cast<short>(index / _columns);
}

int CellController::calculateIndexByDirection(int   index,
                                              short direction) const noexcept {
  // Calculating column at given direction with overflow handling
  int column = calculateColumn(index);
  int c      = (column + kDirections[direction][0] + _columns) % _columns;

  // Calculating row at given direction with overflow handling
  int row = calculateColumn(index);
  int r   = row + kDirections[direction][1];
  if (r > _rows - 1 || r < 0) {
    return -1;
  }

  return r * _columns + c;
}

void CellController::addCell(const std::shared_ptr<Cell>& cellPtr) noexcept {
  _cellBuffer[cellPtr->_index] = *cellPtr;

  // Pushing cell to the front of the linked list
  // so it will be processed not earlier than the next tick
  // and before older cells (younger cells have smaller "reaction time")
  _cellList.pushFront(cellPtr);
}

void CellController::removeCell(const std::shared_ptr<Cell>& cellPtr) noexcept {
  _cellBuffer[cellPtr->_index] = Cell{};

  _cellList.remove(cellPtr);
}
