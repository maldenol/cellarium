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

// Initial values of CellController::Params fields
static constexpr unsigned int kInitRandomSeed = 0;

static constexpr int   kInitWidth    = 800;
static constexpr int   kInitHeight   = 600;
static constexpr float kInitCellSize = 8.0f;

static constexpr float kInitPhotosynthesisDepthMultiplier = 0.7f;
static constexpr float kInitMineralHeightMultiplier       = 0.7f;

static constexpr int   kInitGenomSize                      = 64;
static constexpr int   kInitMaxInstructionsPerTick         = 16;
static constexpr int   kInitMaxAkinGenomDifference         = 4;
static constexpr int   kInitMinChildEnergy                 = 40;
static constexpr int   kInitMaxEnergy                      = 400;
static constexpr int   kInitMaxBurstOfPhotosynthesisEnergy = 10;
static constexpr float kInitSummerDaytimeToWholeDayRatio   = 0.6f;
static constexpr int   kInitMaxMinerals                    = 100;
static constexpr int   kInitMaxBurstOfMinerals             = 10;
static constexpr float kInitEnergyPerMineral               = 0.2f;
static constexpr int   kInitMaxBurstOfFoodEnergy           = 20;
static constexpr float kInitRandomMutationChance           = 0.01f;
static constexpr float kInitBudMutationChance              = 0.25f;
static constexpr int   kInitDayDurationInTicks             = 240;
static constexpr int   kInitSeasonDurationInDays           = 30;
static constexpr int   kInitGammaFlashPeriodInDays         = 150;
static constexpr int   kInitGammaFlashMaxMutationsCount    = 8;

static constexpr bool  kInitEnableInstructionTurn                                 = true;
static constexpr bool  kInitEnableInstructionMove                                 = true;
static constexpr bool  kInitEnableInstructionGetEnergyFromPhotosynthesis          = true;
static constexpr bool  kInitEnableInstructionGetEnergyFromMinerals                = true;
static constexpr bool  kInitEnableInstructionGetEnergyFromFood                    = true;
static constexpr bool  kInitEnableInstructionBud                                  = true;
static constexpr bool  kInitEnableInstructionMutateRandomGene                     = true;
static constexpr bool  kInitEnableInstructionShareEnergy                          = true;
static constexpr bool  kInitEnableInstructionTouch                                = true;
static constexpr bool  kInitEnableInstructionDetermineEnergyLevel                 = true;
static constexpr bool  kInitEnableInstructionDetermineDepth                       = true;
static constexpr bool  kInitenableInstructionDetermineBurstOfPhotosynthesisEnergy = true;
static constexpr bool  kInitEnableInstructionDetermineBurstOfMinerals             = true;
static constexpr bool  kInitEnableInstructionDetermineBurstOfMineralEnergy        = true;

static constexpr bool  kInitEnableZeroEnergyOrganic                 = false;
static constexpr bool  kInitEnableForcedBuddingOnMaximalEnergyLevel = true;
static constexpr bool  kInitEnableTryingToBudInUnoccupiedDirection  = true;
static constexpr bool  kInitEnableDeathOnBuddingIfNotEnoughSpace    = true;
static constexpr bool  kInitEnableSeasons                           = true;
static constexpr bool  kInitEnableDaytimes                          = false;
static constexpr bool  kInitEnableMaximizingFoodEnergy              = true;
static constexpr bool  kInitEnableDeadCellPinningOnSinking          = true;

static const std::vector<int> kInitFirstCellGenome            = std::vector<int>(1, 3);
static constexpr float        kInitFirstCellEnergyMultiplier = 3.0f;
static constexpr int          kInitFirstCellDirection        = 2;
static constexpr float        kInitFirstCellIndexMultiplier  = 2.5f;

// Instruction enumeration
enum class CellInstructions {
  DoNothing,
  Turn,
  Move,
  GetEnergyFromPhotosynthesis,
  GetEnergyFromMinerals,
  GetEnergyFromFood,
  Bud,
  MutateRandomGene,
  ShareEnergy,
  Touch,
  DetermineEnergyLevel,
  DetermineDepth,
  DetermineBurstOfPhotosynthesisEnergy,
  DetermineBurstOfMinerals,
  DetermineBurstOfMineralEnergy,
  Size
};

// Cell rendering mode enumeration
enum class CellRenderingModes {
  Diet,
  EnergyLevel,
  EnergySharingBalance,
  LastEnergyShare,
  Age,
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

