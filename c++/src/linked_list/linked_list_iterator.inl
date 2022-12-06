// Copyright 2021 Malovanyi Denys Olehovych (maldenol)
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

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
