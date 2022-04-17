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
#include "./linked_list.hpp"

// STD
#include <utility>
#include <vector>

template<typename T>
LinkedList<T>::LinkedList() noexcept {}

template<typename T>
LinkedList<T>::LinkedList(const LinkedList &linkedList) noexcept {
  operator=(linkedList);
}

template<typename T>
LinkedList<T> &LinkedList<T>::operator=(const LinkedList &linkedList) noexcept {
  // Values to push to front
  std::vector<T> valuesToCopy{};

  std::shared_ptr<Element> currElem{linkedList._first};

  // Until current is not null
  while (currElem != nullptr) {
    // Pushing value to vector
    valuesToCopy.push_back(currElem->value);

    // Keep looking
    currElem = currElem->next;
  }

  // For each value
  for (int i = valuesToCopy.size() - 1; i >= 0; --i) {
    // Pushing value to front
    pushFront(valuesToCopy[i]);
  }

  return *this;
}

template<typename T>
LinkedList<T>::LinkedList(LinkedList &&linkedList) noexcept : _first{std::exchange(linkedList._first, nullptr)} {}

template<typename T>
LinkedList<T> &LinkedList<T>::operator=(LinkedList &&linkedList) noexcept {
  std::swap(_first, linkedList._first);

  return *this;
}

template<typename T>
LinkedList<T>::~LinkedList() noexcept {
  std::shared_ptr<Element> currElem{_first};

  // Until current is not null
  while (currElem != nullptr) {
    // Removing current element
    currElem = currElem->next;
  }
}

template<typename T>
int LinkedList<T>::pushFront(const T &value) noexcept {
  // Considering the value is not in the list yet

  // Creating new element with given value
  Element *newElementPtr = new(std::nothrow) Element{_first, value};

  // If failed to allocate memory
  if (newElementPtr == nullptr) {
    // Value is not added
    return -1;
  }

  _first = std::shared_ptr<Element>(newElementPtr);
  // Value is added
  return 0;
}

template<typename T>
int LinkedList<T>::replace(const T &value, const T &newValue) noexcept {
  // Considering the value is unique

  std::shared_ptr<Element> currElem{_first};

  // Until current is not null
  while (currElem != nullptr) {
    // If value is found

    if (currElem->value == value) {
      // Replacing value of current element with the new one
      currElem->value = newValue;

      // Value is found
      return 0;
    }

    // Keep searching
    currElem = currElem->next;
  }

  // Value is not found
  return -1;
}

template<typename T>
int LinkedList<T>::remove(const T &value) noexcept {
  // Considering the value is unique

  std::shared_ptr<Element> prevElem{};
  std::shared_ptr<Element> currElem{_first};

  // Until current is not null
  while (currElem != nullptr) {
    // If value is found
    if (currElem->value == value) {
      // If current element is the first one
      if (prevElem == nullptr) {
        // Removing current element
        _first = currElem->next;
      }
      // If current element is not the first one
      else {
        // Removing current element
        prevElem->next = currElem->next;
      }

      // Value is found
      return 0;
    }

    // Keep searching
    prevElem = currElem;
    currElem = currElem->next;
  }

  // Value is not found
  return -1;
}

template<typename T>
size_t LinkedList<T>::count() const noexcept {
  size_t elementCount{};

  std::shared_ptr<Element> currElem{_first};

  // Until current is not null
  while (currElem != nullptr) {
    ++elementCount;

    // Keep looking
    currElem = currElem->next;
  }

  return elementCount;
}

template<typename T>
const typename LinkedList<T>::Iterator LinkedList<T>::getIterator() const noexcept { return Iterator{_first}; }

template<typename T>
typename LinkedList<T>::Iterator LinkedList<T>::getIterator() noexcept { return Iterator{_first}; }
