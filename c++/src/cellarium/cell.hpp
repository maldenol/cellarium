// Copyright 2021 Malovanyi Denys Olehovych (maldenol)
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

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
