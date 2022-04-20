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

// Header file
#include "./cell_controller.hpp"

// STD
#include <array>
#include <cmath>
#include <utility>

// Using header file namespace
using namespace CellEvolution;

// Position offsets per 2D direction
static constexpr int                                             kDirectionCount{8};
static constexpr std::array<std::array<int, 2>, kDirectionCount> kDirectionOffsets{
    std::array<int, 2>{ 0, -1},
     std::array<int, 2>{ 1, -1},
     std::array<int, 2>{ 1,  0},
    std::array<int, 2>{ 1,  1},
     std::array<int, 2>{ 0,  1},
     std::array<int, 2>{-1,  1},
    std::array<int, 2>{-1,  0},
     std::array<int, 2>{-1, -1},
};
// First cell properties
static constexpr int   kFirstCellGenomInstructions{3};
static constexpr float kFirstCellEnergyMultiplier{3.0f};
static constexpr int   kFirstCellDirection{2};
static constexpr float kFirstCellIndexMultiplier{2.5f};
// Last energy share value fade multiplier
static constexpr float kLastEnergyShareFadeMultiplier{0.99f};
// Budded cell parent color multiplier
static constexpr float kBuddedCellParentColorMultiplier{2.0f};
// Mathematical constant
static constexpr float kTwoPi{6.28318530f};

// Linearly interpolates value from one range (in) into another (out)
template <typename T1, typename T2, typename T3, typename T4, typename T5>
float map(T1 value, T2 inMin, T3 inMax, T4 outMin, T5 outMax) {
  return static_cast<float>(outMin) + (static_cast<float>(outMax) - static_cast<float>(outMin)) *
                                          (static_cast<float>(value) - static_cast<float>(inMin)) /
                                          (static_cast<float>(inMax) - static_cast<float>(inMin));
}
// Linearly interpolates value from one range (in) into another (out) with clamping
template <typename T1, typename T2, typename T3, typename T4, typename T5>
float mapClamp(T1 value, T2 inMin, T3 inMax, T4 outMin, T5 outMax) {
  // If inMin is less or equal to inMax
  if (static_cast<float>(inMin) <= static_cast<float>(inMax)) {
    // If value is less or equal to inMin - returning outMin
    if (static_cast<float>(value) <= static_cast<float>(inMin)) {
      return static_cast<float>(outMin);
    }

    // If value is greater or equal to inMax - returning outMax
    if (static_cast<float>(value) >= static_cast<float>(inMax)) {
      return static_cast<float>(outMax);
    }
  } else {
    // If value is greater or equal to inMin - returning outMin
    if (static_cast<float>(value) >= static_cast<float>(inMin)) {
      return static_cast<float>(outMin);
    }

    // If value is less or equal to inMax - returning outMax
    if (static_cast<float>(value) <= static_cast<float>(inMax)) {
      return static_cast<float>(outMax);
    }
  }

  // Interpolation in other cases
  return static_cast<float>(outMin) + (static_cast<float>(outMax) - static_cast<float>(outMin)) *
                                          (static_cast<float>(value) - static_cast<float>(inMin)) /
                                          (static_cast<float>(inMax) - static_cast<float>(inMin));
}

CellController::CellController() : CellController{Params{}} {}

CellController::CellController(const Params &params)
    : _mersenneTwisterEngine{params.mersenneTwisterEngine},
      _randomSeed{params.randomSeed},
      _width{params.width},
      _height{params.height},
      _cellSize{params.cellSize},
      _columns{params.columns},
      _rows{params.rows},
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
      _enableInstructionTurn{params.enableInstructionTurn},
      _enableInstructionMove{params.enableInstructionMove},
      _enableInstructionGetEnergyFromPhotosynthesis{
          params.enableInstructionGetEnergyFromPhotosynthesis},
      _enableInstructionGetEnergyFromMinerals{params.enableInstructionGetEnergyFromMinerals},
      _enableInstructionGetEnergyFromFood{params.enableInstructionGetEnergyFromFood},
      _enableInstructionBud{params.enableInstructionBud},
      _enableInstructionMutateRandomGen{params.enableInstructionMutateRandomGen},
      _enableInstructionShareEnergy{params.enableInstructionShareEnergy},
      _enableInstructionLookForward{params.enableInstructionLookForward},
      _enableInstructionDetermineEnergyLevel{params.enableInstructionDetermineEnergyLevel},
      _enableInstructionDetermineDepth{params.enableInstructionDetermineDepth},
      _enableInstructionDeterminePhotosynthesisEnergy{
          params.enableInstructionDeterminePhotosynthesisEnergy},
      _enableInstructionDetermineMineralEnergy{params.enableInstructionDetermineMineralEnergy},
      _enableDeadCellPinningOnSinking{params.enableDeadCellPinningOnSinking} {
  // Allocating memory for vector of cells
  _cellVector.assign(_columns * _rows, Cell{});
  _cellVector.reserve(_columns * _rows);

  // Creating the first cell
  std::vector<int> firstCellGenom;
  firstCellGenom.assign(_genomSize, kFirstCellGenomInstructions);
  firstCellGenom.reserve(_genomSize);
  addCell(Cell{firstCellGenom,
               static_cast<int>(static_cast<float>(_minChildEnergy) * kFirstCellEnergyMultiplier),
               kFirstCellDirection,
               static_cast<int>(static_cast<float>(_columns) * kFirstCellIndexMultiplier)});
}

