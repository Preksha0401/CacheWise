# CacheWise - CPU Cache Locality Benchmark

**Investigating how memory-access patterns influence CPU performance using C++17.**

CacheWise is a lightweight C++ benchmarking project that compares row-wise and column-wise matrix traversal to investigate the relationship between memory layout, CPU cache locality, and execution time.

The project uses controlled experiments across multiple matrix sizes to measure runtime differences between two implementations that perform the same computation but access memory differently.

## Overview

Modern processors use a hierarchy of memory, including CPU caches, to reduce the cost of accessing frequently needed data. Consequently, a program's performance can depend not only on the number of operations it performs but also on how it accesses memory.

CacheWise explores this behavior through a simple experiment:

- **Row-wise traversal:** Visits adjacent elements in a row-major matrix.
- **Column-wise traversal:** Visits elements from different rows, producing larger memory strides.
- **Performance measurement:** Records execution time for both traversal strategies across multiple matrix dimensions.
- **Correctness validation:** Verifies that both implementations compute the same sum.
- **Result analysis:** Exports measurements to CSV for comparison and visualization.

## Problem Statement

Two implementations can perform the same mathematical computation and have the same asymptotic time complexity while exhibiting different execution times because of their memory-access patterns.

CacheWise investigates this question:

> How does the order in which a program traverses matrix elements affect execution time, and how can CPU cache locality help explain the observed performance?

## How It Works

For an `N × N` matrix stored as a contiguous, row-major array, the element at row `i` and column `j` is accessed using:

```cpp
matrix[i * N + j]
```

### 1. Row-wise traversal

```cpp
for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
        sum += matrix[i * N + j];
    }
}
```

The inner loop visits adjacent elements in memory. This access pattern can benefit from spatial locality because nearby values are often brought into the same cache line.

### 2. Column-wise traversal

```cpp
for (size_t j = 0; j < N; ++j) {
    for (size_t i = 0; i < N; ++i) {
        sum += matrix[i * N + j];
    }
}
```

The inner loop jumps between rows. In row-major storage, consecutive accesses are separated by `N` elements, potentially reducing cache-line utilization and increasing cache misses.

Both methods visit every matrix element and compute the same sum. The experiment measures whether their execution times differ under the tested conditions.

## Project Workflow

1. Initialize a matrix with deterministic values.
2. Execute both traversal implementations as warm-up runs.
3. Measure each implementation using a monotonic clock.
4. Repeat measurements to reduce reliance on a single timing sample.
5. Verify that both implementations return equivalent results.
6. Calculate average execution times and the observed slowdown ratio.
7. Export measurements to CSV for further analysis.

## Technology Stack

- **Language:** C++17
- **Platform:** Linux or a compatible environment with a C++ compiler
- **Build:** GCC or another C++17-compatible compiler
- **Timing:** C++ `<chrono>` library
- **Data export:** CSV
- **Visualization:** Python and Matplotlib

## Getting Started

### Prerequisites

- GCC or another C++17-compatible compiler
- Python 3 and Matplotlib for plotting results

### Compile

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic \
    cachewise.cpp -o cachewise
```

### Run the benchmark

```bash
./cachewise results.csv
```

The benchmark prints a timing summary in the terminal and writes the measurements to `results.csv`.

### Generate the visualization

After installing Matplotlib:

```bash
python3 -m pip install matplotlib
python3 plot_results.py
```

The plotting script reads `results.csv` and generates a graph comparing the measured execution times.

## Benchmark Design

The benchmark evaluates several matrix dimensions, ranging from smaller matrices to larger workloads.

| Parameter | Description |
|---|---|
| Matrix dimensions | Multiple `N × N` sizes |
| Data representation | Contiguous row-major storage |
| Traversal strategies | Row-wise and column-wise |
| Timing method | Monotonic clock |
| Repeated trials | Multiple measurements per strategy |
| Correctness check | Compare computed sums |
| Output | Terminal summary and CSV measurements |

The matrix is initialized outside the timed region so that the primary measurement focuses on traversal and summation rather than allocation and initialization.

Warm-up runs and repeated trials help reduce some sources of measurement noise, but they do not eliminate all system-level variability.

## Results

Benchmark results will be added after running the experiment on the target machine.

The analysis will examine:

- Execution time as matrix dimensions increase.
- The relative performance of row-wise and column-wise traversal.
- Whether the difference changes for larger working sets.
- Possible explanations involving spatial locality, cache capacity, and memory-access patterns.

The observed results may vary with CPU architecture, cache hierarchy, compiler optimizations, background workload, and other system conditions. No fixed performance improvement is assumed in advance.

## Correctness and Validation

CacheWise validates the experiment by checking that both traversal methods compute equivalent sums for every tested matrix size.

Additional validation includes:

- Successful compilation with warnings enabled.
- Successful generation of CSV output.
- Finite, non-negative timing measurements.
- Repeated executions to check the consistency of observed trends.

## Limitations

CacheWise is a focused microbenchmark, not a general-purpose profiler.

- Execution time alone does not directly measure cache misses.
- Results from one processor may not generalize to other architectures.
- Compiler optimizations can influence the measured difference.
- Warm-up runs and repeated measurements reduce some noise but do not guarantee identical results.
- A timing difference suggests a performance relationship; hardware performance counters would be needed to investigate cache behavior more directly.

## Future Work

Potential extensions include:

- Collecting hardware performance counters using Linux `perf`, where supported and permitted.
- Comparing different compiler optimization levels.
- Investigating cache-miss behavior across working-set sizes.
- Exploring additional memory-access patterns and data layouts.

These extensions are outside the initial project scope.

## Learning Objectives

CacheWise provides practical experience with:

- C++ memory layout and array indexing.
- CPU cache hierarchy and spatial locality.
- Performance benchmarking and timing methodology.
- Correctness checks and controlled experiments.
- CSV-based data analysis.
- Interpreting performance measurements without overclaiming causality.

---

**CacheWise** is a systems-programming learning project focused on understanding the relationship between software memory-access patterns and CPU performance.
