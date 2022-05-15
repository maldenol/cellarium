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

#ifndef CELL_CONTROLLER_HPP
#define CELL_CONTROLLER_HPP

// STD
#include <memory>
#include <random>
#include <vector>

// "LinkedList" internal library
#include <linked_list/linked_list.hpp>

// "cell" internal header
#include "./cell.hpp"

namespace CellEvolution {

// CellController::Params property initial values
static constexpr unsigned int kInitRandomSeed = 1234567890;

static constexpr int   kInitWidth = 800;
static constexpr int   kInitHeight = 600;
static constexpr float kInitCellSize = 8.0f;

static constexpr float kInitPhotosynthesisDepthMultiplier = 0.7f;
static constexpr float kInitMineralHeightMultiplier       = 0.7f;

static constexpr int kInitGenomSize = 64;
static constexpr int kInitMaxInstructionsPerTick = 16;
static constexpr int kInitMaxAkinGenomDifference = 4;
static constexpr int kInitMinChildEnergy = 40;
static constexpr int kInitMaxEnergy = 800;
static constexpr int kInitMaxPhotosynthesisEnergy = 30;
static constexpr float kInitSummerDaytimeToWholeDayRatio = 0.6f;
static constexpr bool  kInitEnableDaytimes = true;
static constexpr bool  kInitEnableSeasons = true;
static constexpr int   kInitMaxMineralEnergy = 15;
static constexpr int kInitMaxFoodEnergy = 50;
static constexpr float kInitRandomMutationChance = 0.01f;
static constexpr float kInitBudMutationChance = 0.25f;
static constexpr int   kInitDayDurationInTicks = 240;
static constexpr int   kInitSeasonDurationInDays = 92;
static constexpr int   kInitGammaFlashPeriodInDays = 46;
static constexpr int   kInitGammaFlashMaxMutationsCount = 8;
static constexpr bool  kInitEnableInstructionTurn = true;
static constexpr bool  kInitEnableInstructionMove = true;
static constexpr bool  kInitEnableInstructionGetEnergyFromPhotosynthesis = true;
static constexpr bool  kInitEnableInstructionGetEnergyFromMinerals = true;
static constexpr bool  kInitEnableInstructionGetEnergyFromFood = true;
static constexpr bool  kInitEnableInstructionBud = true;
static constexpr bool  kInitEnableInstructionMutateRandomGen = true;
static constexpr bool  kInitEnableInstructionShareEnergy = true;
static constexpr bool  kInitEnableInstructionTouch = true;
static constexpr bool  kInitEnableInstructionDetermineEnergyLevel = true;
static constexpr bool  kInitEnableInstructionDetermineDepth = true;
static constexpr bool  kInitenableInstructionDeterminePhotosynthesisEnergy = true;
static constexpr bool  kInitEnableInstructionDetermineMineralEnergy = true;
static constexpr bool  kInitEnableForcedBuddingOnMaximalEnergyLevel = true;
static constexpr bool  kInitEnableTryingToBudInUnoccupiedDirection = true;
static constexpr bool  kInitEnableDeathOnBuddingIfNotEnoughSpace = true;
static constexpr bool  kInitEnableDeadCellPinningOnSinking = true;
// Instruction enumeration
enum class CellInstructions {
  DoNothing,
  Turn,
  Move,
  GetEnergyFromPhotosynthesis,
  GetEnergyFromMinerals,
  GetEnergyFromFood,
  Bud,
  MutateRandomGen,
  ShareEnergy,
  Touch,
  DetermineEnergyLevel,
  DetermineDepth,
  DeterminePhotosynthesisEnergy,
  DetermineMineralEnergy,
  Size
};
// Cell rendering mode enumeration
enum class CellRenderingModes {
  Diet,
  EnergyLevel,
  EnergySharingBalance,
  LastEnergyShare,
  Size
};

// Class for stroring simulation parameters and state, computing it and filling data for rendering
class CellController {
 public:
  // Auxiliary struct for class construction
  struct Params {
    std::mt19937 mersenneTwisterEngine{kInitRandomSeed};
    unsigned int randomSeed{kInitRandomSeed};

    int   width{kInitWidth};
    int   height{kInitHeight};
    float cellSize{kInitCellSize};

