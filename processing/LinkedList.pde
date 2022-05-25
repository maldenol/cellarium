// Copyright 2021 Malovanyi Denys Olehovych (maldenol)
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

class LinkedList<T> {
  private Element first;

  public LinkedList() {
    this.first = null;
  }

  public void pushFront(T value) {
    this.first = new Element(this.first, value);
  }

  public void remove(T value) {
    Element previous = null;
    Element current  = this.first;

    // Until current is not null
    while (current != null) {
      // If value is found
      if (current.value == value) {
        // If current element is not the first one
        if (previous != null) {
          // Removing current element
          previous.next = current.next;
        }
        // If current element is the first one
        else {
          // Removing current element
          this.first = current.next;
        }

        return;
      }

      // Keep searching
      previous = current;
      current  = current.next;
    }

    // Value is not found
  }

  public ListIterator listIterator() {
    return new ListIterator(this.first);
  }

  private class Element {
    private Element next;
    private T value;

    public Element(Element next, T value) {
      this.next  = next;
      this.value = value;
    }
  }

  public class ListIterator {
    private Element next;

    public ListIterator(Element next) {
      this.next = next;
    }

    public boolean hasNext() {
      return (this.next != null);
    }

    public T next() {
      T value   = this.next.value;

      this.next = this.next.next;

      return value;
    }
  }
}
