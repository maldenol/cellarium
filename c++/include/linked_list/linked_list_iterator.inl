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
#include "./linked_list.hpp"

// STD
#include <utility>

template<typename T>
LinkedList<T>::Iterator::Iterator(std::shared_ptr<Element> next) noexcept : _next{next} {}

template<typename T>
LinkedList<T>::Iterator::Iterator(const Iterator &iterator) noexcept : _next{iterator._next} {}

template<typename T>
typename LinkedList<T>::Iterator &LinkedList<T>::Iterator::operator=(const Iterator &iterator) noexcept {
  _next = iterator._next;

  return *this;
}

template<typename T>
LinkedList<T>::Iterator::Iterator(Iterator &&iterator) noexcept : _next{std::exchange(iterator._next, nullptr)} {}

template<typename T>
typename LinkedList<T>::Iterator &LinkedList<T>::Iterator::operator=(Iterator &&iterator) noexcept {
  std::swap(_next, iterator._next);

  return *this;
}

template<typename T>
LinkedList<T>::Iterator::~Iterator() noexcept {}

template<typename T>
bool LinkedList<T>::Iterator::hasNext() const noexcept { return _next.lock() != nullptr; }

template<typename T>
const T &LinkedList<T>::Iterator::next() const noexcept {
  const T &value   = _next.lock()->value;
  _next = _next.lock()->next;
  return value;
}

template<typename T>
T &LinkedList<T>::Iterator::next() noexcept {
  T &value   = _next.lock()->value;
  _next = _next.lock()->next;
  return value;
}
