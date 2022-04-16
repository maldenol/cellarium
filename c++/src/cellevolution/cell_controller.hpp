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

#ifndef CELL_CONTROLLER_HPP
#define CELL_CONTROLLER_HPP

// STD
#include <algorithm>
#include <memory>
#include <random>
#include <vector>

// "LinkedList" internal library
#include <linked_list/linked_list.hpp>

// "cell" internal header
#include "./cell.hpp"

namespace CellEvolution {

// CellController::Params property initial values
static constexpr unsigned int kInitRandomSeed{1234567890};

static constexpr int   kInitWidth{800};
static constexpr int   kInitHeight{600};
static constexpr float kInitCellSize{8.0f};

static constexpr int kInitColumns{static_cast<int>(static_cast<float>(kInitWidth) / kInitCellSize)};
static constexpr int kInitRows{static_cast<int>(static_cast<float>(kInitHeight) / kInitCellSize)};
static constexpr int kInitGenomSize{64};
static constexpr int kInitMaxInstructionsPerTick{16};
static constexpr int kInitMaxAkinGenomDifference{4};
static constexpr int kInitMinChildEnergy{40};
static constexpr int kInitMaxEnergy{800};
static constexpr int kInitMaxPhotosynthesisEnergy{30};
static constexpr int kInitMaxPhotosynthesisDepth{
    static_cast<int>(static_cast<float>(kInitRows) * 0.7f)};
static constexpr float kInitSummerDaytimeToWholeDayRatio{0.6f};
static constexpr bool  kInitEnableDaytimes{true};
static constexpr bool  kInitEnableSeasons{true};
static constexpr int   kInitMaxMineralEnergy{15};
static constexpr int kInitMaxMineralHeight{static_cast<int>(static_cast<float>(kInitRows) * 0.7f)};
static constexpr int kInitMaxFoodEnergy{50};
static constexpr float kInitRandomMutationChance{0.01f};
static constexpr float kInitBudMutationChance{0.25f};
static constexpr int   kInitDayDurationInTicks{240};
static constexpr int   kInitSeasonDurationInDays{92};
static constexpr int   kInitGammaFlashPeriodInDays{46};
static constexpr int   kInitGammaFlashMaxMutationsCount{8};
static constexpr bool  kInitEnableInstructionTurn{true};
static constexpr bool  kInitEnableInstructionMove{true};
static constexpr bool  kInitEnableInstructionGetEnergyFromPhotosynthesis{true};
static constexpr bool  kInitEnableInstructionGetEnergyFromMinerals{true};
static constexpr bool  kInitEnableInstructionGetEnergyFromFood{true};
static constexpr bool  kInitEnableInstructionBud{true};
static constexpr bool  kInitEnableInstructionMutateRandomGen{true};
static constexpr bool  kInitEnableInstructionShareEnergy{true};
static constexpr bool  kInitEnableInstructionLookForward{true};
static constexpr bool  kInitEnableInstructionDetermineEnergyLevel{true};
static constexpr bool  kInitEnableInstructionDetermineDepth{true};
static constexpr bool  kInitenableInstructionDeterminePhotosynthesisEnergy{true};
static constexpr bool  kInitEnableInstructionDetermineMineralEnergy{true};
static constexpr bool  kInitEnableDeadCellPinningOnSinking{true};
// Instruction constants
static constexpr int kInstructionDoNothing{0};
static constexpr int kInstructionTurn{1};
static constexpr int kInstructionMove{2};
static constexpr int kInstructionGetEnergyFromPhotosynthesis{3};
static constexpr int kInstructionGetEnergyFromMinerals{4};
static constexpr int kInstructionGetEnergyFromFood{5};
static constexpr int kInstructionBud{6};
static constexpr int kInstructionMutateRandomGen{7};
static constexpr int kInstructionShareEnergy{8};
static constexpr int kInstructionLookForward{9};
static constexpr int kInstructionDetermineEnergyLevel{10};
static constexpr int kInstructionDetermineDepth{11};
static constexpr int kInstructionDeterminePhotosynthesisEnergy{12};
static constexpr int kInstructionDetermineMineralEnergy{13};

// Class for stroring simulation parameters and state, computing it and filling data for rendering
class CellController {
 public:
  // Auxiliary structure for class construction
  struct Params {
    std::mt19937 mersenneTwisterEngine{kInitRandomSeed};
    unsigned int randomSeed{kInitRandomSeed};

    int   width{kInitWidth};
    int   height{kInitHeight};
    float cellSize{kInitCellSize};