    float maxPhotosynthesisDepthMultiplier{kInitPhotosynthesisDepthMultiplier};
    float maxMineralHeightMultiplier{kInitMineralHeightMultiplier};

    int   genomSize{kInitGenomSize};
    int   maxInstructionsPerTick{kInitMaxInstructionsPerTick};
    int   maxAkinGenomDifference{kInitMaxAkinGenomDifference};
    int   minChildEnergy{kInitMinChildEnergy};
    int   maxEnergy{kInitMaxEnergy};
    int   maxPhotosynthesisEnergy{kInitMaxPhotosynthesisEnergy};
    float summerDaytimeToWholeDayRatio{kInitSummerDaytimeToWholeDayRatio};
    bool  enableDaytimes{kInitEnableDaytimes};
    bool  enableSeasons{kInitEnableSeasons};
    int   maxMineralEnergy{kInitMaxMineralEnergy};
    int   maxFoodEnergy{kInitMaxFoodEnergy};
    float randomMutationChance{kInitRandomMutationChance};
    float budMutationChance{kInitBudMutationChance};
    int   dayDurationInTicks{kInitDayDurationInTicks};
    int   seasonDurationInDays{kInitSeasonDurationInDays};
    int   gammaFlashPeriodInDays{kInitGammaFlashPeriodInDays};
    int   gammaFlashMaxMutationsCount{kInitGammaFlashMaxMutationsCount};
    bool  enableInstructionTurn{kInitEnableInstructionTurn};
    bool  enableInstructionMove{kInitEnableInstructionMove};
    bool  enableInstructionGetEnergyFromPhotosynthesis{
        kInitEnableInstructionGetEnergyFromPhotosynthesis};
    bool enableInstructionGetEnergyFromMinerals{kInitEnableInstructionGetEnergyFromMinerals};
    bool enableInstructionGetEnergyFromFood{kInitEnableInstructionGetEnergyFromFood};
    bool enableInstructionBud{kInitEnableInstructionBud};
    bool enableInstructionMutateRandomGen{kInitEnableInstructionMutateRandomGen};
    bool enableInstructionShareEnergy{kInitEnableInstructionShareEnergy};
    bool enableInstructionTouch{kInitEnableInstructionTouch};
    bool enableInstructionDetermineEnergyLevel{kInitEnableInstructionDetermineEnergyLevel};
    bool enableInstructionDetermineDepth{kInitEnableInstructionDetermineDepth};
    bool enableInstructionDeterminePhotosynthesisEnergy{
        kInitenableInstructionDeterminePhotosynthesisEnergy};
    bool enableInstructionDetermineMineralEnergy{kInitEnableInstructionDetermineMineralEnergy};
    bool enableForcedBuddingOnMaximalEnergyLevel{kInitEnableForcedBuddingOnMaximalEnergyLevel};
    bool enableTryingToBudInUnoccupiedDirection{kInitEnableTryingToBudInUnoccupiedDirection};
    bool enableDeathOnBuddingIfNotEnoughSpace{kInitEnableDeathOnBuddingIfNotEnoughSpace};
    bool enableDeadCellPinningOnSinking{kInitEnableDeadCellPinningOnSinking};
  };

  // Struct for storing index and color for rendering cell
  struct CellRenderingData {
    int   index;
    float colorR;
    float colorG;
    float colorB;
  };

 private:
  // Pseufo-random numbers generation
  std::mt19937 _mersenneTwisterEngine{};
  unsigned int _randomSeed{};

  // Simulation discrete space size
  int _columns{};
  int _rows{};

  // Genom machine and simulation environment properties
  int   _genomSize{};
  int   _maxInstructionsPerTick{};
  int   _maxAkinGenomDifference{};
  int   _minChildEnergy{};
  int   _maxEnergy{};
  int   _maxPhotosynthesisEnergy{};
  int   _maxPhotosynthesisDepth{};
  float _summerDaytimeToWholeDayRatio{};
  bool  _enableDaytimes{};
  bool  _enableSeasons{};
  int   _maxMineralEnergy{};
  int   _maxMineralHeight{};
  int   _maxFoodEnergy{};
  float _randomMutationChance{};
  float _budMutationChance{};
  int   _dayDurationInTicks{};
  int   _seasonDurationInDays{};
  int   _gammaFlashPeriodInDays{};
  int   _gammaFlashMaxMutationsCount{};

