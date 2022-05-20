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

// Header file
#include "./cell_controller.hpp"

// STD
#include <algorithm>
#include <array>
#include <cmath>
#include <utility>

// Using header file namespace
using namespace CellEvolution;

// Position offsets per 2D direction
static constexpr int                                             kDirectionCount = 8;
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

// Last energy share value fade multiplier
static constexpr float kLastEnergyShareFadeMultiplier = 0.99f;
// Budded cell parent color multiplier
static constexpr float kBuddedCellParentColorMultiplier = 2.0f;

// Mathematical constant
static constexpr float kTwoPi = 6.28318530f;

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
      _columns(static_cast<int>(static_cast<float>(params.width) / params.cellSize)),
      _rows(static_cast<int>(static_cast<float>(params.height) / params.cellSize)),
      _genomSize{params.genomSize},
      _maxInstructionsPerTick{params.maxInstructionsPerTick},
      _maxAkinGenomDifference{params.maxAkinGenomDifference},
      _minChildEnergy{params.minChildEnergy},
      _maxEnergy{params.maxEnergy},
      _maxBurstOfPhotosynthesisEnergy{params.maxBurstOfPhotosynthesisEnergy},
      _maxPhotosynthesisDepth(
          static_cast<int>(static_cast<float>(_rows) * params.maxPhotosynthesisDepthMultiplier)),
      _summerDaytimeToWholeDayRatio{params.summerDaytimeToWholeDayRatio},
      _maxMinerals{params.maxMinerals},
      _maxBurstOfMinerals{params.maxBurstOfMinerals},
      _energyPerMineral{params.energyPerMineral},
      _maxMineralHeight(
          static_cast<int>(static_cast<float>(_rows) * params.maxMineralHeightMultiplier)),
      _maxBurstOfFoodEnergy{params.maxBurstOfFoodEnergy},
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
      _enableInstructionTouch{params.enableInstructionTouch},
      _enableInstructionDetermineEnergyLevel{params.enableInstructionDetermineEnergyLevel},
      _enableInstructionDetermineDepth{params.enableInstructionDetermineDepth},
      _enableInstructionDetermineBurstOfPhotosynthesisEnergy{
          params.enableInstructionDetermineBurstOfPhotosynthesisEnergy},
      _enableInstructionDetermineBurstOfMinerals{params.enableInstructionDetermineBurstOfMinerals},
      _enableInstructionDetermineBurstOfMineralEnergy{
          params.enableInstructionDetermineBurstOfMineralEnergy},
      _enableZeroEnergyOrganic{params.enableZeroEnergyOrganic},
      _enableForcedBuddingOnMaximalEnergyLevel{params.enableForcedBuddingOnMaximalEnergyLevel},
      _enableTryingToBudInUnoccupiedDirection{params.enableTryingToBudInUnoccupiedDirection},
      _enableDeathOnBuddingIfNotEnoughSpace{params.enableDeathOnBuddingIfNotEnoughSpace},
      _enableSeasons{params.enableSeasons},
      _enableDaytimes{params.enableDaytimes},
      _enableMaximizingFoodEnergy{params.enableMaximizingFoodEnergy},
      _enableDeadCellPinningOnSinking{params.enableDeadCellPinningOnSinking} {
  // Allocating memory for vector of cell pointers
  const int maxCellCount{_columns * _rows};
  for (int i = 0; i < maxCellCount; ++i) {
    _cellPtrVector.push_back(std::unique_ptr<Cell>{});
  }
  _cellPtrVector.reserve(maxCellCount);

  // Creating the first cell genom
  std::vector<int> firstCellGenom;
  const size_t     paramsFirstCellGenomSize  = params.firstCellGenom.size();
  const int firstCellDefaultGenomInstruction = params.firstCellGenom[paramsFirstCellGenomSize - 1];
  firstCellGenom.assign(_genomSize, firstCellDefaultGenomInstruction);
  for (size_t i = 0; i < paramsFirstCellGenomSize; ++i) {
    firstCellGenom[i] = params.firstCellGenom[i];
  }

  // Constructing and adding the first cell
  addCell(std::make_unique<Cell>(
      firstCellGenom,
      static_cast<int>(static_cast<float>(_minChildEnergy) * params.firstCellEnergyMultiplier),
      params.firstCellDirection,
      static_cast<int>(static_cast<float>(_columns) * params.firstCellIndexMultiplier)));
}

