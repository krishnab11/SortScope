// Reads from stdin:  <bubble|selection|insertion|quick|heapsort> <n> <v1 .. vn>
// Writes the recorded steps as JSON to stdout.
#include <iostream>
#include <string>
#include <vector>
#include "sorting.h"

using namespace std;

static void printArray(const vector<int>& v) {
    cout << "[";
    for (size_t i = 0; i < v.size(); i++) cout << (i ? "," : "") << v[i];
    cout << "]";
}

int main() {
    string algo;
    int n;
    if (!(cin >> algo >> n) || n <= 0) {
        cerr << "bad input\n";
        return 1;
    }
    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        if (!(cin >> arr[i])) {
            cerr << "bad input\n";
            return 1;
        }
    }

    Queue steps;
    if (algo == "bubble") bubbleSort(arr, steps);
    else if (algo == "selection") selectionSort(arr, steps);
    else if (algo == "insertion") insertionSort(arr, steps);
    else if (algo == "quick") quickSort(arr, steps);
    else if (algo == "heapsort") heapSort(arr, steps);
    else {
        cerr << "unknown algorithm\n";
        return 1;
    }

    cout << "{\"algorithm\":\"" << algo << "\",\"total_steps\":" << steps.size() << ",\"steps\":[";
    bool first = true;
    while (!steps.isEmpty()) {
        Step s = steps.dequeue();  // FIFO: same order they were recorded
        if (!first) cout << ",";
        first = false;
        cout << "{\"values\":";
        printArray(s.values);
        cout << ",\"a\":" << s.a << ",\"b\":" << s.b << ",\"op\":\"" << s.op << "\""
             << ",\"line\":" << s.line << ",\"comparisons\":" << s.comparisons
             << ",\"moves\":" << s.moves << ",\"calls\":" << s.calls
             << ",\"lo\":" << s.lo << ",\"hi\":" << s.hi << ",\"pivot\":" << s.pivot
             << ",\"heapSize\":" << s.heapSize << ",\"sorted\":";
        printArray(s.sorted);
        cout << ",\"msg\":\"" << s.msg << "\"}";
    }
    cout << "]}\n";
    return 0;
}
