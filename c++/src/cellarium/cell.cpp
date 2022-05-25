// Copyright 2021 Malovanyi Denys Olehovych (maldenol)
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

// Header file
#include "./cell.hpp"

// STD
#include <utility>

// Using header file namespace
using namespace cellarium;

// Global constants
constexpr int kInitIndex = -1;

Cell::Cell() noexcept : _index{kInitIndex} {};

Cell::Cell(const std::vector<int> &genome, int energy, int direction, int index)
    : _genome{genome}, _energy{energy}, _direction{direction}, _index{index}, _isAlive{true} {}

Cell::Cell(const Cell &cell) noexcept
    : _genome{cell._genome},
      _counter{cell._counter},
      _energy{cell._energy},
      _minerals{cell._minerals},
      _direction{cell._direction},
      _index{cell._index},
      _age{cell._age},
      _colorR{cell._colorR},
      _colorG{cell._colorG},
      _colorB{cell._colorB},
      _energyShareBalance{cell._energyShareBalance},
      _lastEnergyShare{cell._lastEnergyShare},
      _isAlive{cell._isAlive},
      _isPinned{cell._isPinned} {}

Cell &Cell::operator=(const Cell &cell) noexcept {
  _genome             = cell._genome;
  _counter            = cell._counter;
  _energy             = cell._energy;
  _minerals           = cell._minerals;
  _direction          = cell._direction;
  _index              = cell._index;
  _age                = cell._age;
  _colorR             = cell._colorR;
  _colorG             = cell._colorG;
  _colorB             = cell._colorB;
  _energyShareBalance = cell._energyShareBalance;
  _lastEnergyShare    = cell._lastEnergyShare;
  _isAlive            = cell._isAlive;
  _isPinned           = cell._isPinned;

  return *this;
}

Cell::Cell(Cell &&cell) noexcept
    : _genome{std::exchange(cell._genome, std::vector<int>{})},
      _counter{std::exchange(cell._counter, 0)},
      _energy{std::exchange(cell._energy, 0)},
      _minerals{std::exchange(cell._minerals, 0)},
      _direction{std::exchange(cell._direction, 0)},
      _index{std::exchange(cell._index, kInitIndex)},
      _age{std::exchange(cell._age, 0)},
      _colorR{std::exchange(cell._colorR, 0)},
      _colorG{std::exchange(cell._colorG, 0)},
      _colorB{std::exchange(cell._colorB, 0)},
      _energyShareBalance{std::exchange(cell._energyShareBalance, 0)},
      _lastEnergyShare{std::exchange(cell._lastEnergyShare, 0.0f)},
      _isAlive{std::exchange(cell._isAlive, false)},
      _isPinned{std::exchange(cell._isPinned, false)} {}

Cell &Cell::operator=(Cell &&cell) noexcept {
  std::swap(_genome, cell._genome);
  std::swap(_counter, cell._counter);
  std::swap(_energy, cell._energy);
  std::swap(_minerals, cell._minerals);
  std::swap(_direction, cell._direction);
  std::swap(_index, cell._index);
  std::swap(_age, cell._age);
  std::swap(_colorR, cell._colorR);
  std::swap(_colorG, cell._colorG);
  std::swap(_colorB, cell._colorB);
  std::swap(_energyShareBalance, cell._energyShareBalance);
  std::swap(_lastEnergyShare, cell._lastEnergyShare);
  std::swap(_isAlive, cell._isAlive);
  std::swap(_isPinned, cell._isPinned);

  return *this;
}

Cell::~Cell() noexcept {};

bool Cell::operator==(const Cell &cell) const noexcept { return _index == cell._index; }

bool Cell::operator!=(const Cell &cell) const noexcept { return _index != cell._index; }