CellController::CellController(const CellController &cellController) noexcept
    : _mersenneTwisterEngine{cellController._mersenneTwisterEngine},
      _randomSeed{cellController._randomSeed},
      _columns{cellController._columns},
      _rows{cellController._rows},
      _genomSize{cellController._genomSize},
      _maxInstructionsPerTick{cellController._maxInstructionsPerTick},
      _maxAkinGenomDifference{cellController._maxAkinGenomDifference},
      _minChildEnergy{cellController._minChildEnergy},
      _maxEnergy{cellController._maxEnergy},
      _maxBurstOfPhotosynthesisEnergy{cellController._maxBurstOfPhotosynthesisEnergy},
      _maxPhotosynthesisDepth{cellController._maxPhotosynthesisDepth},
      _summerDaytimeToWholeDayRatio{cellController._summerDaytimeToWholeDayRatio},
      _maxMinerals{cellController._maxMinerals},
      _maxBurstOfMinerals{cellController._maxBurstOfMinerals},
      _energyPerMineral{cellController._energyPerMineral},
      _maxMineralHeight{cellController._maxMineralHeight},
      _maxBurstOfFoodEnergy{cellController._maxBurstOfFoodEnergy},
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
      _enableInstructionTouch{cellController._enableInstructionTouch},
      _enableInstructionDetermineEnergyLevel{cellController._enableInstructionDetermineEnergyLevel},
      _enableInstructionDetermineDepth{cellController._enableInstructionDetermineDepth},
      _enableInstructionDetermineBurstOfPhotosynthesisEnergy{
          cellController._enableInstructionDetermineBurstOfPhotosynthesisEnergy},
      _enableInstructionDetermineBurstOfMinerals{
          cellController._enableInstructionDetermineBurstOfMinerals},
      _enableInstructionDetermineBurstOfMineralEnergy{
          cellController._enableInstructionDetermineBurstOfMineralEnergy},
      _enableZeroEnergyOrganic{cellController._enableZeroEnergyOrganic},
      _enableForcedBuddingOnMaximalEnergyLevel{
          cellController._enableForcedBuddingOnMaximalEnergyLevel},
      _enableTryingToBudInUnoccupiedDirection{
          cellController._enableTryingToBudInUnoccupiedDirection},
      _enableDeathOnBuddingIfNotEnoughSpace{cellController._enableDeathOnBuddingIfNotEnoughSpace},
      _enableSeasons{cellController._enableSeasons},
      _enableDaytimes{cellController._enableDaytimes},
      _enableMaximizingFoodEnergy{cellController._enableMaximizingFoodEnergy},
      _enableDeadCellPinningOnSinking{cellController._enableDeadCellPinningOnSinking},
      _cellIndexList{cellController._cellIndexList},
      // Copying _cellPtrVector in body
      _ticksNumber{cellController._ticksNumber},
      _yearsNumber{cellController._yearsNumber} {
  // Allocating memory for vector of cell pointers
  const int maxCellCount{_columns * _rows};
  for (int i = 0; i < maxCellCount; ++i) {
    _cellPtrVector.push_back(std::unique_ptr<Cell>{});
  }
  _cellPtrVector.reserve(maxCellCount);

  // Copying _cellPtrVector
  LinkedList<int>::Iterator iter{cellController._cellIndexList.getIterator()};
  while (iter.hasNext()) {
    const int index{iter.next()};
    _cellPtrVector[index] = std::make_unique<Cell>(*cellController._cellPtrVector[index]);
  }
}

