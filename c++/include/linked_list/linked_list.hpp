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

#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include <memory>

template <typename T>
class LinkedList {
 private:
  struct Element {
    std::shared_ptr<Element> next{};
    T                        value{};
  };

  std::shared_ptr<Element> _first{};

 public:
  class Iterator {
   private:
    std::weak_ptr<Element> _next{};

   public:
    explicit Iterator(std::shared_ptr<Element> next) noexcept;
    Iterator(const Iterator &iterator) noexcept;
    Iterator &operator=(const Iterator &iterator) noexcept;
    Iterator(Iterator &&iterator) noexcept;
    Iterator &operator=(Iterator &&iterator) noexcept;
    ~Iterator() noexcept;

    bool     hasNext() const noexcept;
    const T &next() const noexcept;
    T       &next() noexcept;
  };

  LinkedList() noexcept;
  LinkedList(const LinkedList &linkedList) = delete;
  LinkedList &operator=(const LinkedList &linkedList) = delete;
  LinkedList(LinkedList &&linkedList) noexcept;
  LinkedList &operator=(LinkedList &&linkedList) noexcept;
  ~LinkedList() noexcept;

  void pushFront(const T &value) noexcept;
  void remove(const T &value) noexcept;

  const Iterator getIterator() const noexcept;
  Iterator       getIterator() noexcept;
};

#include "./linked_list_iterator.inl"
#include "./linked_list.inl"

#endif