CellController::CellController(const CellController &cellController) noexcept
    : _mersenneTwisterEngine{cellController._mersenneTwisterEngine},
      _randomSeed{cellController._randomSeed},
      _width{cellController._width},
      _height{cellController._height},
      _cellSize{cellController._cellSize},
      _columns{cellController._columns},
      _rows{cellController._rows},
      _genomSize{cellController._genomSize},
      _maxInstructionsPerTick{cellController._maxInstructionsPerTick},
      _maxAkinGenomDifference{cellController._maxAkinGenomDifference},
      _minChildEnergy{cellController._minChildEnergy},
      _maxEnergy{cellController._maxEnergy},
      _maxPhotosynthesisEnergy{cellController._maxPhotosynthesisEnergy},
      _maxPhotosynthesisDepth{cellController._maxPhotosynthesisDepth},
      _summerDaytimeToWholeDayRatio{cellController._summerDaytimeToWholeDayRatio},
      _enableDaytimes{cellController._enableDaytimes},
      _enableSeasons{cellController._enableSeasons},
      _maxMineralEnergy{cellController._maxMineralEnergy},
      _maxMineralHeight{cellController._maxMineralHeight},
      _maxFoodEnergy{cellController._maxFoodEnergy},
      _randomMutationChance{cellController._randomMutationChance},
      _budMutationChance{cellController._budMutationChance},
      _dayDurationInTicks{cellController._dayDurationInTicks},
      _seasonDurationInDays{cellController._seasonDurationInDays},
      _gammaFlashPeriodInDays{cellController._gammaFlashPeriodInDays},
      _gammaFlashMaxMutationsCount{cellController._gammaFlashMaxMutationsCount},
      _enableInstructionTurn{cellController._enableInstructionTurn},
      _enableInstructionMove{cellController._enableInstructionMove},
      _enableInstructionGetEnergyFromPhotosynthesis{
          cellController._enableInstructionGetEnergyFromPhotosynthesis},
      _enableInstructionGetEnergyFromMinerals{
          cellController._enableInstructionGetEnergyFromMinerals},
      _enableInstructionGetEnergyFromFood{cellController._enableInstructionGetEnergyFromFood},
      _enableInstructionBud{cellController._enableInstructionBud},
      _enableInstructionMutateRandomGen{cellController._enableInstructionMutateRandomGen},
      _enableInstructionShareEnergy{cellController._enableInstructionShareEnergy},
      _enableInstructionLookForward{cellController._enableInstructionLookForward},
      _enableInstructionDetermineEnergyLevel{cellController._enableInstructionDetermineEnergyLevel},
      _enableInstructionDetermineDepth{cellController._enableInstructionDetermineDepth},
      _enableInstructionDeterminePhotosynthesisEnergy{
          cellController._enableInstructionDeterminePhotosynthesisEnergy},
      _enableInstructionDetermineMineralEnergy{
          cellController._enableInstructionDetermineMineralEnergy},
      _enableDeadCellPinningOnSinking{cellController._enableDeadCellPinningOnSinking},
      _cellIndexList{cellController._cellIndexList},
      _cellVector{cellController._cellVector},
      _ticksNumber{cellController._ticksNumber},
      _yearsNumber{cellController._yearsNumber} {}

CellController &CellController::operator=(const CellController &cellController) noexcept {
  _mersenneTwisterEngine        = cellController._mersenneTwisterEngine;
  _randomSeed                   = cellController._randomSeed;
  _width                        = cellController._width;
  _height                       = cellController._height;
  _cellSize                     = cellController._cellSize;
  _columns                      = cellController._columns;
  _rows                         = cellController._rows;
  _genomSize                    = cellController._genomSize;
  _maxInstructionsPerTick       = cellController._maxInstructionsPerTick;
  _maxAkinGenomDifference       = cellController._maxAkinGenomDifference;
  _minChildEnergy               = cellController._minChildEnergy;
  _maxEnergy                    = cellController._maxEnergy;
  _maxPhotosynthesisEnergy      = cellController._maxPhotosynthesisEnergy;
  _maxPhotosynthesisDepth       = cellController._maxPhotosynthesisDepth;
  _summerDaytimeToWholeDayRatio = cellController._summerDaytimeToWholeDayRatio;
  _enableDaytimes               = cellController._enableDaytimes;
  _enableSeasons                = cellController._enableSeasons;
  _maxMineralEnergy             = cellController._maxMineralEnergy;
  _maxMineralHeight             = cellController._maxMineralHeight;
  _maxFoodEnergy                = cellController._maxFoodEnergy;
  _randomMutationChance         = cellController._randomMutationChance;
  _budMutationChance            = cellController._budMutationChance;
  _dayDurationInTicks           = cellController._dayDurationInTicks;
  _seasonDurationInDays         = cellController._seasonDurationInDays;
  _gammaFlashPeriodInDays       = cellController._gammaFlashPeriodInDays;
  _gammaFlashMaxMutationsCount  = cellController._gammaFlashMaxMutationsCount;
  _enableInstructionTurn        = cellController._enableInstructionTurn;
  _enableInstructionMove        = cellController._enableInstructionMove;
  _enableInstructionGetEnergyFromPhotosynthesis =
      cellController._enableInstructionGetEnergyFromPhotosynthesis;
  _enableInstructionGetEnergyFromMinerals = cellController._enableInstructionGetEnergyFromMinerals;
  _enableInstructionGetEnergyFromFood     = cellController._enableInstructionGetEnergyFromFood;
  _enableInstructionBud                   = cellController._enableInstructionBud;
  _enableInstructionMutateRandomGen       = cellController._enableInstructionMutateRandomGen;
  _enableInstructionShareEnergy           = cellController._enableInstructionShareEnergy;
  _enableInstructionLookForward           = cellController._enableInstructionLookForward;
  _enableInstructionDetermineEnergyLevel  = cellController._enableInstructionDetermineEnergyLevel;
  _enableInstructionDetermineDepth        = cellController._enableInstructionDetermineDepth;
  _enableInstructionDeterminePhotosynthesisEnergy =
      cellController._enableInstructionDeterminePhotosynthesisEnergy;
  _enableInstructionDetermineMineralEnergy =
      cellController._enableInstructionDetermineMineralEnergy;
  _enableDeadCellPinningOnSinking = cellController._enableDeadCellPinningOnSinking;
  _cellIndexList                  = cellController._cellIndexList;
  _cellVector                     = cellController._cellVector;
  _ticksNumber                    = cellController._ticksNumber;
  _yearsNumber                    = cellController._yearsNumber;

  return *this;
}