    int   genomeSize{kInitGenomSize};
    int   maxInstructionsPerTick{kInitMaxInstructionsPerTick};
    int   maxAkinGenomDifference{kInitMaxAkinGenomDifference};
    int   minChildEnergy{kInitMinChildEnergy};
    int   maxEnergy{kInitMaxEnergy};
    int   maxBurstOfPhotosynthesisEnergy{kInitMaxBurstOfPhotosynthesisEnergy};
    float summerDaytimeToWholeDayRatio{kInitSummerDaytimeToWholeDayRatio};
    int   maxMinerals{kInitMaxMinerals};
    int   maxBurstOfMinerals{kInitMaxBurstOfMinerals};
    float energyPerMineral{kInitEnergyPerMineral};
    int   maxBurstOfFoodEnergy{kInitMaxBurstOfFoodEnergy};
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
    bool enableInstructionMutateRandomGene{kInitEnableInstructionMutateRandomGene};
    bool enableInstructionShareEnergy{kInitEnableInstructionShareEnergy};
    bool enableInstructionTouch{kInitEnableInstructionTouch};
    bool enableInstructionDetermineEnergyLevel{kInitEnableInstructionDetermineEnergyLevel};
    bool enableInstructionDetermineDepth{kInitEnableInstructionDetermineDepth};
    bool enableInstructionDetermineBurstOfPhotosynthesisEnergy{
        kInitenableInstructionDetermineBurstOfPhotosynthesisEnergy};
    bool enableInstructionDetermineBurstOfMinerals{kInitEnableInstructionDetermineBurstOfMinerals};
    bool enableInstructionDetermineBurstOfMineralEnergy{kInitEnableInstructionDetermineBurstOfMineralEnergy};

    bool enableZeroEnergyOrganic{kInitEnableZeroEnergyOrganic};
    bool enableForcedBuddingOnMaximalEnergyLevel{kInitEnableForcedBuddingOnMaximalEnergyLevel};
    bool enableTryingToBudInUnoccupiedDirection{kInitEnableTryingToBudInUnoccupiedDirection};
    bool enableDeathOnBuddingIfNotEnoughSpace{kInitEnableDeathOnBuddingIfNotEnoughSpace};
    bool enableSeasons{kInitEnableSeasons};
    bool enableDaytimes{kInitEnableDaytimes};
    bool enableMaximizingFoodEnergy{kInitEnableMaximizingFoodEnergy};
    bool enableDeadCellPinningOnSinking{kInitEnableDeadCellPinningOnSinking};

