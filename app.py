"""SortScope: a small Flask bridge between the browser and the C++ sorting engine."""
import json
import os
import subprocess

from flask import Flask, jsonify, render_template, request

app = Flask(__name__)

BASE = os.path.dirname(os.path.abspath(__file__))
EXE = os.path.join(BASE, "build", "sortscope" + (".exe" if os.name == "nt" else ""))
SOURCES = [os.path.join(BASE, "cpp", f) for f in ("main.cpp", "sorting.cpp")]
HEADERS = [os.path.join(BASE, "cpp", f) for f in ("sorting.h", "recorder.h", "structures.h")]

ALGORITHMS = {"bubble", "selection", "insertion", "quick", "heapsort"}
MAX_NUMBERS = 30                 # keeps the animation and the heap tree readable
MIN_VALUE, MAX_VALUE = -999, 999


def ensure_compiled():
    """Compile the C++ engine if it is missing or older than its sources."""
    newest = max(os.path.getmtime(f) for f in SOURCES + HEADERS)
    if os.path.exists(EXE) and os.path.getmtime(EXE) >= newest:
        return
    os.makedirs(os.path.dirname(EXE), exist_ok=True)
    subprocess.run(["g++", "-std=c++17", "-O2", "-o", EXE] + SOURCES, check=True)


def is_int(v):
    return isinstance(v, int) and not isinstance(v, bool)


def validate(algorithm, numbers):
    """Return an error message, or None if the request is fine."""
    if algorithm not in ALGORITHMS:
        return "Choose a sorting algorithm."
    if not isinstance(numbers, list) or len(numbers) == 0:
        return "Enter at least one number."
    if len(numbers) > MAX_NUMBERS:
        return f"Use at most {MAX_NUMBERS} numbers."
    if not all(is_int(v) for v in numbers):
        return "Only whole numbers are allowed."
    if any(v < MIN_VALUE or v > MAX_VALUE for v in numbers):
        return f"Numbers must be between {MIN_VALUE} and {MAX_VALUE}."
    return None


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/api/sort", methods=["POST"])
def sort_api():
    data = request.get_json(silent=True) or {}
    algorithm, numbers = data.get("algorithm"), data.get("array")

    error = validate(algorithm, numbers)
    if error:
        return jsonify(error=error), 400

    try:
        ensure_compiled()
        text = f"{algorithm} {len(numbers)} " + " ".join(map(str, numbers))
        result = subprocess.run([EXE], input=text, capture_output=True, text=True, timeout=5)
    except FileNotFoundError:
        return jsonify(error="g++ was not found. Install a C++ compiler and try again."), 500
    except (subprocess.CalledProcessError, subprocess.TimeoutExpired):
        return jsonify(error="The C++ engine failed to run."), 500

    if result.returncode != 0:
        return jsonify(error="The C++ engine rejected the input."), 400
    return jsonify(json.loads(result.stdout))


if __name__ == "__main__":
    ensure_compiled()
    app.run(debug=True)
