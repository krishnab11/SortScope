#pragma once
#include <string>
#include <vector>
#include "structures.h"

using namespace std;

// Keeps the running counters and writes every step into the Queue.
struct Recorder {
    Queue& q;
    vector<int> sorted;
    int comps = 0, moves = 0, calls = 0;
    int lo = -1, hi = -1, pivot = -1, heapSize = -1;

    explicit Recorder(Queue& queue) : q(queue) {}

    void mark(int i) {
        for (int x : sorted) if (x == i) return;
        sorted.push_back(i);
    }
    void markAll(int n) { for (int i = 0; i < n; i++) mark(i); }

    void add(const vector<int>& v, int a, int b, const string& op, int line, const string& msg) {
        Step s;
        s.values = v;
        s.a = a;
        s.b = b;
        s.op = op;
        s.line = line;
        s.comparisons = comps;
        s.moves = moves;
        s.calls = calls;
        s.msg = msg;
        s.sorted = sorted;
        s.lo = lo;
        s.hi = hi;
        s.pivot = pivot;
        s.heapSize = heapSize;
        q.enqueue(s);
    }
};