CellController::CellController(CellController &&cellController) noexcept
    : _mersenneTwisterEngine{std::exchange(cellController._mersenneTwisterEngine, std::mt19937{})},
      _randomSeed{std::exchange(cellController._randomSeed, 0)},
      _width{std::exchange(cellController._width, 0)},
      _height{std::exchange(cellController._height, 0)},
      _cellSize{std::exchange(cellController._cellSize, 0.0f)},
      _columns{std::exchange(cellController._columns, 0)},
      _rows{std::exchange(cellController._rows, 0)},
      _genomSize{std::exchange(cellController._genomSize, 0)},
      _maxInstructionsPerTick{std::exchange(cellController._maxInstructionsPerTick, 0)},
      _maxAkinGenomDifference{std::exchange(cellController._maxAkinGenomDifference, 0)},
      _minChildEnergy{std::exchange(cellController._minChildEnergy, 0)},
      _maxEnergy{std::exchange(cellController._maxEnergy, 0)},
      _maxPhotosynthesisEnergy{std::exchange(cellController._maxPhotosynthesisEnergy, 0)},
      _maxPhotosynthesisDepth{std::exchange(cellController._maxPhotosynthesisDepth, 0)},
      _summerDaytimeToWholeDayRatio{
          std::exchange(cellController._summerDaytimeToWholeDayRatio, 0.0f)},
      _enableDaytimes{std::exchange(cellController._enableDaytimes, false)},
      _enableSeasons{std::exchange(cellController._enableSeasons, false)},
      _maxMineralEnergy{std::exchange(cellController._maxMineralEnergy, 0)},
      _maxMineralHeight{std::exchange(cellController._maxMineralHeight, 0)},
      _maxFoodEnergy{std::exchange(cellController._maxFoodEnergy, 0)},
      _randomMutationChance{std::exchange(cellController._randomMutationChance, 0.0f)},
      _budMutationChance{std::exchange(cellController._budMutationChance, 0.0f)},
      _dayDurationInTicks{std::exchange(cellController._dayDurationInTicks, 0)},
      _seasonDurationInDays{std::exchange(cellController._seasonDurationInDays, 0)},
      _gammaFlashPeriodInDays{std::exchange(cellController._gammaFlashPeriodInDays, 0)},
      _gammaFlashMaxMutationsCount{std::exchange(cellController._gammaFlashMaxMutationsCount, 0)},
      _enableInstructionTurn{std::exchange(cellController._enableInstructionTurn, false)},
      _enableInstructionMove{std::exchange(cellController._enableInstructionMove, false)},
      _enableInstructionGetEnergyFromPhotosynthesis{
          std::exchange(cellController._enableInstructionGetEnergyFromPhotosynthesis, false)},
      _enableInstructionGetEnergyFromMinerals{
          std::exchange(cellController._enableInstructionGetEnergyFromMinerals, false)},
      _enableInstructionGetEnergyFromFood{
          std::exchange(cellController._enableInstructionGetEnergyFromFood, false)},
      _enableInstructionBud{std::exchange(cellController._enableInstructionBud, false)},
      _enableInstructionMutateRandomGen{
          std::exchange(cellController._enableInstructionMutateRandomGen, false)},
      _enableInstructionShareEnergy{
          std::exchange(cellController._enableInstructionShareEnergy, false)},
      _enableInstructionLookForward{
          std::exchange(cellController._enableInstructionLookForward, false)},
      _enableInstructionDetermineEnergyLevel{
          std::exchange(cellController._enableInstructionDetermineEnergyLevel, false)},
      _enableInstructionDetermineDepth{
          std::exchange(cellController._enableInstructionDetermineDepth, false)},
      _enableInstructionDeterminePhotosynthesisEnergy{
          std::exchange(cellController._enableInstructionDeterminePhotosynthesisEnergy, false)},
      _enableInstructionDetermineMineralEnergy{
          std::exchange(cellController._enableInstructionDetermineMineralEnergy, false)},
      _enableDeadCellPinningOnSinking{
          std::exchange(cellController._enableDeadCellPinningOnSinking, false)},
      _cellIndexList{std::exchange(cellController._cellIndexList, LinkedList<int>{})},
      _cellVector{std::exchange(cellController._cellVector, std::vector<Cell>{})},
      _ticksNumber{std::exchange(cellController._ticksNumber, 0)},
      _yearsNumber{std::exchange(cellController._yearsNumber, 0)} {}