CellController &CellController::operator=(const CellController &cellController) noexcept {
  _mersenneTwisterEngine          = cellController._mersenneTwisterEngine;
  _randomSeed                     = cellController._randomSeed;
  _columns                        = cellController._columns;
  _rows                           = cellController._rows;
  _genomSize                      = cellController._genomSize;
  _maxInstructionsPerTick         = cellController._maxInstructionsPerTick;
  _maxAkinGenomDifference         = cellController._maxAkinGenomDifference;
  _minChildEnergy                 = cellController._minChildEnergy;
  _maxEnergy                      = cellController._maxEnergy;
  _maxBurstOfPhotosynthesisEnergy = cellController._maxBurstOfPhotosynthesisEnergy;
  _maxPhotosynthesisDepth         = cellController._maxPhotosynthesisDepth;
  _summerDaytimeToWholeDayRatio   = cellController._summerDaytimeToWholeDayRatio;
  _maxMinerals                    = cellController._maxMinerals;
  _maxBurstOfMinerals             = cellController._maxBurstOfMinerals;
  _energyPerMineral               = cellController._energyPerMineral;
  _maxMineralHeight               = cellController._maxMineralHeight;
  _maxBurstOfFoodEnergy           = cellController._maxBurstOfFoodEnergy;
  _randomMutationChance           = cellController._randomMutationChance;
  _budMutationChance              = cellController._budMutationChance;
  _dayDurationInTicks             = cellController._dayDurationInTicks;
  _seasonDurationInDays           = cellController._seasonDurationInDays;
  _gammaFlashPeriodInDays         = cellController._gammaFlashPeriodInDays;
  _gammaFlashMaxMutationsCount    = cellController._gammaFlashMaxMutationsCount;
  _enableInstructionTurn          = cellController._enableInstructionTurn;
  _enableInstructionMove          = cellController._enableInstructionMove;
  _enableInstructionGetEnergyFromPhotosynthesis =
      cellController._enableInstructionGetEnergyFromPhotosynthesis;
  _enableInstructionGetEnergyFromMinerals = cellController._enableInstructionGetEnergyFromMinerals;
  _enableInstructionGetEnergyFromFood     = cellController._enableInstructionGetEnergyFromFood;
  _enableInstructionBud                   = cellController._enableInstructionBud;
  _enableInstructionMutateRandomGen       = cellController._enableInstructionMutateRandomGen;
  _enableInstructionShareEnergy           = cellController._enableInstructionShareEnergy;
  _enableInstructionTouch                 = cellController._enableInstructionTouch;
  _enableInstructionDetermineEnergyLevel  = cellController._enableInstructionDetermineEnergyLevel;
  _enableInstructionDetermineDepth        = cellController._enableInstructionDetermineDepth;
  _enableInstructionDetermineBurstOfPhotosynthesisEnergy =
      cellController._enableInstructionDetermineBurstOfPhotosynthesisEnergy;
  _enableInstructionDetermineBurstOfMinerals =
      cellController._enableInstructionDetermineBurstOfMinerals;
  _enableInstructionDetermineBurstOfMineralEnergy =
      cellController._enableInstructionDetermineBurstOfMineralEnergy;
  _enableZeroEnergyOrganic = cellController._enableZeroEnergyOrganic;
  _enableForcedBuddingOnMaximalEnergyLevel =
      cellController._enableForcedBuddingOnMaximalEnergyLevel;
  _enableTryingToBudInUnoccupiedDirection = cellController._enableTryingToBudInUnoccupiedDirection;
  _enableDeathOnBuddingIfNotEnoughSpace   = cellController._enableDeathOnBuddingIfNotEnoughSpace;
  _enableSeasons                          = cellController._enableSeasons;
  _enableDaytimes                         = cellController._enableDaytimes;
  _enableMaximizingFoodEnergy             = cellController._enableMaximizingFoodEnergy;
  _enableDeadCellPinningOnSinking         = cellController._enableDeadCellPinningOnSinking;
  _cellIndexList                          = cellController._cellIndexList;

  // Allocating memory for vector of cell pointers
  const int maxCellCount{_columns * _rows};
  for (int i = 0; i < maxCellCount; ++i) {
    _cellPtrVector.push_back(std::unique_ptr<Cell>{});
  }
  _cellPtrVector.reserve(maxCellCount);

  // Copying _cellPtrVector
  LinkedList<int>::Iterator iter{cellController._cellIndexList.getIterator()};
  while (iter.hasNext()) {
    const int index{iter.next()};
    _cellPtrVector[index] = std::make_unique<Cell>(*cellController._cellPtrVector[index]);
  }

  _ticksNumber = cellController._ticksNumber;
  _yearsNumber = cellController._yearsNumber;

  return *this;
}

CellController::CellController(CellController &&cellController) noexcept
    : _mersenneTwisterEngine{std::exchange(cellController._mersenneTwisterEngine, std::mt19937{})},
      _randomSeed{std::exchange(cellController._randomSeed, 0)},
      _columns{std::exchange(cellController._columns, 0)},
      _rows{std::exchange(cellController._rows, 0)},
      _genomSize{std::exchange(cellController._genomSize, 0)},
      _maxInstructionsPerTick{std::exchange(cellController._maxInstructionsPerTick, 0)},
      _maxAkinGenomDifference{std::exchange(cellController._maxAkinGenomDifference, 0)},
      _minChildEnergy{std::exchange(cellController._minChildEnergy, 0)},
      _maxEnergy{std::exchange(cellController._maxEnergy, 0)},
      _maxBurstOfPhotosynthesisEnergy{
          std::exchange(cellController._maxBurstOfPhotosynthesisEnergy, 0)},
      _maxPhotosynthesisDepth{std::exchange(cellController._maxPhotosynthesisDepth, 0)},
      _summerDaytimeToWholeDayRatio{
          std::exchange(cellController._summerDaytimeToWholeDayRatio, 0.0f)},
      _maxMinerals{std::exchange(cellController._maxMinerals, 0)},
      _maxBurstOfMinerals{std::exchange(cellController._maxBurstOfMinerals, 0)},
      _energyPerMineral{std::exchange(cellController._energyPerMineral, 0)},
      _maxMineralHeight{std::exchange(cellController._maxMineralHeight, 0)},
      _maxBurstOfFoodEnergy{std::exchange(cellController._maxBurstOfFoodEnergy, 0)},
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
      _enableInstructionTouch{std::exchange(cellController._enableInstructionTouch, false)},
      _enableInstructionDetermineEnergyLevel{
          std::exchange(cellController._enableInstructionDetermineEnergyLevel, false)},
      _enableInstructionDetermineDepth{
          std::exchange(cellController._enableInstructionDetermineDepth, false)},
      _enableInstructionDetermineBurstOfPhotosynthesisEnergy{std::exchange(
          cellController._enableInstructionDetermineBurstOfPhotosynthesisEnergy, false)},
      _enableInstructionDetermineBurstOfMinerals{
          std::exchange(cellController._enableInstructionDetermineBurstOfMinerals, false)},
      _enableInstructionDetermineBurstOfMineralEnergy{
          std::exchange(cellController._enableInstructionDetermineBurstOfMineralEnergy, false)},
      _enableZeroEnergyOrganic{std::exchange(cellController._enableZeroEnergyOrganic, false)},
      _enableForcedBuddingOnMaximalEnergyLevel{
          std::exchange(cellController._enableForcedBuddingOnMaximalEnergyLevel, false)},
      _enableTryingToBudInUnoccupiedDirection{
          std::exchange(cellController._enableTryingToBudInUnoccupiedDirection, false)},
      _enableDeathOnBuddingIfNotEnoughSpace{
          std::exchange(cellController._enableDeathOnBuddingIfNotEnoughSpace, false)},
      _enableSeasons{std::exchange(cellController._enableSeasons, false)},
      _enableDaytimes{std::exchange(cellController._enableDaytimes, false)},
      _enableMaximizingFoodEnergy{std::exchange(cellController._enableMaximizingFoodEnergy, false)},
      _enableDeadCellPinningOnSinking{
          std::exchange(cellController._enableDeadCellPinningOnSinking, false)},
      _cellIndexList{std::exchange(cellController._cellIndexList, LinkedList<int>{})},
      _cellPtrVector{
          std::exchange(cellController._cellPtrVector, std::vector<std::unique_ptr<Cell>>{})},
      _ticksNumber{std::exchange(cellController._ticksNumber, 0)},
      _yearsNumber{std::exchange(cellController._yearsNumber, 0)} {}

