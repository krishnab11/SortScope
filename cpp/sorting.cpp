#include "sorting.h"
#include <string>
#include <utility>
#include "recorder.h"

using namespace std;

// The "line" numbers below are 0-based positions in the C++ listings shown in
// the browser (static/script.js, ALGOS table). Keep the two in sync.

// ======================= Bubble Sort =======================
void bubbleSort(vector<int> a, Queue& q) {
    Recorder r(q);
    int n = a.size();
    r.add(a, -1, -1, "start", -1, "Bubble Sort compares neighbours and swaps them when they are out of order.");

    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        r.add(a, -1, -1, "focus", 2,
              "Pass " + to_string(i + 1) + ": compare neighbours from index 0 up to index " +
                  to_string(n - i - 1) + ".");
        for (int j = 0; j < n - i - 1; j++) {
            r.comps++;
            r.add(a, j, j + 1, "compare", 4,
                  "Compare " + to_string(a[j]) + " and " + to_string(a[j + 1]) + ".");
            if (a[j] > a[j + 1]) {
                swap(a[j], a[j + 1]);
                r.moves++;
                swapped = true;
                r.add(a, j, j + 1, "swap", 5,
                      "Swap: " + to_string(a[j + 1]) + " is bigger than " + to_string(a[j]) +
                          ", so they trade places.");
            }
        }
        r.mark(n - 1 - i);  // the biggest value of this pass is now in place
        if (!swapped) break;
    }
    r.markAll(n);
    r.add(a, -1, -1, "done", -1, "Sorted!");
}

// ======================= Selection Sort =======================
void selectionSort(vector<int> a, Queue& q) {
    Recorder r(q);
    int n = a.size();
    r.add(a, -1, -1, "start", -1, "Selection Sort finds the smallest value in the unsorted part and puts it first.");

    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        r.pivot = minIdx;
        r.add(a, i, -1, "focus", 3,
              "Pass " + to_string(i + 1) + ": assume index " + to_string(i) + " holds the smallest value.");
        for (int j = i + 1; j < n; j++) {
            r.comps++;
            r.add(a, j, -1, "compare", 5,
                  "Compare " + to_string(a[j]) + " with the smallest so far (" + to_string(a[minIdx]) + ").");
            if (a[j] < a[minIdx]) {
                minIdx = j;
                r.pivot = minIdx;
            }
        }
        if (minIdx != i) {
            swap(a[i], a[minIdx]);
            r.moves++;
            r.pivot = -1;
            r.add(a, i, minIdx, "swap", 6,
                  "Swap the smallest value (" + to_string(a[i]) + ") into position " + to_string(i) + ".");
        }
        r.pivot = -1;
        r.mark(i);
    }
    r.markAll(n);
    r.add(a, -1, -1, "done", -1, "Sorted!");
}

// ======================= Insertion Sort =======================
// The picked "key" is drawn at its current slot in each snapshot, so you can
// watch it slide left while bigger values shift right.
static vector<int> withKey(vector<int> a, int slot, int key) {
    a[slot] = key;
    return a;
}

void insertionSort(vector<int> a, Queue& q) {
    Recorder r(q);
    int n = a.size();
    r.mark(0);
    r.add(a, -1, -1, "start", -1, "Insertion Sort takes each value and slides it left into its place.");

    for (int i = 1; i < n; i++) {
        int key = a[i];
        int j = i - 1;
        r.add(a, i, -1, "focus", 3, "Pick the key " + to_string(key) + " (index " + to_string(i) + ").");
        while (j >= 0) {
            r.comps++;
            r.add(withKey(a, j + 1, key), j, j + 1, "compare", 4,
                  "Compare " + to_string(a[j]) + " with the key " + to_string(key) + ".");
            if (a[j] > key) {
                a[j + 1] = a[j];  // shift right
                r.moves++;
                j--;
                r.add(withKey(a, j + 1, key), j + 1, j + 2, "shift", 5,
                      "Shift " + to_string(a[j + 2]) + " one place right to make room for " +
                          to_string(key) + ".");
            } else {
                break;
            }
        }
        a[j + 1] = key;
        r.mark(i);
        r.add(a, j + 1, -1, "insert", 8,
              "Insert the key " + to_string(key) + " at index " + to_string(j + 1) + ".");
    }
    r.markAll(n);
    r.add(a, -1, -1, "done", -1, "Sorted!");
}

