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
#include "./cell.hpp"

// STD
#include <utility>

// Using header file namespace
using namespace CellEvolution;

Cell::Cell() noexcept : _index{-1} {};

Cell::Cell(const std::vector<int> &genom, int energy, int direction, int index)
    : _genom{genom}, _energy{energy}, _direction{direction}, _index{index}, _isAlive{true} {}

Cell::Cell(const Cell &cell) noexcept
    : _genom{cell._genom},
      _counter{cell._counter},
      _energy{cell._energy},
      _direction{cell._direction},
      _index{cell._index},
      _colorR{cell._colorR},
      _colorG{cell._colorG},
      _colorB{cell._colorB},
      _energyShareBalance{cell._energyShareBalance},
      _lastEnergyShare{cell._lastEnergyShare},
      _isAlive{cell._isAlive},
      _pinned{cell._pinned} {}

Cell &Cell::operator=(const Cell &cell) noexcept {
  _genom              = cell._genom;
  _counter            = cell._counter;
  _energy             = cell._energy;
  _direction          = cell._direction;
  _index              = cell._index;
  _colorR             = cell._colorR;
  _colorG             = cell._colorG;
  _colorB             = cell._colorB;
  _energyShareBalance = cell._energyShareBalance;
  _lastEnergyShare    = cell._lastEnergyShare;
  _isAlive            = cell._isAlive;
  _pinned             = cell._pinned;

  return *this;
}

Cell::Cell(Cell &&cell) noexcept
    : _genom{std::exchange(cell._genom, std::vector<int>{})},
      _counter{std::exchange(cell._counter, 0)},
      _energy{std::exchange(cell._energy, 0)},
      _direction{std::exchange(cell._direction, 0)},
      _index{std::exchange(cell._index, 0)},
      _colorR{std::exchange(cell._colorR, 0)},
      _colorG{std::exchange(cell._colorG, 0)},
      _colorB{std::exchange(cell._colorB, 0)},
      _energyShareBalance{std::exchange(cell._energyShareBalance, 0)},
      _lastEnergyShare{std::exchange(cell._lastEnergyShare, 0.0f)},
      _isAlive{std::exchange(cell._isAlive, false)},
      _pinned{std::exchange(cell._pinned, false)} {}

Cell &Cell::operator=(Cell &&cell) noexcept {
  std::swap(_genom, cell._genom);
  std::swap(_counter, cell._counter);
  std::swap(_energy, cell._energy);
  std::swap(_direction, cell._direction);
  std::swap(_index, cell._index);
  std::swap(_colorR, cell._colorR);
  std::swap(_colorG, cell._colorG);
  std::swap(_colorB, cell._colorB);
  std::swap(_energyShareBalance, cell._energyShareBalance);
  std::swap(_lastEnergyShare, cell._lastEnergyShare);
  std::swap(_isAlive, cell._isAlive);
  std::swap(_pinned, cell._pinned);

  return *this;
}

Cell::~Cell() noexcept {};

bool Cell::operator==(const Cell &cell) const noexcept { return _index == cell._index; }

bool Cell::operator!=(const Cell &cell) const noexcept { return _index != cell._index; }