CellController &CellController::operator=(CellController &&cellController) noexcept {
  std::swap(_mersenneTwisterEngine, cellController._mersenneTwisterEngine);
  std::swap(_randomSeed, cellController._randomSeed);
  std::swap(_columns, cellController._columns);
  std::swap(_rows, cellController._rows);
  std::swap(_genomSize, cellController._genomSize);
  std::swap(_maxInstructionsPerTick, cellController._maxInstructionsPerTick);
  std::swap(_maxAkinGenomDifference, cellController._maxAkinGenomDifference);
  std::swap(_minChildEnergy, cellController._minChildEnergy);
  std::swap(_maxEnergy, cellController._maxEnergy);
  std::swap(_maxBurstOfPhotosynthesisEnergy, cellController._maxBurstOfPhotosynthesisEnergy);
  std::swap(_maxPhotosynthesisDepth, cellController._maxPhotosynthesisDepth);
  std::swap(_summerDaytimeToWholeDayRatio, cellController._summerDaytimeToWholeDayRatio);
  std::swap(_maxMinerals, cellController._maxMinerals);
  std::swap(_maxBurstOfMinerals, cellController._maxBurstOfMinerals);
  std::swap(_energyPerMineral, cellController._energyPerMineral);
  std::swap(_maxMineralHeight, cellController._maxMineralHeight);
  std::swap(_maxBurstOfFoodEnergy, cellController._maxBurstOfFoodEnergy);
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
  std::swap(_enableInstructionTouch, cellController._enableInstructionTouch);
  std::swap(_enableInstructionDetermineEnergyLevel,
            cellController._enableInstructionDetermineEnergyLevel);
  std::swap(_enableInstructionDetermineDepth, cellController._enableInstructionDetermineDepth);
  std::swap(_enableInstructionDetermineBurstOfPhotosynthesisEnergy,
            cellController._enableInstructionDetermineBurstOfPhotosynthesisEnergy);
  std::swap(_enableInstructionDetermineBurstOfMinerals,
            cellController._enableInstructionDetermineBurstOfMinerals);
  std::swap(_enableInstructionDetermineBurstOfMineralEnergy,
            cellController._enableInstructionDetermineBurstOfMineralEnergy);
  std::swap(_enableZeroEnergyOrganic, cellController._enableZeroEnergyOrganic);
  std::swap(_enableForcedBuddingOnMaximalEnergyLevel,
            cellController._enableForcedBuddingOnMaximalEnergyLevel);
  std::swap(_enableTryingToBudInUnoccupiedDirection,
            cellController._enableTryingToBudInUnoccupiedDirection);
  std::swap(_enableDeathOnBuddingIfNotEnoughSpace,
            cellController._enableDeathOnBuddingIfNotEnoughSpace);
  std::swap(_enableSeasons, cellController._enableSeasons);
  std::swap(_enableDaytimes, cellController._enableDaytimes);
  std::swap(_enableMaximizingFoodEnergy, cellController._enableMaximizingFoodEnergy);
  std::swap(_enableDeadCellPinningOnSinking, cellController._enableDeadCellPinningOnSinking);
  std::swap(_cellIndexList, cellController._cellIndexList);
  std::swap(_cellPtrVector, cellController._cellPtrVector);
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
    const int index = iter.next();
    Cell     &cell  = *_cellPtrVector[index];

    // Moving cell if it is dead (organic)
    if (!cell._isAlive) {
      move(cell);
      continue;
    }

    // Updating cell energy
    cell._energy--;
    // If cell energy is less than one
    if (cell._energy <= 0) {
      // Turning cell into organic if zero energy organic is enabled
      if (_enableZeroEnergyOrganic) {
        cell._isAlive = false;
      }
      // Removing cell if zero energy organic is disabled
      else {
        removeCell(std::move(_cellPtrVector[index]));
      }

      continue;
    }
    // Making cell bud if its energy greater or equals to maximal
    if (_enableForcedBuddingOnMaximalEnergyLevel && cell._energy >= _maxEnergy - 1) {
      bud(cell);
      continue;
    }

    // Updating cell minerals
    cell._minerals += calculateBurstOfMinerals(cell._index);
    if (cell._minerals > _maxMinerals) {
      cell._minerals = _maxMinerals;
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
      switch (static_cast<CellInstructions>(instuction)) {
        // Do nothing
        case CellInstructions::DoNothing: {
          incrementGenomCounter(cell);
        } break;
        // Turning
        case CellInstructions::Turn: {
          if (_enableInstructionTurn) {
            turn(cell);
          }
          incrementGenomCounter(cell);
        } break;
        // Moving (no more instructions permitted)
        case CellInstructions::Move: {
          if (_enableInstructionMove) {
            i = _maxInstructionsPerTick;
            move(cell);
          }
          incrementGenomCounter(cell);
        } break;
        // Getting energy from photosynthesis (no more instructions permitted)
        case CellInstructions::GetEnergyFromPhotosynthesis: {
          if (_enableInstructionGetEnergyFromPhotosynthesis) {
            i = _maxInstructionsPerTick;
            getEnergyFromPhotosynthesis(cell);
          }
          incrementGenomCounter(cell);
        } break;
        // Getting energy from minerals (no more instructions permitted)
        case CellInstructions::GetEnergyFromMinerals: {
          if (_enableInstructionGetEnergyFromMinerals) {
            i = _maxInstructionsPerTick;
            getEnergyFromMinerals(cell);
          }
          incrementGenomCounter(cell);
        } break;
        // Getting energy from food (no more instructions permitted)
        case CellInstructions::GetEnergyFromFood: {
          if (_enableInstructionGetEnergyFromFood) {
            i = _maxInstructionsPerTick;
            getEnergyFromFood(cell);
          }
          incrementGenomCounter(cell);
        } break;
        // Budding (no more instructions permitted)
        case CellInstructions::Bud: {
          if (_enableInstructionBud) {
            i = _maxInstructionsPerTick;
            bud(cell);
          }
          incrementGenomCounter(cell);
        } break;
        // Making random gen mutate (no more instructions permitted)
        case CellInstructions::MutateRandomGen: {
          if (_enableInstructionMutateRandomGen) {
            i = _maxInstructionsPerTick;
            mutateRandomGen(cell);
          }
          incrementGenomCounter(cell);
        } break;
        // Sharing energy (no more instructions permitted)
        case CellInstructions::ShareEnergy: {
          if (_enableInstructionShareEnergy) {
            i = _maxInstructionsPerTick;
            shareEnergy(cell);
          }
          incrementGenomCounter(cell);
        } break;
        // Looking forward (conditional instruction)
        case CellInstructions::Touch: {
          if (_enableInstructionTouch) {
            touch(cell);
          } else {
            incrementGenomCounter(cell);
          }
        } break;
        // Determining own energy level (conditional instruction)
        case CellInstructions::DetermineEnergyLevel: {
          if (_enableInstructionDetermineEnergyLevel) {
            determineEnergyLevel(cell);
          } else {
            incrementGenomCounter(cell);
          }
        } break;
        // Determining own depth (conditional instruction)
        case CellInstructions::DetermineDepth: {
          if (_enableInstructionDetermineDepth) {
            determineDepth(cell);
          } else {
            incrementGenomCounter(cell);
          }
        } break;
        // Determining available energy from photosynthesis (conditional instruction)
        case CellInstructions::DetermineBurstOfPhotosynthesisEnergy: {
          if (_enableInstructionDetermineBurstOfPhotosynthesisEnergy) {
            determineBurstOfPhotosynthesisEnergy(cell);
          } else {
            incrementGenomCounter(cell);
          }
        } break;
        // Determining available minerals (conditional instruction)
        case CellInstructions::DetermineBurstOfMinerals: {
          if (_enableInstructionDetermineBurstOfMinerals) {
            determineBurstOfMinerals(cell);
          } else {
            incrementGenomCounter(cell);
          }
        } break;
        // Determining available energy from minerals (conditional instruction)
        case CellInstructions::DetermineBurstOfMineralEnergy: {
          if (_enableInstructionDetermineBurstOfMineralEnergy) {
            determineBurstOfMineralEnergy(cell);
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

void CellController::render(CellRenderingData *cellRenderingData, int cellRenderingMode) {
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
    const Cell &cell = *_cellPtrVector[iter.next()];

    float colorR{}, colorG{}, colorB{};

    // If cell is alive
    if (cell._isAlive) {
      // Choosing appropriate cell rendering mode
      switch (static_cast<CellRenderingModes>(cellRenderingMode)) {
        // Diet mode
        case CellRenderingModes::Diet: {
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
        case CellRenderingModes::EnergyLevel: {
          colorR = kMaxColor;
          colorG = map(cell._energy, 0.0f, _maxEnergy, kMaxColor, kMinColor);
          colorB = kMinColor;
        } break;
        // Energy sharing balance mode
        case CellRenderingModes::EnergySharingBalance: {
          colorR = map(cell._energyShareBalance, -_maxEnergy, _maxEnergy, kMaxColor, kMinColor);
          colorG = map(cell._energyShareBalance, -_maxEnergy, _maxEnergy, kHalfColor, kMaxColor);
          colorB = map(cell._energyShareBalance, -_maxEnergy, _maxEnergy, kMinColor, kMaxColor);
        } break;
        // Last energy share mode
        case CellRenderingModes::LastEnergyShare: {
          colorR = map(cell._lastEnergyShare, -1.0f, 1.0f, kMaxColor, kMinColor);
          colorG = map(cell._lastEnergyShare, -1.0f, 1.0f, kHalfColor, kMaxColor);
          colorB = map(cell._lastEnergyShare, -1.0f, 1.0f, kMinColor, kMaxColor);
        } break;
      }
    }
    // If cell is dead (organic)
    else {
      colorR = kThreeQuartersColor;
      colorG = kThreeQuartersColor;
      colorB = kThreeQuartersColor;
    }

    // Putting cell rendering data to array
    cellRenderingData[renderingDataCount] = CellRenderingData{cell._index, colorR, colorG, colorB};

    // Incrementing count of RenderingData objects
    ++renderingDataCount;
  }
}

size_t CellController::getCellCount() const noexcept { return _cellIndexList.count(); }

float CellController::getSunPosition() const noexcept {
  if (!_enableDaytimes) {
    return 0.0f;
  }

  return map(_ticksNumber % _dayDurationInTicks, 0.0f, _dayDurationInTicks - 1, -1.0f, 1.0f);
}

float CellController::getDaytimeWidth() const noexcept {
  // Local constant
  static constexpr float kSeasonCount{4.0f};

  if (!_enableDaytimes) {
    return 1.0f;
  }

  float daytimeWidthRatio = 1.0f - _summerDaytimeToWholeDayRatio;

  if (_enableSeasons) {
    daytimeWidthRatio =
        map(std::sin(map(_ticksNumber / _dayDurationInTicks / _seasonDurationInDays, 0.0f,
                         kSeasonCount, 0.0f, kTwoPi)),
            -1.0f, 1.0f, 1.0f - _summerDaytimeToWholeDayRatio, _summerDaytimeToWholeDayRatio);
  }

  return daytimeWidthRatio;
}

int CellController::getColumns() const noexcept { return _columns; }

int CellController::getRows() const noexcept { return _rows; }

int CellController::getMaxPhotosynthesisDepth() const noexcept { return _maxPhotosynthesisDepth; }

int CellController::getMaxMineralHeight() const noexcept { return _maxMineralHeight; }

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
      Cell &cell = *_cellPtrVector[iter.next()];

      // Ignoring if cell is dead (organic)
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
  // If given cell is dead (organic)
  else {
    // If given cell is pinned
    if (_enableDeadCellPinningOnSinking && cell._pinned) {
      return;
    }

    // Setting direction to 4 so it will be moving down (sinking) each tick if cell is dead (organic) and not pinned yet
    targetDirection = 4;
  }
  int targetIndex = calculateIndexByIndexAndDirection(cell._index, targetDirection);

  // If coordinates are beyond simulation world (above top or below bottom)
  if (targetIndex == -1) {
    return;
  }

  // If there is nothing at this direction
  if (_cellPtrVector[targetIndex] == nullptr) {
    // Moving cell

    // Order matters
    _cellPtrVector[targetIndex] = std::move(_cellPtrVector[cell._index]);      // 1
    _cellIndexList.replace(_cellPtrVector[targetIndex]->_index, targetIndex);  // 2
    _cellPtrVector[targetIndex]->_index = targetIndex;                         // 3
  }
  // If there is an obstacle
  else {
    // If given cell is dead (organic)
    if (!cell._isAlive) {
      // Making cell pinned
      cell._pinned = true;
    }
    // If given cell is alive
    else {
      // Making cell know what has it collided with
      touch(cell);
      // Reverting move command counter incremention
      --cell._counter;
    }
  }
}

void CellController::getEnergyFromPhotosynthesis(Cell &cell) const noexcept {
  // Calculating energy from photosynthesis at index
  int deltaEnergy = calculateBurstOfPhotosynthesisEnergy(cell._index);

  // If energy from photosynthesis is positive
  if (deltaEnergy > 0) {
    // Increasing energy level
    cell._energy += deltaEnergy;

    // Making cell color more green
    ++cell._colorG;
  }
}

void CellController::getEnergyFromMinerals(Cell &cell) const noexcept {
  // Calculating energy from minerals at index
  int deltaEnergy = calculateBurstOfMineralEnergy(cell._minerals);

  // If energy from minerals is positive
  if (deltaEnergy > 0) {
    // Increasing energy level
    cell._energy += deltaEnergy;

    // Converting all the minerals
    cell._minerals = 0;

    // Making cell color more blue
    ++cell._colorB;
  }
}

void CellController::getEnergyFromFood(Cell &cell) noexcept {
  // Calculating coordinates by index and direction
  int deltaDirection  = getNextNthGen(cell, 1);
  int targetDirection = (cell._direction + deltaDirection) % kDirectionCount;
  int targetIndex     = calculateIndexByIndexAndDirection(cell._index, targetDirection);

  // If coordinates are beyond simulation world (above top or below bottom)
  if (targetIndex == -1) {
    return;
  }

  // If there is a live cell (prey) or organic
  if (_cellPtrVector[targetIndex] != nullptr) {
    // Getting cell at this direction
    std::unique_ptr<Cell> targetCellPtr = std::move(_cellPtrVector[targetIndex]);

    // If maximizing food energy is enabled (food energy is constant and equals to its maximum)
    if (_enableMaximizingFoodEnergy) {
      // Increasing energy level
      cell._energy += _maxBurstOfFoodEnergy;
    }
    // If maximizing food energy is disabled
    else {
      // Calculating energy from food
      int deltaEnergy = std::min(targetCellPtr->_energy, _maxBurstOfFoodEnergy);

      // Increasing energy level
      cell._energy += deltaEnergy;
    }

    // Making cell color more red
    ++cell._colorR;

    // Removing prey or organic
    removeCell(std::move(targetCellPtr));
  }
}

void CellController::bud(Cell &cell) noexcept {
  // Checking and updating energy
  if (cell._energy < _minChildEnergy * 2) {
    return;
  }

  // Checking each direction clockwise for ability to bud
  for (int i = 0; i < kDirectionCount; ++i) {
    // Calculating coordinates by index and direction
    int deltaDirection  = getNextNthGen(cell, 1);
    int targetDirection = (cell._direction + deltaDirection + i) % kDirectionCount;
    int targetIndex     = calculateIndexByIndexAndDirection(cell._index, targetDirection);

    // Making sure cycle body won't repeat if trying to bud in unoccupied space is not enabled
    if (!_enableTryingToBudInUnoccupiedDirection) {
      i = kDirectionCount;
    }

    // If coordinates are beyond simulation world (above top or below bottom)
    if (targetIndex == -1) {
      continue;
    }

    // If there is nothing at this direction
    if (_cellPtrVector[targetIndex] == nullptr) {
      // Creating new cell
      std::unique_ptr<Cell> buddedCellPtr =
          std::make_unique<Cell>(cell._genom, cell._energy / 2, cell._direction, targetIndex);

      // Assigning cell color
      float colorVectorLength = static_cast<float>(std::sqrt(
          cell._colorR * cell._colorR + cell._colorG * cell._colorG + cell._colorB * cell._colorB));
      buddedCellPtr->_colorR  = static_cast<int>(
          static_cast<float>(cell._colorR) * kBuddedCellParentColorMultiplier / colorVectorLength);
      buddedCellPtr->_colorG = static_cast<int>(
          static_cast<float>(cell._colorG) * kBuddedCellParentColorMultiplier / colorVectorLength);
      buddedCellPtr->_colorB = static_cast<int>(
          static_cast<float>(cell._colorB) * kBuddedCellParentColorMultiplier / colorVectorLength);

      // Applying random bud mutation to the budded cell
      if (static_cast<float>(_mersenneTwisterEngine()) /
              static_cast<float>(_mersenneTwisterEngine.max()) <
          _budMutationChance) {
        mutateRandomGen(*buddedCellPtr);
      }

      // Applying random bud mutation to current cell
      cell._energy -= cell._energy / 2;
      if (static_cast<float>(_mersenneTwisterEngine()) /
              static_cast<float>(_mersenneTwisterEngine.max()) <
          _budMutationChance) {
        mutateRandomGen(cell);
      }

      addCell(std::move(buddedCellPtr));

      return;
    }
  }

  // Turning cell into organic if it have to bud but not able to do it
  if (_enableDeathOnBuddingIfNotEnoughSpace) {
    cell._isAlive = false;
  }
}

void CellController::mutateRandomGen(Cell &cell) noexcept {
  // Changing random gen on another random one
  cell._genom[std::floor(static_cast<float>(_mersenneTwisterEngine()) /
                         static_cast<float>(_mersenneTwisterEngine.max()) *
                         static_cast<float>(_genomSize))] =
      std::floor(static_cast<float>(_mersenneTwisterEngine()) /
                 static_cast<float>(_mersenneTwisterEngine.max()) * static_cast<float>(_genomSize));
}

void CellController::shareEnergy(Cell &cell) const noexcept {
  // Calculating coordinates by index and direction
  int deltaDirection  = getNextNthGen(cell, 1);
  int targetDirection = (cell._direction + deltaDirection) % kDirectionCount;
  int targetIndex     = calculateIndexByIndexAndDirection(cell._index, targetDirection);

  // If coordinates are beyond simulation world (above top or below bottom)
  if (targetIndex == -1) {
    return;
  }

  // If there is a cell
  if (_cellPtrVector[targetIndex] != nullptr && _cellPtrVector[targetIndex]->_isAlive) {
    // Getting cell at this direction
    Cell &targetCell = *_cellPtrVector[targetIndex];

    // Calculating energy to share
    int deltaEnergy = static_cast<int>(static_cast<float>(cell._energy * getNextNthGen(cell, 2)) /
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

void CellController::touch(Cell &cell) const noexcept {
  // Calculating coordinates by index and direction
  int deltaDirection  = getNextNthGen(cell, 1);
  int targetDirection = (cell._direction + deltaDirection) % kDirectionCount;
  int targetIndex     = calculateIndexByIndexAndDirection(cell._index, targetDirection);

  // If coordinates are beyond simulation world (above top or below bottom)
  if (targetIndex == -1) {
    return;
  }

  // If there is a live cell (prey) or organic
  if (_cellPtrVector[targetIndex] != nullptr) {
    // Getting cell at this direction
    const Cell &targetCell = *_cellPtrVector[targetIndex];

    // If it is a live cell
    if (targetCell._isAlive) {
      // If it is an akin cell
      if (areAkin(cell, targetCell)) {
        jumpCounter(cell, getNextNthGen(cell, 4));
      }
      // If it is a strange cell
      else {
        jumpCounter(cell, getNextNthGen(cell, 5));
      }
    }
    // If it is food
    else {
      jumpCounter(cell, getNextNthGen(cell, 3));
    }
  }
  // If there is nothing at this direction
  else {
    jumpCounter(cell, getNextNthGen(cell, 2));
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
  // Greater or equal
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
  // Greater or equal
  else {
    jumpCounter(cell, getNextNthGen(cell, 3));
  }
}

void CellController::determineBurstOfPhotosynthesisEnergy(Cell &cell) const noexcept {
  // Calculating value to compare
  int valueToCompare = static_cast<int>(
      static_cast<float>(_maxBurstOfPhotosynthesisEnergy * getNextNthGen(cell, 1)) /
      static_cast<float>(_genomSize));

  // Calculating available photosynthesis energy
  int deltaEnergy = calculateBurstOfPhotosynthesisEnergy(cell._index);

  // Less
  if (deltaEnergy < valueToCompare) {
    jumpCounter(cell, getNextNthGen(cell, 2));
  }
  // Greater or equal
  else {
    jumpCounter(cell, getNextNthGen(cell, 3));
  }
}

void CellController::determineBurstOfMinerals(Cell &cell) const noexcept {
  // Calculating value to compare
  int valueToCompare =
      static_cast<int>(static_cast<float>(_maxBurstOfMinerals * getNextNthGen(cell, 1)) /
                       static_cast<float>(_genomSize));

  // Calculating available minerals
  int deltaMinerals = calculateBurstOfMinerals(cell._index);

  // Less
  if (deltaMinerals < valueToCompare) {
    jumpCounter(cell, getNextNthGen(cell, 2));
  }
  // Greater or equal
  else {
    jumpCounter(cell, getNextNthGen(cell, 3));
  }
}

void CellController::determineBurstOfMineralEnergy(Cell &cell) const noexcept {
  // Calculating value to compare
  int valueToCompare =
      static_cast<int>(_energyPerMineral * static_cast<float>(_maxMinerals) *
                       static_cast<float>(getNextNthGen(cell, 1)) / static_cast<float>(_genomSize));

  // Calculating available mineral energy
  int deltaEnergy = calculateBurstOfMineralEnergy(cell._minerals);

  // Less
  if (deltaEnergy < valueToCompare) {
    jumpCounter(cell, getNextNthGen(cell, 2));
  }
  // Greater or equal
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

int CellController::calculateBurstOfPhotosynthesisEnergy(int index) const noexcept {
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
  return static_cast<int>((static_cast<float>(_maxBurstOfPhotosynthesisEnergy) * depthCoefficient *
                           daytimeCoefficient));
}

int CellController::calculateBurstOfMinerals(int index) const noexcept {
  // Calculating cell row
  int row = calculateRowByIndex(index);

  // Getting minerals
  return static_cast<int>(
      mapClamp(row, _rows - 1, _rows - 1 - _maxMineralHeight, _maxBurstOfMinerals, 0.0f));
}

int CellController::calculateBurstOfMineralEnergy(int minerals) const noexcept {
  // Getting energy from minerals
  return static_cast<int>(static_cast<float>(minerals) * _energyPerMineral);
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

void CellController::addCell(std::unique_ptr<Cell> cellPtr) noexcept {
  // Pushing cell to the front of the linked list
  // so it will be processed not earlier than the next tick
  // and before older cells (younger cells have smaller "reaction time")

  // Order matters
  _cellIndexList.pushFront(cellPtr->_index);             // 1
  _cellPtrVector[cellPtr->_index] = std::move(cellPtr);  // 2
}

void CellController::removeCell(std::unique_ptr<Cell> cellPtr) noexcept {
  // Order matters
  _cellIndexList.remove(cellPtr->_index);     // 1
  _cellPtrVector[cellPtr->_index] = nullptr;  // 2
}
