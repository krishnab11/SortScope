# SortScope

See every step of sorting, one line of C++ at a time.

SortScope animates five sorting algorithms. The sorting itself runs in **C++**; a small Flask server passes the
numbers to the C++ program, and the browser replays the recorded steps while highlighting the line of code that is running.

| Algorithm | Best | Average | Worst | Space |
|---|---|---|---|---|
| Bubble Sort | O(n) | O(n²) | O(n²) | O(1) |
| Selection Sort | O(n²) | O(n²) | O(n²) | O(1) |
| Insertion Sort | O(n) | O(n²) | O(n²) | O(1) |
| Quick Sort (recursive) | O(n log n) | O(n log n) | O(n²) | O(log n) |
| Heap Sort | O(n log n) | O(n log n) | O(n log n) | O(1) |

Heap Sort is shown as a binary tree and an array at the same time, so you can see a swap happen in both.

## Run

```
pip install -r requirements.txt
python app.py
```

Open http://127.0.0.1:5000. You need `g++` installed; the C++ program compiles itself into `build/` the first time.
To compile by hand: `g++ -std=c++17 -O2 -o build/sortscope cpp/main.cpp cpp/sorting.cpp`

## How it works

1. The browser sends the algorithm name and the numbers to `/api/sort`.
2. Flask checks them (1 to 30 whole numbers between -999 and 999) and runs `build/sortscope`, giving the input on stdin.
3. C++ sorts a copy of the array and records every compare, swap and shift in a linked-list **Queue**
   (each step also carries the number of the code line that is running), then prints the steps as JSON.
4. JavaScript plays the steps back. A **Stack** of visited steps powers the Previous button.

## Project layout

```
sortscope/
├── app.py              Flask bridge + input validation
├── requirements.txt
├── cpp/
│   ├── main.cpp        reads input, runs the algorithm, prints JSON
│   ├── sorting.h/.cpp  bubble, selection, insertion, quick, heap sort
│   ├── recorder.h      counters + writes steps into the Queue
│   └── structures.h    Step + linked-list Queue
├── templates/index.html
└── static/
    ├── style.css
    └── script.js       drawing, code listings, playback, Stack
```

## Viva questions

1. Why is Bubble Sort O(n²)? Which sort does the fewest swaps?
2. How is a heap stored in an array? (parent = (i-1)/2, children = 2i+1 and 2i+2)
3. Why is building a heap O(n) but Heap Sort O(n log n)?
4. Why does Quick Sort use O(log n) space? (the recursion stack)
5. What is the difference between a Stack and a Queue, and where does this project use each?
6. Why record every step instead of only the final result?
