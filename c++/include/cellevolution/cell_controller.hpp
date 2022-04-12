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

#include <memory>
#include <vector>

#include <linked_list/linked_list.hpp>

#include "./cell.hpp"

class CellController {
 private:
  // Random seed
  unsigned int _seed{};

  // Space scale
  short _width{};
  short _height{};
  short _scale{};

  // Space dimensions
  short _rows{};
  short _columns{};

  // Genom machine and environment properties
  short _genomSize{};
  short _maxInstructionsPerTick{};
  short _maxAkinGenomDifference{};
  short _minChildEnergy{};
  short _maxEnergy{};
  short _maxPhotosynthesisEnergy{};
  short _maxPhotosynthesisDepth{};
  float _summerDaytimeToWholeDayRatio{};
  bool  _enableDaytimes{};
  bool  _enableSeasons{};
  short _maxMineralEnergy{};
  short _maxMineralHeight{};
  short _maxFoodEnergy{};
  float _randomMutationChance{};
  float _budMutationChance{};
  short _dayDurationInTicks{};
  short _seasonDurationInDays{};
  short _gammaFlashPeriodInDays{};
  short _gammaFlashMaxMutationsCount{};

  // Flags for enabling/disabling cell genom instructions and other parameters
  bool _enableTurnInstruction{};
  bool _enableMoveInstruction{};
  bool _enablePhotosynthesisEnergyInstruction{};
  bool _enableMineralEnergyInstruction{};
  bool _enableFoodEnergyInstruction{};
  bool _enableBudInstruction{};
  bool _enableMutateInstruction{};
  bool _enableShareEnergyInstruction{};
  bool _enableLookInstruction{};
  bool _enableDetermineEnergyLevelInstruction{};
  bool _enableDetermineDepthInstruction{};
  bool _enableDeterminePhotosynthesisEnergyInstruction{};
  bool _enableDetermineMineralEnergyInstruction{};
  bool _enableDeadCellPinningOnSinking{};

  // Linked list of cells for quick consequent access
  LinkedList<std::shared_ptr<Cell>> _cellList{};
  // Buffer of cells for quick access by index
  std::vector<Cell> _cellBuffer{};

  // World time in ticks
  int _ticksNumber{};
  int _yearsNumber{};

  // Photosynthesis and mineral energy buffers for optimization
  std::vector<short> _surgeOfPhotosynthesisEnergy{};
  std::vector<short> _surgeOfMineralEnergy{};
  bool               _drawBackground{};

 public:
  // Auxiliary structure for class construction
  struct Params {
    unsigned int seed{1234567890};
    short        width{800};
    short        height{600};
    short        scale{8};
    short        rows{static_cast<short>(height / scale)};
    short        columns{static_cast<short>(width / scale)};
    short        genomSize{64};
    short        maxInstructionsPerTick{16};
    short        maxAkinGenomDifference{4};
    short        minChildEnergy{40};
    short        maxEnergy{800};
    short        maxPhotosynthesisEnergy{30};
    short maxPhotosynthesisDepth{static_cast<short>(height * 0.7f / scale)};
    float summerDaytimeToWholeDayRatio{0.6f};
    bool  enableDaytimes{true};
    bool  enableSeasons{true};
    short maxMineralEnergy{15};
    short maxMineralHeight{static_cast<short>(height * 0.7f / scale)};
    short maxFoodEnergy{50};
    float randomMutationChance{0.01f};
    float budMutationChance{0.25f};
    short dayDurationInTicks{240};
    short seasonDurationInDays{92};
    short gammaFlashPeriodInDays{46};
    short gammaFlashMaxMutationsCount{8};
    bool  enableTurnInstruction{true};
    bool  enableMoveInstruction{true};
    bool  enablePhotosynthesisEnergyInstruction{true};
    bool  enableMineralEnergyInstruction{true};
    bool  enableFoodEnergyInstruction{true};
    bool  enableBudInstruction{true};
    bool  enableMutateInstruction{true};
    bool  enableShareEnergyInstruction{true};
    bool  enableLookInstruction{true};
    bool  enableDetermineEnergyLevelInstruction{true};
    bool  enableDetermineDepthInstruction{true};
    bool  enableDeterminePhotosynthesisEnergyInstruction{true};
    bool  enableDetermineMineralEnergyInstruction{true};
    bool  enableDeadCellPinningOnSinking{true};
  };

  // Constructors
  CellController();
  explicit CellController(const Params& params);
  CellController(const CellController& cellController) = delete;
  CellController& operator=(const CellController& cellController) = delete;
  CellController(CellController&& cellController)                 = delete;
  CellController& operator=(CellController&& cellController) = delete;
  ~CellController();

  // Computes one simulation tick
  void act(bool drawBackground) noexcept;

 private:
  // Updates tick counters
  void updateTime() noexcept;

  // Updates energy surge buffers for optimization when drawing
  void updateEnergy() noexcept;

  // Makes every cell mutate
  void gammaFlash() noexcept;

  // Perform appropriate cell genom instructions
  void turn(Cell& cell) const noexcept;
  void move(Cell& cell) noexcept;
  void getEnergyFromPhotosynthesis(Cell& cell) noexcept;
  void getEnergyFromMinerals(Cell& cell) noexcept;
  void getEnergyFromFood(Cell& cell) noexcept;
  void bud(Cell& cell) noexcept;
  void mutateRandomGen(Cell& cell) const noexcept;
  void shareEnergy(Cell& cell) noexcept;
  void lookForward(Cell& cell) noexcept;
  void determineEnergyLevel(Cell& cell) const noexcept;
  void determineDepth(Cell& cell) const noexcept;
  void determinePhotosynthesisEnergy(Cell& cell) noexcept;
  void determineMineralEnergy(Cell& cell) noexcept;
  void incrementGenomCounter(Cell& cell) const noexcept;

  // Perform cell genom calculations
  void  addGenToCounter(Cell& cell) const noexcept;
  void  jumpCounter(Cell& cell, short offset) const noexcept;
  short getNextNthGen(Cell& cell, short n) const noexcept;
  void  turnIntoFood(Cell& cell) const noexcept;
  bool  areAkin(Cell& cell1, Cell& cell2) const noexcept;

  // Get surge of energy
  short getPhotosynthesisEnergy(int index) const noexcept;
  short getMineralEnergy(int index) const noexcept;
  short calculatePhotosynthesisEnergy(int index) const noexcept;
  short calculateMineralEnergy(int index) const noexcept;

  // Calculate indices, columns and rows
  short calculateColumn(int index) const noexcept;
  short calculateRow(int index) const noexcept;
  int   calculateIndexByDirection(int index, short direction) const noexcept;

  // Add and remove cells
  void addCell(const std::shared_ptr<Cell>& cellPtr) noexcept;
  void removeCell(const std::shared_ptr<Cell>& cellPtr) noexcept;
};

#endif
