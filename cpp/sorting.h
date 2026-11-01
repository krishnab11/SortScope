#pragma once
#include <vector>
#include "structures.h"

// Each function sorts a copy of the array and enqueues every step.
void bubbleSort(std::vector<int> a, Queue& steps);
void selectionSort(std::vector<int> a, Queue& steps);
void insertionSort(std::vector<int> a, Queue& steps);
void quickSort(std::vector<int> a, Queue& steps);   // recursive
void heapSort(std::vector<int> a, Queue& steps);    // max-heap + recursive heapify
