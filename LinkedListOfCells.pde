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

class LinkedListOfCells {
  private Element firstElement;

  public LinkedListOfCells() {
    this.firstElement = null;
  }

  public void addAsFirst(Cell cell) {
    this.firstElement = new Element(this.firstElement, cell);
  }

  public void remove(Cell cell) {
    Element previous = null, current = this.firstElement;

    while (current != null) {
      if (current.cell == cell) {
        if (previous != null) {
          previous.nextElement = current.nextElement;
        } else {
          this.firstElement = current.nextElement;
        }

        return;
      }

      previous = current;
      current = current.nextElement;
    }
  }

  public ListIterator listIterator() {
    return new ListIterator(this.firstElement);
  }


  private class Element {
    private Element nextElement;
    private Cell cell;

    public Element(Element nextElement, Cell cell) {
      this.nextElement = nextElement;
      this.cell = cell;
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

    public Cell next() {
      Cell valueToReturn = this.next.cell;
      this.next = this.next.nextElement;
      return valueToReturn;
    }
  }
}
