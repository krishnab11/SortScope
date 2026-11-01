const $ = (id) => document.getElementById(id);

// ---- Stack: remembers visited steps so "Previous" can step back ----
class Stack {
  constructor() { this.items = []; }
  push(x) { this.items.push(x); }
  pop() { return this.items.pop(); }
  isEmpty() { return this.items.length === 0; }
  clear() { this.items = []; }
  fill(n) { this.items = Array.from({ length: n }, (_, i) => i); }
}

// Everything the page shows about each algorithm.
// The C++ engine sends a "line" number (0-based) with every step; that listing line is highlighted.
const ALGOS = {
  bubble: {
    name: "Bubble Sort", moves: "Swaps",
    desc: "Compares neighbouring values and swaps them when they are out of order. After every pass the largest remaining value settles at the end.",
    lines: [
      "void bubbleSort(vector<int>& a) {",
      "    int n = a.size();",
      "    for (int i = 0; i < n - 1; i++) {",
      "        for (int j = 0; j < n - i - 1; j++) {",
      "            if (a[j] > a[j + 1]) {",
      "                swap(a[j], a[j + 1]);",
      "            }",
      "        }",
      "    }",
      "}",
    ],
    cx: [["Best", "O(n)"], ["Average", "O(n²)"], ["Worst", "O(n²)"], ["Space", "O(1)"]],
  },
  selection: {
    name: "Selection Sort", moves: "Swaps",
    desc: "Finds the smallest value in the unsorted part and swaps it to the front. Makes very few swaps, but always scans the whole unsorted part.",
    lines: [
      "void selectionSort(vector<int>& a) {",
      "    int n = a.size();",
      "    for (int i = 0; i < n - 1; i++) {",
      "        int minIdx = i;",
      "        for (int j = i + 1; j < n; j++)",
      "            if (a[j] < a[minIdx]) minIdx = j;",
      "        swap(a[i], a[minIdx]);",
      "    }",
      "}",
    ],
    cx: [["Best", "O(n²)"], ["Average", "O(n²)"], ["Worst", "O(n²)"], ["Space", "O(1)"]],
  },
  insertion: {
    name: "Insertion Sort", moves: "Shifts",
    desc: "Takes one value at a time and slides it left into its place in the sorted part. Very fast on nearly sorted data.",
    lines: [
      "void insertionSort(vector<int>& a) {",
      "    int n = a.size();",
      "    for (int i = 1; i < n; i++) {",
      "        int key = a[i], j = i - 1;",
      "        while (j >= 0 && a[j] > key) {",
      "            a[j + 1] = a[j];          // shift right",
      "            j--;",
      "        }",
      "        a[j + 1] = key;",
      "    }",
      "}",
    ],
    cx: [["Best", "O(n)"], ["Average", "O(n²)"], ["Worst", "O(n²)"], ["Space", "O(1)"]],
  },
  quick: {
    name: "Quick Sort (recursive)", moves: "Swaps",
    desc: "Picks the last value as pivot, moves smaller values to its left, then sorts the left and right parts recursively. The dimmed bars are outside the current part.",
    lines: [
      "int partition(vector<int>& a, int lo, int hi) {",
      "    int pivot = a[hi], i = lo - 1;",
      "    for (int j = lo; j < hi; j++) {",
      "        if (a[j] < pivot) {",
      "            i++;",
      "            swap(a[i], a[j]);",
      "        }",
      "    }",
      "    swap(a[i + 1], a[hi]);          // pivot in place",
      "    return i + 1;",
      "}",
      "",
      "void quickSort(vector<int>& a, int lo, int hi) {",
      "    if (lo >= hi) return;",
      "    int p = partition(a, lo, hi);",
      "    quickSort(a, lo, p - 1);        // left part",
      "    quickSort(a, p + 1, hi);        // right part",
      "}",
    ],
    cx: [["Best", "O(n log n)"], ["Average", "O(n log n)"], ["Worst", "O(n²)"], ["Space", "O(log n)"]],
  },
  heapsort: {
    name: "Heap Sort", moves: "Swaps",
    desc: "Builds a max-heap, then repeatedly swaps the root (the largest value) to the end and restores the heap. Shown as a tree and an array: parent = (i−1)/2, left = 2i+1, right = 2i+2.",
    lines: [
      "void heapify(vector<int>& a, int n, int i) {",
      "    int largest = i, l = 2*i + 1, r = 2*i + 2;",
      "    if (l < n && a[l] > a[largest]) largest = l;",
      "    if (r < n && a[r] > a[largest]) largest = r;",
      "    if (largest != i) {",
      "        swap(a[i], a[largest]);",
      "        heapify(a, n, largest);      // recursive",
      "    }",
      "}",
      "",
      "void heapSort(vector<int>& a) {",
      "    int n = a.size();",
      "    for (int i = n/2 - 1; i >= 0; i--)   // build max-heap",
      "        heapify(a, n, i);",
      "    for (int end = n - 1; end > 0; end--) {",
      "        swap(a[0], a[end]);              // largest goes to the end",
      "        heapify(a, end, 0);",
      "    }",
      "}",
    ],
    cx: [["Best", "O(n log n)"], ["Average", "O(n log n)"], ["Worst", "O(n log n)"],
         ["Build heap", "O(n)"], ["Space", "O(1)"]],
  },
};

