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