// ======================= Quick Sort (recursive) =======================
// Lomuto partition, pivot = last element of the current range.
static void quickRec(vector<int>& a, int lo, int hi, Recorder& r) {
    if (lo > hi) return;
    if (lo == hi) {  // base case: one element is already in place
        r.mark(lo);
        return;
    }

    int pivot = a[hi];
    int i = lo - 1;
    r.lo = lo;
    r.hi = hi;
    r.pivot = hi;
    r.add(a, -1, -1, "focus", 1,
          "Sort indices " + to_string(lo) + " to " + to_string(hi) + ". The pivot is " + to_string(pivot) + ".");

    for (int j = lo; j < hi; j++) {
        r.comps++;
        r.add(a, j, -1, "compare", 3,
              "Compare " + to_string(a[j]) + " with the pivot (" + to_string(pivot) + ").");
        if (a[j] < pivot) {
            i++;
            if (i != j) {
                swap(a[i], a[j]);
                r.moves++;
                r.add(a, i, j, "swap", 5,
                      to_string(a[i]) + " is smaller than the pivot, so move it to the left side.");
            }
        }
    }

    int p = i + 1;  // final position of the pivot
    if (p != hi) {
        swap(a[p], a[hi]);
        r.moves++;
        r.add(a, p, hi, "swap", 8,
              "Place the pivot (" + to_string(pivot) + ") in its final position " + to_string(p) + ".");
    } else {
        r.add(a, p, -1, "focus", 8,
              "The pivot (" + to_string(pivot) + ") is already in its final position " + to_string(p) + ".");
    }
    r.mark(p);
    r.pivot = -1;

    if (lo < p - 1) {
        r.lo = lo;
        r.hi = p - 1;
        r.add(a, -1, -1, "call", 15,
              "Recursive call: sort the left part (indices " + to_string(lo) + " to " + to_string(p - 1) + ").");
    }
    quickRec(a, lo, p - 1, r);

    if (p + 1 < hi) {
        r.lo = p + 1;
        r.hi = hi;
        r.add(a, -1, -1, "call", 16,
              "Recursive call: sort the right part (indices " + to_string(p + 1) + " to " + to_string(hi) + ").");
    }
    quickRec(a, p + 1, hi, r);
}

void quickSort(vector<int> a, Queue& q) {
    Recorder r(q);
    int n = a.size();
    r.add(a, -1, -1, "start", -1,
          "Quick Sort picks a pivot, moves smaller values left, then sorts each side recursively.");
    quickRec(a, 0, n - 1, r);
    r.markAll(n);
    r.lo = r.hi = r.pivot = -1;
    r.add(a, -1, -1, "done", -1, "Sorted!");
}

// ======================= Heap Sort =======================
// Max-heap in the array: parent = (i-1)/2, left = 2i+1, right = 2i+2.
static void heapify(vector<int>& a, int n, int i, Recorder& r) {
    r.calls++;
    int largest = i, l = 2 * i + 1, rt = 2 * i + 2;

    if (l < n) {
        r.comps++;
        bool win = a[l] > a[largest];
        r.add(a, l, largest, "compare", 2,
              "Compare left child " + to_string(a[l]) + " with " + to_string(a[largest]) +
                  (win ? ": it is larger, so it becomes the largest." : ": no change."));
        if (win) largest = l;
    }
    if (rt < n) {
        r.comps++;
        bool win = a[rt] > a[largest];
        r.add(a, rt, largest, "compare", 3,
              "Compare right child " + to_string(a[rt]) + " with " + to_string(a[largest]) +
                  (win ? ": it is larger, so it becomes the largest." : ": no change."));
        if (win) largest = rt;
    }
    if (largest != i) {
        swap(a[i], a[largest]);
        r.moves++;
        r.add(a, i, largest, "swap", 5,
              "Swap the parent with its larger child. " + to_string(a[i]) + " moves up.");
        r.add(a, largest, -1, "call", 6,
              "Recursive call: heapify(a, n, " + to_string(largest) + ") keeps sifting " +
                  to_string(a[largest]) + " down.");
        heapify(a, n, largest, r);  // recursive
    }
}

void heapSort(vector<int> a, Queue& q) {
    Recorder r(q);
    int n = a.size();
    r.heapSize = n;
    r.add(a, -1, -1, "start", -1,
          "Heap Sort: first build a max-heap, then repeatedly move the root (largest) to the end.");

    // Phase 1: build a max-heap
    for (int i = n / 2 - 1; i >= 0; i--) {
        r.add(a, i, -1, "focus", 13,
              "Build max-heap: heapify the node at index " + to_string(i) + " (value " + to_string(a[i]) + ").");
        heapify(a, n, i, r);
    }
    if (n > 1)
        r.add(a, 0, -1, "focus", 14, "Max-heap built. The root holds the largest value.");

    // Phase 2: extract the maximum n-1 times
    for (int end = n - 1; end > 0; end--) {
        swap(a[0], a[end]);
        r.moves++;
        r.add(a, 0, end, "swap", 15,
              "Swap the root with the last heap element. " + to_string(a[end]) +
                  " is now in its final place.");
        r.mark(end);
        r.heapSize = end;  // the heap shrinks by one
        if (end > 1) {
            r.add(a, 0, -1, "focus", 16,
                  "Heapify the root of the remaining heap (size " + to_string(end) + ").");
            heapify(a, end, 0, r);
        }
    }
    r.markAll(n);
    r.heapSize = n;
    r.add(a, -1, -1, "done", -1, "Sorted!");
}