    std::vector<int> firstCellGenome{kInitFirstCellGenome};
    float            firstCellEnergyMultiplier{kInitFirstCellEnergyMultiplier};
    int              firstCellDirection{kInitFirstCellDirection};
    float            firstCellIndexMultiplier{kInitFirstCellIndexMultiplier};
  };

  // Struct for storing index and color for rendering cell
  struct CellRenderingData {
    int   index{};

    float colorR{};
    float colorG{};
    float colorB{};
  };

  // Struct for storing simulation statistics
  struct Statistics {
    int tick{};
    int day{};
    int year{};
    int season{};

    int daysToGammaFlash{};

    int countOfLiveCells{};
    int countOfDeadCells{};

    long countOfBuds{};

    long countOfPhotosynthesisEnergyBursts{};
    long countOfMineralEnergyBursts{};
    long countOfFoodEnergyBursts{};
  };

 private:
  // Pseufo-random numbers generation
  std::mt19937 _mersenneTwisterEngine{};
  unsigned int _randomSeed{};

  // Simulation discrete space size
  int _columns{};
  int _rows{};

  // Genome machine and simulation environment properties
  int   _genomeSize{};
  int   _maxInstructionsPerTick{};
  int   _maxAkinGenomDifference{};
  int   _minChildEnergy{};
  int   _maxEnergy{};
  int   _maxBurstOfPhotosynthesisEnergy{};
  int   _maxPhotosynthesisDepth{};
  float _summerDaytimeToWholeDayRatio{};
  int   _maxMinerals{};
  int   _maxBurstOfMinerals{};
  float _energyPerMineral{};
  int   _maxMineralHeight{};
  int   _maxBurstOfFoodEnergy{};
  float _randomMutationChance{};
  float _budMutationChance{};
  int   _dayDurationInTicks{};
  int   _seasonDurationInDays{};
  int   _gammaFlashPeriodInDays{};
  int   _gammaFlashMaxMutationsCount{};

  // Cell genome instruction enabling flags
  bool _enableInstructionTurn{};
  bool _enableInstructionMove{};
  bool _enableInstructionGetEnergyFromPhotosynthesis{};
  bool _enableInstructionGetEnergyFromMinerals{};
  bool _enableInstructionGetEnergyFromFood{};
  bool _enableInstructionBud{};
  bool _enableInstructionMutateRandomGene{};
  bool _enableInstructionShareEnergy{};
  bool _enableInstructionTouch{};
  bool _enableInstructionDetermineEnergyLevel{};
  bool _enableInstructionDetermineDepth{};
  bool _enableInstructionDetermineBurstOfPhotosynthesisEnergy{};
  bool _enableInstructionDetermineBurstOfMinerals{};
  bool _enableInstructionDetermineBurstOfMineralEnergy{};

  // Other simulation rule enabling flags
  bool _enableZeroEnergyOrganic{};
  bool _enableForcedBuddingOnMaximalEnergyLevel{};
  bool _enableTryingToBudInUnoccupiedDirection{};
  bool _enableDeathOnBuddingIfNotEnoughSpace{};
  bool _enableSeasons{};
  bool _enableDaytimes{};
  bool _enableMaximizingFoodEnergy{};
  bool _enableDeadCellPinningOnSinking{};

  // Linked list of cell indices for quick consequent access
  LinkedList<int> _cellIndexList{};
  // Vector of cells for quick random access
  std::vector<std::unique_ptr<Cell>> _cellPtrVector{};

  // Simulation time counters
  int _ticksNumber{};
  int _yearsNumber{};

  // Simulation statistics counters
  int  _countOfLiveCells{};
  int  _countOfDeadCells{};
  long _countOfBuds{};
  long _countOfPhotosynthesisEnergyBursts{};
  long _countOfMineralEnergyBursts{};
  long _countOfFoodEnergyBursts{};

  // Selected cell pointer
  const Cell *_selectedCellPtr{};

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

  // Returns simulation statistics
  Statistics getSimulationStatistics() const noexcept;

  // Getters
  int getColumns() const noexcept;
  int getRows() const noexcept;
  int getMaxPhotosynthesisDepth() const noexcept;
  int getMaxMineralHeight() const noexcept;

  // Select cell and return selected cell
  bool selectCell(int column, int row) noexcept;
  const Cell *getSelectedCell() const noexcept;

  // Friend function for viewing and changing class properties
  friend void processSimulationParameters(CellController &cellController);

 private:
  // Updates tick counters
  void updateTime() noexcept;

  // Makes every cell mutate
  void gammaFlash() noexcept;

  // Perform appropriate cell genome instructions
  void turn(Cell &cell) const noexcept;
  void move(Cell &cell) noexcept;
  void getEnergyFromPhotosynthesis(Cell &cell) noexcept;
  void getEnergyFromMinerals(Cell &cell) noexcept;
  void getEnergyFromFood(Cell &cell) noexcept;
  void bud(Cell &cell) noexcept;
  void mutateRandomGene(Cell &cell) noexcept;
  void shareEnergy(Cell &cell) const noexcept;
  void touch(Cell &cell) const noexcept;
  void determineEnergyLevel(Cell &cell) const noexcept;
  void determineDepth(Cell &cell) const noexcept;
  void determineBurstOfPhotosynthesisEnergy(Cell &cell) const noexcept;
  void determineBurstOfMinerals(Cell &cell) const noexcept;
  void determineBurstOfMineralEnergy(Cell &cell) const noexcept;
  void incrementGenomCounter(Cell &cell) const noexcept;

  // Perform cell genome calculations
  void addGenToCounter(Cell &cell) const noexcept;
  void jumpCounter(Cell &cell, int offset) const noexcept;
  int  getNextNthGen(const Cell &cell, int n) const noexcept;
  bool areAkin(const Cell &cell1, const Cell &cell2) const noexcept;

  // Calculate bursts of energy and minerals
  int calculateBurstOfPhotosynthesisEnergy(int index) const noexcept;
  int calculateBurstOfMinerals(int index) const noexcept;
  int calculateBurstOfMineralEnergy(int minerals) const noexcept;

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