CellController &CellController::operator=(CellController &&cellController) noexcept {
  std::swap(_mersenneTwisterEngine, cellController._mersenneTwisterEngine);
  std::swap(_randomSeed, cellController._randomSeed);
  std::swap(_width, cellController._width);
  std::swap(_height, cellController._height);
  std::swap(_cellSize, cellController._cellSize);
  std::swap(_columns, cellController._columns);
  std::swap(_rows, cellController._rows);
  std::swap(_genomSize, cellController._genomSize);
  std::swap(_maxInstructionsPerTick, cellController._maxInstructionsPerTick);
  std::swap(_maxAkinGenomDifference, cellController._maxAkinGenomDifference);
  std::swap(_minChildEnergy, cellController._minChildEnergy);
  std::swap(_maxEnergy, cellController._maxEnergy);
  std::swap(_maxPhotosynthesisEnergy, cellController._maxPhotosynthesisEnergy);
  std::swap(_maxPhotosynthesisDepth, cellController._maxPhotosynthesisDepth);
  std::swap(_summerDaytimeToWholeDayRatio, cellController._summerDaytimeToWholeDayRatio);
  std::swap(_enableDaytimes, cellController._enableDaytimes);
  std::swap(_enableSeasons, cellController._enableSeasons);
  std::swap(_maxMineralEnergy, cellController._maxMineralEnergy);
  std::swap(_maxMineralHeight, cellController._maxMineralHeight);
  std::swap(_maxFoodEnergy, cellController._maxFoodEnergy);
  std::swap(_randomMutationChance, cellController._randomMutationChance);
  std::swap(_budMutationChance, cellController._budMutationChance);
  std::swap(_dayDurationInTicks, cellController._dayDurationInTicks);
  std::swap(_seasonDurationInDays, cellController._seasonDurationInDays);
  std::swap(_gammaFlashPeriodInDays, cellController._gammaFlashPeriodInDays);
  std::swap(_gammaFlashMaxMutationsCount, cellController._gammaFlashMaxMutationsCount);
  std::swap(_enableInstructionTurn, cellController._enableInstructionTurn);
  std::swap(_enableInstructionMove, cellController._enableInstructionMove);
  std::swap(_enableInstructionGetEnergyFromPhotosynthesis,
            cellController._enableInstructionGetEnergyFromPhotosynthesis);
  std::swap(_enableInstructionGetEnergyFromMinerals,
            cellController._enableInstructionGetEnergyFromMinerals);
  std::swap(_enableInstructionGetEnergyFromFood,
            cellController._enableInstructionGetEnergyFromFood);
  std::swap(_enableInstructionBud, cellController._enableInstructionBud);
  std::swap(_enableInstructionMutateRandomGen, cellController._enableInstructionMutateRandomGen);
  std::swap(_enableInstructionShareEnergy, cellController._enableInstructionShareEnergy);
  std::swap(_enableInstructionLookForward, cellController._enableInstructionLookForward);
  std::swap(_enableInstructionDetermineEnergyLevel,
            cellController._enableInstructionDetermineEnergyLevel);
  std::swap(_enableInstructionDetermineDepth, cellController._enableInstructionDetermineDepth);
  std::swap(_enableInstructionDeterminePhotosynthesisEnergy,
            cellController._enableInstructionDeterminePhotosynthesisEnergy);
  std::swap(_enableInstructionDetermineMineralEnergy,
            cellController._enableInstructionDetermineMineralEnergy);
  std::swap(_enableDeadCellPinningOnSinking, cellController._enableDeadCellPinningOnSinking);
  std::swap(_cellIndexList, cellController._cellIndexList);
  std::swap(_cellVector, cellController._cellVector);
  std::swap(_ticksNumber, cellController._ticksNumber);
  std::swap(_yearsNumber, cellController._yearsNumber);

  return *this;
}

CellController::~CellController() noexcept {}

