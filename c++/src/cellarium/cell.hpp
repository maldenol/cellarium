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

#ifndef CELL_HPP
#define CELL_HPP

// STD
#include <vector>

namespace cellarium {

// Class for storing cell properties
class Cell {
 private:
  std::vector<int> _genome{};
  int              _counter{};

  int _energy{};

  int _minerals{};

  int _direction{};
  int _index{};

  int _age{};

  int _colorR{};
  int _colorG{};
  int _colorB{};

  int   _energyShareBalance{};
  float _lastEnergyShare{};

  bool _isAlive{};
  bool _isPinned{};

 public:
  Cell() noexcept;
  Cell(const std::vector<int> &genome, int energy, int direction, int index);
  Cell(const Cell &cell) noexcept;
  Cell &operator=(const Cell &cell) noexcept;
  Cell(Cell &&cell) noexcept;
  Cell &operator=(Cell &&cell) noexcept;
  ~Cell() noexcept;

  bool operator==(const Cell &cell) const noexcept;
  bool operator!=(const Cell &cell) const noexcept;

  friend class CellController;

  // Friend function for viewing struct properties
  friend void processCellOverview(Cell &cell, bool selectedCellExists);
};

}

#endif
