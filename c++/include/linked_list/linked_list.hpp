// Copyright 2021 Malovanyi Denys Olehovych (maldenol)
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include <memory>

// Linked list implementation class
template <typename T>
class LinkedList {
 private:
  // Linked list element struct
  struct Element {
    std::shared_ptr<Element> next{};
    T                        value{};
  };

  std::shared_ptr<Element> _first{};

  int _elementCount{};

 public:
  // Linked list iterator implementation class
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
  LinkedList(const LinkedList &linkedList) noexcept;
  LinkedList &operator=(const LinkedList &linkedList) noexcept;
  LinkedList(LinkedList &&linkedList) noexcept;
  LinkedList &operator=(LinkedList &&linkedList) noexcept;
  ~LinkedList() noexcept;

  int pushFront(const T &value) noexcept;
  int replace(const T &value, const T &newValue) noexcept;
  int remove(const T &value) noexcept;

  size_t count() const noexcept;

  const Iterator getIterator() const noexcept;
  Iterator       getIterator() noexcept;
};

#include "./linked_list_iterator.inl"
#include "./linked_list.inl"

#endif