void CellController::act() noexcept {
  // Updating world time
  updateTime();

  // Going through all cells sequently
  LinkedList<int>::Iterator iter{_cellIndexList.getIterator()};
  while (iter.hasNext()) {
    Cell &cell = _cellVector[iter.next()];

    // Moving cell if it is dead
    if (!cell._isAlive) {
      move(cell);
      continue;
    }

    // Updating cell energy
    cell._energy--;
    // Removing cell if its energy is less than one
    if (cell._energy <= 0) {
      removeCell(cell);
      continue;
    }
    // Making cell bud if its energy greater or equals to maximal
    if (cell._energy >= _maxEnergy - 1) {
      bud(cell);
      continue;
    }

    // Fading last energy share
    cell._lastEnergyShare *= kLastEnergyShareFadeMultiplier;

    // Applying random mutation
    if (static_cast<float>(_mersenneTwisterEngine()) /
            static_cast<float>(_mersenneTwisterEngine.max()) <
        _randomMutationChance) {
      mutateRandomGen(cell);
    }

    // Executing genom machine instructions with maximum instructions per tick limit
    for (int i = 0; i < _maxInstructionsPerTick; ++i) {
      // Getting current instruction from genom
      int instuction = cell._genom[cell._counter];

      // Performing appropriate instruction
      switch (instuction) {
        // Do nothing
        case kInstructionDoNothing: {
          incrementGenomCounter(cell);
        } break;
        // Turning
        case kInstructionTurn: {
          if (_enableInstructionTurn) {
            turn(cell);
          }
          incrementGenomCounter(cell);
        } break;
        // Moving (no more instructions permitted)
        case kInstructionMove: {
          if (_enableInstructionMove) {
            i = _maxInstructionsPerTick;
            move(cell);
          }
          incrementGenomCounter(cell);
        } break;
        // Getting energy from photosynthesis (no more instructions permitted)
        case kInstructionGetEnergyFromPhotosynthesis: {
          if (_enableInstructionGetEnergyFromPhotosynthesis) {
            i = _maxInstructionsPerTick;
            getEnergyFromPhotosynthesis(cell);
          }
          incrementGenomCounter(cell);
        } break;
        // Getting energy from minerals (no more instructions permitted)
        case kInstructionGetEnergyFromMinerals: {
          if (_enableInstructionGetEnergyFromMinerals) {
            i = _maxInstructionsPerTick;
            getEnergyFromMinerals(cell);
          }
          incrementGenomCounter(cell);
        } break;
        // Getting energy from food (no more instructions permitted)
        case kInstructionGetEnergyFromFood: {
          if (_enableInstructionGetEnergyFromFood) {
            i = _maxInstructionsPerTick;
            getEnergyFromFood(cell);
          }
          incrementGenomCounter(cell);
        } break;
        // Budding (no more instructions permitted)
        case kInstructionBud: {
          if (_enableInstructionBud) {
            i = _maxInstructionsPerTick;
            bud(cell);
          }
          incrementGenomCounter(cell);
        } break;
        // Making random gen mutate (no more instructions permitted)
        case kInstructionMutateRandomGen: {
          if (_enableInstructionMutateRandomGen) {
            i = _maxInstructionsPerTick;
            mutateRandomGen(cell);
          }
          incrementGenomCounter(cell);
        } break;
        // Sharing energy (no more instructions permitted)
        case kInstructionShareEnergy: {
          if (_enableInstructionShareEnergy) {
            i = _maxInstructionsPerTick;
            shareEnergy(cell);
          }
          incrementGenomCounter(cell);
        } break;
        // Looking forward (conditional instruction)
        case kInstructionLookForward: {
          if (_enableInstructionLookForward) {
            lookForward(cell);
          } else {
            incrementGenomCounter(cell);
          }
        } break;
        // Determining own energy level (conditional instruction)
        case kInstructionDetermineEnergyLevel: {
          if (_enableInstructionDetermineEnergyLevel) {
            determineEnergyLevel(cell);
          } else {
            incrementGenomCounter(cell);
          }
        } break;
        // Determining own depth (conditional instruction)
        case kInstructionDetermineDepth: {
          if (_enableInstructionDetermineDepth) {
            determineDepth(cell);
          } else {
            incrementGenomCounter(cell);
          }
        } break;
        // Determining available energy from photosynthesis (conditional instruction)
        case kInstructionDeterminePhotosynthesisEnergy: {
          if (_enableInstructionDeterminePhotosynthesisEnergy) {
            determinePhotosynthesisEnergy(cell);
          } else {
            incrementGenomCounter(cell);
          }
        } break;
        // Determining available energy from minerals (conditional instruction)
        case kInstructionDetermineMineralEnergy: {
          if (_enableInstructionDetermineMineralEnergy) {
            determineMineralEnergy(cell);
          } else {
            incrementGenomCounter(cell);
          }
        } break;
        // Unconditional jump
        default: {
          addGenToCounter(cell);
        } break;
      }
    }
  }

  // Performing gamma flash
  gammaFlash();
}

void CellController::render(RenderingData *renderingData, int cellRenderingMode) {
  // Local constants
  static constexpr float kMinColor{0.0f};
  static constexpr float kHalfColor{0.5f};
  static constexpr float kThreeQuartersColor{0.75f};
  static constexpr float kMaxColor{1.0f};

  // Initializing count of RenderingData objects
  int renderingDataCount{};

  // Rendering each cell and putting its rendering data to array
  LinkedList<int>::Iterator iter{_cellIndexList.getIterator()};
  while (iter.hasNext()) {
    const Cell &cell = _cellVector[iter.next()];

    float colorR{}, colorG{}, colorB{};

    // If cell is alive
    if (cell._isAlive) {
      // Choosing appropriate cell rendering mode
      switch (cellRenderingMode) {
        // Diet mode
        case kRenderingModeDiet: {
          // Normalizing color and reducing it to range from 0 to 255
          colorR = static_cast<float>(cell._colorR);
          colorG = static_cast<float>(cell._colorG);
          colorB = static_cast<float>(cell._colorB);

          float colorVectorLength = std::sqrt(colorR * colorR + colorG * colorG + colorB * colorB);

          if (colorVectorLength < 1.0f) {
            colorR = kMinColor;
            colorG = kMinColor;
            colorB = kMinColor;
          } else {
            colorR /= colorVectorLength;
            colorG /= colorVectorLength;
            colorB /= colorVectorLength;
          }
        } break;
        // Energy level mode
        case kRenderingModeEnergyLevel: {
          colorR = 1.0f;
          colorG = map(cell._energy, 0.0f, _maxEnergy, kMaxColor, kMinColor);
          colorB = kMinColor;
        } break;
        // Energy sharing balance mode
        case kRenderingModeEnergySharingBalance: {
          colorR = map(cell._energyShareBalance, -_maxEnergy, _maxEnergy, kMaxColor, kMinColor);
          colorG = map(cell._energyShareBalance, -_maxEnergy, _maxEnergy, kHalfColor, kMaxColor);
          colorB = map(cell._energyShareBalance, -_maxEnergy, _maxEnergy, kMinColor, kMaxColor);
        } break;
        // Last energy share mode
        case kRenderingModeLastEnergyShare: {
          colorR = map(cell._lastEnergyShare, -1.0f, 1.0f, kMaxColor, kMinColor);
          colorG = map(cell._lastEnergyShare, -1.0f, 1.0f, kHalfColor, kMaxColor);
          colorB = map(cell._lastEnergyShare, -1.0f, 1.0f, kMinColor, kMaxColor);
        } break;
      }
    }
    // If cell is dead
    else {
      colorR = kThreeQuartersColor;
      colorG = kThreeQuartersColor;
      colorB = kThreeQuartersColor;
    }

    // Putting cell rendering data to array
    renderingData[renderingDataCount] =
        RenderingData{cell._index, colorR, colorG, colorB, kMaxColor};

    // Incrementing count of RenderingData objects
    ++renderingDataCount;
  }
}