let kind = "bubble";
let steps = [];
let idx = 0;
let timer = null;
const history = new Stack();

const isTree = () => kind === "heapsort";

// ---------- helpers ----------
function showError(msg) { $("error").textContent = msg || ""; }

function parseNumbers() {
  const text = $("numbers").value.trim();
  if (!text) throw new Error("Enter at least one number.");
  const parts = text.split(/[\s,]+/).filter(Boolean);
  if (parts.length > 30) throw new Error("Use at most 30 numbers.");
  return parts.map((p) => {
    if (!/^-?\d+$/.test(p)) throw new Error(`"${p}" is not a whole number.`);
    const v = parseInt(p, 10);
    if (v < -999 || v > 999) throw new Error("Numbers must be between -999 and 999.");
    return v;
  });
}

function randomNumbers() {
  const n = 8 + Math.floor(Math.random() * 9);   // 8..16
  return Array.from({ length: n }, () => 1 + Math.floor(Math.random() * 99));
}

// ---------- side panels: code and info ----------
function showAlgorithm(k) {
  kind = k;
  const a = ALGOS[k];

  $("codeTitle").textContent = `${a.name} in C++`;
  const box = $("code");
  box.innerHTML = "";
  a.lines.forEach((text, i) => {
    const row = document.createElement("div");
    row.className = "ln";
    const no = document.createElement("span");
    no.className = "no";
    no.textContent = i + 1;
    const code = document.createElement("code");
    code.textContent = text || " ";
    row.append(no, code);
    box.appendChild(row);
  });

  $("infoTitle").textContent = a.name;
  $("infoDesc").textContent = a.desc;
  $("complexity").innerHTML = a.cx.map(([x, y]) => `<dt>${x}</dt><dd>${y}</dd>`).join("");
  $("moveLabel").textContent = a.moves;
}

// ---------- talking to the C++ engine ----------
async function load() {
  pause();
  showError("");
  try {
    const array = parseNumbers();
    const algorithm = $("algoSel").value;
    const res = await fetch("/api/sort", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ algorithm, array }),
    });
    const data = await res.json();
    if (!res.ok) throw new Error(data.error || "Something went wrong.");

    showAlgorithm(algorithm);
    if (!isTree()) buildBars(array.length);
    steps = data.steps;
    idx = 0;
    history.clear();
    $("playBtn").disabled = false;
    render();
  } catch (e) {
    showError(e.message);
  }
}

// ---------- drawing ----------
function cellClass(i, s) {
  if (s.op === "done") return "sorted";
  if (i === s.a || i === s.b) {
    if (s.op === "compare") return "compare";
    if (s.op === "swap" || s.op === "shift") return "move";
    return "mark";
  }
  if (i === s.pivot) return "pivot";
  if (s.sorted.includes(i)) return "sorted";
  if (s.lo >= 0 && (i < s.lo || i > s.hi)) return "dim";
  return "";
}

function buildBars(n) {
  const bars = $("bars");
  bars.innerHTML = "";
  bars.classList.toggle("dense", n > 16);
  for (let i = 0; i < n; i++) {
    const bar = document.createElement("div");
    bar.className = "bar";
    bar.appendChild(document.createElement("span"));
    bars.appendChild(bar);
  }
}

function drawBars(s) {
  const first = steps[0].values;
  const lo = Math.min(...first);
  const hi = Math.max(...first);
  const range = hi - lo || 1;
  const bars = $("bars").children;
  for (let i = 0; i < bars.length; i++) {
    const v = s.values[i];
    bars[i].style.height = 12 + ((v - lo) / range) * 78 + "%";
    bars[i].firstChild.textContent = v;
    bars[i].className = "bar " + cellClass(i, s);
  }
}