  // Flags for enabling/disabling cell genom instructions and other parameters
  bool _enableInstructionTurn{};
  bool _enableInstructionMove{};
  bool _enableInstructionGetEnergyFromPhotosynthesis{};
  bool _enableInstructionGetEnergyFromMinerals{};
  bool _enableInstructionGetEnergyFromFood{};
  bool _enableInstructionBud{};
  bool _enableInstructionMutateRandomGen{};
  bool _enableInstructionShareEnergy{};
  bool _enableInstructionTouch{};
  bool _enableInstructionDetermineEnergyLevel{};
  bool _enableInstructionDetermineDepth{};
  bool _enableInstructionDeterminePhotosynthesisEnergy{};
  bool _enableInstructionDetermineMineralEnergy{};
  bool _enableForcedBuddingOnMaximalEnergyLevel{};
  bool _enableTryingToBudInUnoccupiedDirection{};
  bool _enableDeathOnBuddingIfNotEnoughSpace{};
  bool _enableDeadCellPinningOnSinking{};

  // Linked list of cell indices for quick consequent access
  LinkedList<int> _cellIndexList{};
  // Vector of cells for quick random access
  std::vector<std::unique_ptr<Cell>> _cellPtrVector{};

  // Simulation time counters
  int _ticksNumber{};
  int _yearsNumber{};

 public:
  // Constructors
  CellController();
  explicit CellController(const Params &params);
  CellController(const CellController &cellController) noexcept;
  CellController &operator=(const CellController &cellController) noexcept;
  CellController(CellController &&cellController) noexcept;
  CellController &operator=(CellController &&cellController) noexcept;
  ~CellController() noexcept;

  // Computes one simulation tick
  void act() noexcept;

  // Puts RenderingData of each cell to given array
  void render(CellRenderingData *cellRenderingData, int cellRenderingMode = 0);

  // Returns count of cells
  size_t getCellCount() const noexcept;

  // Returns sun offset in range from -1 to 1
  float getSunPosition() const noexcept;
  // Returns daytime width in range from 0 to 1
  float getDaytimeWidth() const noexcept;

  // Getters
  int getColumns() const noexcept;
  int getRows() const noexcept;
  int getMaxPhotosynthesisDepth() const noexcept;
  int getMaxMineralHeight() const noexcept;

 private:
  // Updates tick counters
  void updateTime() noexcept;

  // Makes every cell mutate
  void gammaFlash() noexcept;

  // Perform appropriate cell genom instructions
  void turn(Cell &cell) const noexcept;
  void move(Cell &cell) noexcept;
  void getEnergyFromPhotosynthesis(Cell &cell) const noexcept;
  void getEnergyFromMinerals(Cell &cell) const noexcept;
  void getEnergyFromFood(Cell &cell) noexcept;
  void bud(Cell &cell) noexcept;
  void mutateRandomGen(Cell &cell) noexcept;
  void shareEnergy(Cell &cell) const noexcept;
  void touch(Cell &cell) const noexcept;
  void determineEnergyLevel(Cell &cell) const noexcept;
  void determineDepth(Cell &cell) const noexcept;
  void determinePhotosynthesisEnergy(Cell &cell) const noexcept;
  void determineMineralEnergy(Cell &cell) const noexcept;
  void incrementGenomCounter(Cell &cell) const noexcept;

  // Perform cell genom calculations
  void addGenToCounter(Cell &cell) const noexcept;
  void jumpCounter(Cell &cell, int offset) const noexcept;
  int  getNextNthGen(const Cell &cell, int n) const noexcept;
  bool areAkin(const Cell &cell1, const Cell &cell2) const noexcept;

  // Calculate burst of energy
  int calculatePhotosynthesisEnergy(int index) const noexcept;
  int calculateMineralEnergy(int index) const noexcept;

  // Calculate indices, columns and rows
  int calculateColumnByIndex(int index) const noexcept;
  int calculateRowByIndex(int index) const noexcept;
  int calculateIndexByColumnAndRow(int column, int row) const noexcept;
  int calculateIndexByIndexAndDirection(int index, int direction) const noexcept;

  // Add and remove cells
  void addCell(std::unique_ptr<Cell> cellPtr) noexcept;
  void removeCell(std::unique_ptr<Cell> cellPtr) noexcept;
};

}

#endif