size_t CellController::getCellCount() const noexcept { return _cellIndexList.count(); }

void CellController::updateTime() noexcept {
  // Updating ticks
  ++_ticksNumber;

  // Updating ticks and years if ticks overflow
  if (_ticksNumber == _dayDurationInTicks * _seasonDurationInDays * 4) {
    _ticksNumber = 0;
    ++_yearsNumber;
  }
}

void CellController::gammaFlash() noexcept {
  // If the time has come
  if (_ticksNumber % _dayDurationInTicks == 0 &&
      (_ticksNumber / _dayDurationInTicks + _yearsNumber * _seasonDurationInDays * 4) %
              _gammaFlashPeriodInDays ==
          0) {
    // For each cell
    LinkedList<int>::Iterator iter{_cellIndexList.getIterator()};
    while (iter.hasNext()) {
      Cell &cell = _cellVector[iter.next()];

      // Ignoring if cell is dead
      if (!cell._isAlive) {
        continue;
      }

      int mutationsCount = std::ceil(static_cast<float>(_gammaFlashMaxMutationsCount) *
                                     static_cast<float>(_mersenneTwisterEngine()) /
                                     static_cast<float>(_mersenneTwisterEngine.max()));
      for (int i = 0; i < mutationsCount; ++i) {
        mutateRandomGen(cell);
      }
    }
  }
}

void CellController::turn(Cell &cell) const noexcept {
  // Updating direction with overflow handling
  int deltaDirection = getNextNthGen(cell, 1);
  cell._direction    = (cell._direction + deltaDirection) % kDirectionCount;
}