function drawTree(vals, s) {
  const W = 900;
  const levels = Math.floor(Math.log2(vals.length)) + 1;
  const H = levels * 70 + 20;
  const r = vals.length > 15 ? 17 : 20;
  const pos = vals.map((_, i) => {
    const L = Math.floor(Math.log2(i + 1));
    const k = i + 1 - Math.pow(2, L);
    return { x: ((k + 0.5) / Math.pow(2, L)) * W, y: 36 + L * 70 };
  });
  let svg = `<svg viewBox="0 0 ${W} ${H}" role="img" aria-label="Heap as a binary tree">`;
  for (let i = 1; i < vals.length; i++) {
    const p = pos[Math.floor((i - 1) / 2)];
    svg += `<line class="edge" x1="${p.x}" y1="${p.y}" x2="${pos[i].x}" y2="${pos[i].y}"/>`;
  }
  vals.forEach((v, i) => {
    svg += `<g class="n ${cellClass(i, s)}"><circle cx="${pos[i].x}" cy="${pos[i].y}" r="${r}"/>` +
           `<text x="${pos[i].x}" y="${pos[i].y}">${v}</text></g>`;
  });
  $("tree").innerHTML = svg + "</svg>";
}

function drawCells(vals, s) {
  $("cells").innerHTML = vals
    .map((v, i) => `<div class="cell ${cellClass(i, s)}"><b>${v}</b><small>${i}</small></div>`)
    .join("");
}

function render() {
  const s = steps[idx];
  const tree = isTree();

  $("bars").hidden = tree;
  $("tree").hidden = !tree;
  $("cells").hidden = !tree;

  if (tree) {
    drawTree(s.values.slice(0, s.heapSize), s);
    drawCells(s.values, s);
  } else {
    drawBars(s);
  }

  [...$("code").children].forEach((row, i) => row.classList.toggle("on", i === s.line));
  $("message").textContent = s.msg;
  $("statComp").textContent = s.comparisons;
  $("statMoves").textContent = s.moves;
  $("statCalls").textContent = s.calls;
  $("statStep").textContent = `${idx} / ${steps.length - 1}`;
  $("callsRow").hidden = !tree;

  $("prevBtn").disabled = history.isEmpty();
  $("nextBtn").disabled = idx >= steps.length - 1;
  $("lastBtn").disabled = idx >= steps.length - 1;
  $("resetBtn").disabled = idx === 0;
}

// ---------- playback ----------
function next() {
  if (idx >= steps.length - 1) { pause(); return; }
  history.push(idx);
  idx++;
  render();
  if (idx >= steps.length - 1) pause();
}

function prev() {
  if (history.isEmpty()) return;
  pause();
  idx = history.pop();
  render();
}

function reset() {
  pause();
  idx = 0;
  history.clear();
  render();
}

function showResult() {
  pause();
  idx = steps.length - 1;
  history.fill(idx);
  render();
}

// speed 1 -> ~1200 ms per step, speed 10 -> ~50 ms per step
function delay() { return Math.round(1200 * Math.pow(0.7, Number($("speed").value) - 1)); }

function tick() {
  next();
  if (timer !== null) timer = setTimeout(tick, delay());
}

function applySpeed() {
  const ms = delay();
  $("speedVal").textContent = (1000 / ms).toFixed(ms > 200 ? 1 : 0) + " steps per second";
  document.documentElement.style.setProperty("--dur", Math.min(150, Math.round(ms * 0.8)) + "ms");
  if (timer !== null) {
    clearTimeout(timer);
    timer = setTimeout(tick, ms);
  }
}

function play() {
  if (!steps.length) return;
  if (idx >= steps.length - 1) reset();
  $("playBtn").textContent = "Pause";
  timer = setTimeout(tick, delay());
}

function pause() {
  if (timer !== null) clearTimeout(timer);
  timer = null;
  $("playBtn").textContent = "Play";
}

// ---------- events ----------
$("sortBtn").addEventListener("click", load);
$("randomBtn").addEventListener("click", () => {
  $("numbers").value = randomNumbers().join(", ");
  load();
});
$("algoSel").addEventListener("change", load);
$("numbers").addEventListener("keydown", (e) => { if (e.key === "Enter") load(); });

$("playBtn").addEventListener("click", () => (timer === null ? play() : pause()));
$("nextBtn").addEventListener("click", () => { pause(); next(); });
$("prevBtn").addEventListener("click", prev);
$("resetBtn").addEventListener("click", reset);
$("lastBtn").addEventListener("click", showResult);
$("speed").addEventListener("input", applySpeed);

applySpeed();
showAlgorithm("bubble");
load();
