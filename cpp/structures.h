#pragma once
#include <string>
#include <vector>

// One recorded moment of a sort: the array plus what just happened.
struct Step {
    std::vector<int> values;    // snapshot of the array
    int a = -1, b = -1;         // indices being compared / swapped / shifted
    std::string op;             // start, focus, compare, swap, shift, insert, call, done
    int line = -1;              // line of the C++ listing that is running (-1 = none)
    int comparisons = 0;        // running totals
    int moves = 0;              // swaps or shifts
    int calls = 0;              // heapify calls (Heap Sort)
    std::string msg;            // plain-English explanation
    std::vector<int> sorted;    // indices already sorted
    int lo = -1, hi = -1;       // active sub-range (Quick Sort)
    int pivot = -1;             // pivot / current minimum
    int heapSize = -1;          // size of the heap part (Heap Sort), -1 = not a heap
};

// Linked-list Queue (FIFO): steps are enqueued while the algorithm runs
// and dequeued in the same order when the JSON is written.
class Queue {
    struct Node {
        Step data;
        Node* next;
    };
    Node* head = nullptr;
    Node* tail = nullptr;
    int count = 0;

public:
    Queue() = default;
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;
    ~Queue() { while (!isEmpty()) dequeue(); }

    void enqueue(const Step& s) {
        Node* n = new Node{s, nullptr};
        if (tail) tail->next = n; else head = n;
        tail = n;
        count++;
    }

    Step dequeue() {
        if (!head) return Step();
        Node* n = head;
        Step s = n->data;
        head = n->next;
        if (!head) tail = nullptr;
        delete n;
        count--;
        return s;
    }

    bool isEmpty() const { return head == nullptr; }
    int size() const { return count; }
};