    int   columns{kInitColumns};
    int   rows{kInitRows};
    int   genomSize{kInitGenomSize};
    int   maxInstructionsPerTick{kInitMaxInstructionsPerTick};
    int   maxAkinGenomDifference{kInitMaxAkinGenomDifference};
    int   minChildEnergy{kInitMinChildEnergy};
    int   maxEnergy{kInitMaxEnergy};
    int   maxPhotosynthesisEnergy{kInitMaxPhotosynthesisEnergy};
    int   maxPhotosynthesisDepth{kInitMaxPhotosynthesisDepth};
    float summerDaytimeToWholeDayRatio{kInitSummerDaytimeToWholeDayRatio};
    bool  enableDaytimes{kInitEnableDaytimes};
    bool  enableSeasons{kInitEnableSeasons};
    int   maxMineralEnergy{kInitMaxMineralEnergy};
    int   maxMineralHeight{kInitMaxMineralHeight};
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
    bool enableInstructionLookForward{kInitEnableInstructionLookForward};
    bool enableInstructionDetermineEnergyLevel{kInitEnableInstructionDetermineEnergyLevel};
    bool enableInstructionDetermineDepth{kInitEnableInstructionDetermineDepth};
    bool enableInstructionDeterminePhotosynthesisEnergy{
        kInitenableInstructionDeterminePhotosynthesisEnergy};
    bool enableInstructionDetermineMineralEnergy{kInitEnableInstructionDetermineMineralEnergy};
    bool enableDeadCellPinningOnSinking{kInitEnableDeadCellPinningOnSinking};
  };

  // Struct for storing position and color for rendering
  struct RenderingData {
    int   index;
    float colorR;
    float colorG;
    float colorB;
    float colorA;
  };

 private:
  // Pseufo-random numbers generation
  std::mt19937 _mersenneTwisterEngine{};
  unsigned int _randomSeed{};

  // Simulation world and cell sizes
  int   _width{};
  int   _height{};
  float _cellSize{};

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
  bool _enableInstructionLookForward{};
  bool _enableInstructionDetermineEnergyLevel{};
  bool _enableInstructionDetermineDepth{};
  bool _enableInstructionDeterminePhotosynthesisEnergy{};
  bool _enableInstructionDetermineMineralEnergy{};
  bool _enableDeadCellPinningOnSinking{};

  // Linked list of cell pointers for quick consequent access
  LinkedList<std::shared_ptr<Cell>> _cellPtrList{};
  // Vector of cells for quick random access
  std::vector<Cell> _cellVector{};

  // Simulation clock (counts ticks)
  int _ticksNumber{};
  int _yearsNumber{};

  // Photosynthesis and mineral energy surge vectors for optimization when rendering environment
  std::vector<int> _surgeOfPhotosynthesisEnergy{};
  std::vector<int> _surgeOfMineralEnergy{};
  bool             _enableRenderingEnvironment{};

  // Vector of cell rendering data and its size
  std::vector<RenderingData> _renderingDataVector{};
  int                        _renderingDataVectorSize{};

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
  void act(bool enableRenderingEnvironment) noexcept;

  // Updates rendering data vector
  void updateRenderingData(bool enableRenderingEnvironment, int cellRenderingMode);

  // Gets rendering data vector and its size
  const RenderingData *getRenderingData() const noexcept;
  int                  getRenderingDataSize() const noexcept;

 private:
  // Updates tick counters
  void updateTime() noexcept;

  // Updates energy surge vectors for optimization when rendering environment
  void updateEnergy() noexcept;

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
  void shareEnergy(Cell &cell) noexcept;
  void lookForward(Cell &cell) noexcept;
  void determineEnergyLevel(Cell &cell) const noexcept;
  void determineDepth(Cell &cell) const noexcept;
  void determinePhotosynthesisEnergy(Cell &cell) const noexcept;
  void determineMineralEnergy(Cell &cell) const noexcept;
  void incrementGenomCounter(Cell &cell) const noexcept;

  // Perform cell genom calculations
  void addGenToCounter(Cell &cell) const noexcept;
  void jumpCounter(Cell &cell, int offset) const noexcept;
  int  getNextNthGen(const Cell &cell, int n) const noexcept;
  void turnIntoFood(Cell &cell) const noexcept;
  bool areAkin(const Cell &cell1, const Cell &cell2) const noexcept;

  // Get surge of energy
  int getPhotosynthesisEnergy(int index) const noexcept;
  int getMineralEnergy(int index) const noexcept;
  int calculatePhotosynthesisEnergy(int index) const noexcept;
  int calculateMineralEnergy(int index) const noexcept;

  // Calculate indices, columns and rows
  int calculateColumnByIndex(int index) const noexcept;
  int calculateRowByIndex(int index) const noexcept;
  int calculateIndexByColumnAndRow(int column, int row) const noexcept;
  int calculateIndexByIndexAndDirection(int index, int direction) const noexcept;

  // Add and remove cells
  void addCell(const std::shared_ptr<Cell> &cellPtr) noexcept;
  void removeCell(const std::shared_ptr<Cell> &cellPtr) noexcept;

  // Pushes rendering data to its vector
  void pushRenderingData(const Cell &cell, int cellRenderingMode);
};

}

#endif