void CellController::move(Cell &cell) noexcept {
  // Calculating coordinates by target direction
  int targetDirection{};
  // If given cell is alive
  if (cell._isAlive) {
    int deltaDirection = getNextNthGen(cell, 1);
    targetDirection    = (cell._direction + deltaDirection) % kDirectionCount;
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
  int targetIndex = calculateIndexByIndexAndDirection(cell._index, targetDirection);

  // If coordinates are beyond simulation world (above top or below bottom)
  if (targetIndex == -1) {
    return;
  }

  // If there is nothing at this direction
  if (_cellVector[targetIndex] == Cell{}) {
    // Moving cell

    // Order matters
    _cellVector[targetIndex] = std::move(cell);                            // 1
    _cellIndexList.replace(_cellVector[targetIndex]._index, targetIndex);  // 2
    _cellVector[targetIndex]._index = targetIndex;                         // 3
  }
  // If there is an obstalce and given cell is dead
  else if (!cell._isAlive) {
    // Making cell pinned
    cell._pinned = true;
  }
}

void CellController::getEnergyFromPhotosynthesis(Cell &cell) const noexcept {
  // Calculating energy from photosynthesis at current position
  int deltaEnergy = calculatePhotosynthesisEnergy(cell._index);

  // If energy from photosynthesis is positive
  if (deltaEnergy > 0) {
    // Increasing energy level
    cell._energy += deltaEnergy;

    // Making cell color more green
    ++cell._colorG;
  }
}

void CellController::getEnergyFromMinerals(Cell &cell) const noexcept {
  // Calculating energy from minerals at current position
  int deltaEnergy = calculateMineralEnergy(cell._index);

  // If energy from minerals is positive
  if (deltaEnergy > 0) {
    // Increasing energy level
    cell._energy += deltaEnergy;

    // Making cell color more blue
    ++cell._colorB;
  }
}

void CellController::getEnergyFromFood(Cell &cell) noexcept {
  // Calculating coordinates by current index and direction
  int targetIndex = calculateIndexByIndexAndDirection(cell._index, cell._direction);

  // If coordinates are beyond simulation world (above top or below bottom)
  if (targetIndex == -1) {
    return;
  }

  // Getting cell at this direction
  const Cell &targetCell = _cellVector[targetIndex];

  // If there is a cell or food
  if (targetCell != Cell{}) {
    // Calculating energy from food
    int deltaEnergy = std::min(targetCell._energy, _maxFoodEnergy);

    // Increasing energy level
    cell._energy += deltaEnergy;

    // Making cell color more red
    ++cell._colorR;

    // Removing prey or food
    removeCell(targetCell);
  }
}

void CellController::bud(Cell &cell) noexcept {
  // Checking and updating energy
  if (cell._energy < _minChildEnergy * 2) {
    return;
  }

  // Checking each direction clockwise for ability to bud
  for (int i = 0; i < kDirectionCount; ++i) {
    // Calculating coordinates by current index and direction
    int targetIndex =
        calculateIndexByIndexAndDirection(cell._index, (cell._direction + i) % kDirectionCount);

    // If coordinates are beyond simulation world (above top or below bottom)
    if (targetIndex == -1) {
      continue;
    }

    // If there is nothing at this direction
    if (_cellVector[targetIndex] == Cell{}) {
      // Creating new cell
      Cell buddedCell{cell._genom, cell._energy / 2, cell._direction, targetIndex};

      // Assigning cell color
      float colorVectorLength = static_cast<float>(std::sqrt(
          cell._colorR * cell._colorR + cell._colorG * cell._colorG + cell._colorB * cell._colorB));
      buddedCell._colorR      = static_cast<int>(static_cast<float>(cell._colorR) *
                                            kBuddedCellParentColorMultiplier / colorVectorLength);
      buddedCell._colorG      = static_cast<int>(static_cast<float>(cell._colorG) *
                                            kBuddedCellParentColorMultiplier / colorVectorLength);
      buddedCell._colorB      = static_cast<int>(static_cast<float>(cell._colorB) *
                                            kBuddedCellParentColorMultiplier / colorVectorLength);

      // Applying random bud mutation to the budded cell
      if (static_cast<float>(_mersenneTwisterEngine()) /
              static_cast<float>(_mersenneTwisterEngine.max()) <
          _budMutationChance) {
        mutateRandomGen(buddedCell);
      }

      // Applying random bud mutation to current cell
      cell._energy -= cell._energy / 2;
      if (static_cast<float>(_mersenneTwisterEngine()) /
              static_cast<float>(_mersenneTwisterEngine.max()) <
          _budMutationChance) {
        mutateRandomGen(cell);
      }

      addCell(buddedCell);

      return;
    }
  }

  // Turning cell into food if it have to but not able to bud
  cell._isAlive = false;
}

void CellController::mutateRandomGen(Cell &cell) noexcept {
  // Changing random gen on another random one
  cell._genom[std::floor(static_cast<float>(_mersenneTwisterEngine()) /
                         static_cast<float>(_mersenneTwisterEngine.max()) *
                         static_cast<float>(_genomSize))] =
      std::floor(static_cast<float>(_mersenneTwisterEngine()) /
                 static_cast<float>(_mersenneTwisterEngine.max()) * static_cast<float>(_genomSize));
}

void CellController::shareEnergy(Cell &cell) noexcept {
  // Calculating coordinates by current index and direction
  int targetIndex = calculateIndexByIndexAndDirection(cell._index, cell._direction);

  // If coordinates are beyond simulation world (above top or below bottom)
  if (targetIndex == -1) {
    return;
  }

  // Getting cell at this direction
  Cell &targetCell = _cellVector[targetIndex];

  // If there is a cell
  if (targetCell != Cell{} && targetCell._isAlive) {
    // Calculating energy to share
    int deltaEnergy = static_cast<int>(static_cast<float>(cell._energy * getNextNthGen(cell, 1)) /
                                       static_cast<float>(_genomSize));

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

void CellController::lookForward(Cell &cell) const noexcept {
  // Calculating coordinates by current index and direction
  int targetIndex = calculateIndexByIndexAndDirection(cell._index, cell._direction);

  // If coordinates are beyond simulation world (above top or below bottom)
  if (targetIndex == -1) {
    return;
  }

  // Getting cell at this direction
  const Cell &targetCell = _cellVector[targetIndex];

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

void CellController::determineEnergyLevel(Cell &cell) const noexcept {
  // Calculating value to compare
  int valueToCompare = static_cast<int>(static_cast<float>(_maxEnergy * getNextNthGen(cell, 1)) /
                                        static_cast<float>(_genomSize));

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

void CellController::determineDepth(Cell &cell) const noexcept {
  // Calculating values to compare
  int row            = calculateRowByIndex(cell._index);
  int valueToCompare = static_cast<int>(static_cast<float>(_rows * getNextNthGen(cell, 1)) /
                                        static_cast<float>(_genomSize));

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

void CellController::determinePhotosynthesisEnergy(Cell &cell) const noexcept {
  // Calculating value to compare
  int valueToCompare =
      static_cast<int>(static_cast<float>(_maxPhotosynthesisEnergy * getNextNthGen(cell, 1)) /
                       static_cast<float>(_genomSize));

  // Calculating available photosynthesis energy
  int deltaEnergy = calculatePhotosynthesisEnergy(cell._index);

  // Less
  if (deltaEnergy < valueToCompare) {
    jumpCounter(cell, getNextNthGen(cell, 2));
  }
  // Greater
  else if (deltaEnergy > valueToCompare) {
    jumpCounter(cell, getNextNthGen(cell, 4));
  }
  // Equal
  else {
    jumpCounter(cell, getNextNthGen(cell, 3));
  }
}

void CellController::determineMineralEnergy(Cell &cell) const noexcept {
  // Calculating value to compare
  int valueToCompare =
      static_cast<int>(static_cast<float>(_maxMineralEnergy * getNextNthGen(cell, 1)) /
                       static_cast<float>(_genomSize));

  // Calculating available mineral energy
  int deltaEnergy = calculateMineralEnergy(cell._index);

  // Less
  if (deltaEnergy < valueToCompare) {
    jumpCounter(cell, getNextNthGen(cell, 2));
  }
  // Greater
  else if (deltaEnergy > valueToCompare) {
    jumpCounter(cell, getNextNthGen(cell, 4));
  }
  // Equal
  else {
    jumpCounter(cell, getNextNthGen(cell, 3));
  }
}

void CellController::incrementGenomCounter(Cell &cell) const noexcept {
  // Incrementing instruction counter with overflow handling
  cell._counter = (cell._counter + 1) % _genomSize;
}

void CellController::addGenToCounter(Cell &cell) const noexcept {
  // Adding dummy instruction value to instruction counter with overflow handling
  cell._counter = (cell._counter + cell._genom[cell._counter]) % _genomSize;
}

void CellController::jumpCounter(Cell &cell, int offset) const noexcept {
  // Performing jump command on instruction counter with overflow handling
  cell._counter = (cell._counter + offset) % _genomSize;
}

int CellController::getNextNthGen(const Cell &cell, int n) const noexcept {
  // Getting (counter + n)'th gen
  return cell._genom[(cell._counter + n) % _genomSize];
}

bool CellController::areAkin(const Cell &cell1, const Cell &cell2) const noexcept {
  int diff{};

  for (int i = 0; i < _genomSize; ++i) {
    if (cell1._genom[i] != cell2._genom[i]) {
      ++diff;
      if (diff > _maxAkinGenomDifference) {
        return false;
      }
    }
  }

  return true;
}

int CellController::calculatePhotosynthesisEnergy(int index) const noexcept {
  // Local constants
  static constexpr float kSeasonCount{4.0f};
  static constexpr float kHalfOfDay{0.5f};

  // Calculating cell column and row
  int column = calculateColumnByIndex(index);
  int row    = calculateRowByIndex(index);

  // Calculating daytime coefficient if needed
  float daytimeCoefficient = 1.0f;
  if (_enableDaytimes) {
    // Calculation sun epicenter position along X-axis
    int sunPosition = static_cast<int>(
        map(_ticksNumber % _dayDurationInTicks, 0.0f, _dayDurationInTicks - 1, 0.0f, _columns - 1));
    // Calculation minimal or maximal distance to sun epicenter along X-axis
    int distanceToSun = std::abs(sunPosition - column);
    // Calculation minimal distance to sun epicenter along X-axis
    int minDistanceToSun = std::min(distanceToSun, _columns - 1 - distanceToSun);
    // Calculation sunny area width based on current season if needed
    float daytimeWidthRatio = 1.0f - _summerDaytimeToWholeDayRatio;
    if (_enableSeasons) {
      daytimeWidthRatio =
          map(std::sin(map(_ticksNumber / _dayDurationInTicks / _seasonDurationInDays, 0.0f,
                           kSeasonCount, 0.0f, kTwoPi)),
              -1.0f, 1.0f, 1.0f - _summerDaytimeToWholeDayRatio, _summerDaytimeToWholeDayRatio);
    }
    // Calculating daytime coefficient
    daytimeCoefficient = (static_cast<float>(minDistanceToSun) <
                          static_cast<float>(_columns - 1) * kHalfOfDay * daytimeWidthRatio)
                             ? 1.0f
                             : 0.0f;

    // If it is night here
    if (daytimeCoefficient < 1.0f) {
      return 0;
    }
  }

  // Calculating depth coefficient
  float depthCoefficient = mapClamp(row, 0.0f, _maxPhotosynthesisDepth, 1.0f, 0.0f);

  // Getting energy from photosynthesis
  return static_cast<int>(
      (static_cast<float>(_maxPhotosynthesisEnergy) * depthCoefficient * daytimeCoefficient));
}

int CellController::calculateMineralEnergy(int index) const noexcept {
  // Calculating cell row
  int row = calculateRowByIndex(index);

  // Getting energy from minerals
  return static_cast<int>(
      mapClamp(row, _rows - 1, _rows - 1 - _maxMineralHeight, _maxMineralEnergy, 0.0f));
}

int CellController::calculateColumnByIndex(int index) const noexcept {
  return index - index / _columns * _columns;
}

int CellController::calculateRowByIndex(int index) const noexcept { return index / _columns; }

int CellController::calculateIndexByColumnAndRow(int column, int row) const noexcept {
  return row * _columns + column;
}

int CellController::calculateIndexByIndexAndDirection(int index, int direction) const noexcept {
  // Calculating row at given direction with overflow handling
  int row = calculateRowByIndex(index);
  int r   = row + kDirectionOffsets.at(direction).at(1);
  if (r > _rows - 1 || r < 0) {
    return -1;
  }

  // Calculating column at given direction with overflow handling
  int column = calculateColumnByIndex(index);
  int c      = (column + kDirectionOffsets.at(direction).at(0) + _columns) % _columns;

  return calculateIndexByColumnAndRow(c, r);
}

void CellController::addCell(const Cell &cell) noexcept {
  // Pushing cell to the front of the linked list
  // so it will be processed not earlier than the next tick
  // and before older cells (younger cells have smaller "reaction time")

  // Order matters
  _cellIndexList.pushFront(cell._index);  // 1
  _cellVector[cell._index] = cell;        // 2
}

void CellController::removeCell(const Cell &cell) noexcept {
  // Order matters
  _cellIndexList.remove(cell._index);  // 1
  _cellVector[cell._index] = Cell{};   // 2
}
