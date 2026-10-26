
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using Matrix = std::vector<double>;
using Clock = std::chrono::steady_clock;

struct Measurement {
    double milliseconds;
    double result;
};

double rowWiseSum(const Matrix& matrix, std::size_t n) {
    double sum = 0.0;

    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            sum += matrix[i * n + j];
        }
    }

    return sum;
}

double columnWiseSum(const Matrix& matrix, std::size_t n) {
    double sum = 0.0;

    for (std::size_t j = 0; j < n; ++j) {
        for (std::size_t i = 0; i < n; ++i) {
            sum += matrix[i * n + j];
        }
    }

    return sum;
}

template <typename Function>
Measurement measureMilliseconds(Function function) {
    const auto start = Clock::now();

    const double result = function();

    const auto end = Clock::now();

    // Keep the calculated result observable.
    volatile double observedResult = result;
    (void)observedResult;

    const double elapsed =
        std::chrono::duration<double, std::milli>(end - start).count();

    return {elapsed, result};
}

double median(std::vector<double> values) {
    std::sort(values.begin(), values.end());

    const std::size_t middle = values.size() / 2;

    if (values.size() % 2 == 0) {
        return (values[middle - 1] + values[middle]) / 2.0;
    }

    return values[middle];
}

double average(const std::vector<double>& values) {
    return std::accumulate(values.begin(), values.end(), 0.0)
           / static_cast<double>(values.size());
}

int main() {
    const std::vector<std::size_t> matrixSizes = {
        128, 256, 512, 1024, 2048
    };

    const int trials = 7;

    std::ofstream csv("results.csv");

    if (!csv.is_open()) {
        std::cerr << "Error: Could not create results.csv\n";
        return 1;
    }

    csv << "matrix_size,elements,memory_mib,"
        << "row_avg_ms,row_median_ms,"
        << "column_avg_ms,column_median_ms,"
        << "column_over_row_ratio\n";

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "CacheWise - CPU Cache Locality Benchmark\n";
    std::cout << "Trials per method: " << trials << "\n\n";

    std::cout << std::left
              << std::setw(10) << "Size"
              << std::setw(14) << "Memory (MiB)"
              << std::setw(15) << "Row avg (ms)"
              << std::setw(18) << "Column avg (ms)"
              << "Col/Row ratio\n";

    std::cout << std::string(70, '-') << '\n';

    for (const std::size_t n : matrixSizes) {
        Matrix matrix(n * n);

        // Initialize the data outside the timed region.
        for (std::size_t i = 0; i < matrix.size(); ++i) {
            matrix[i] = static_cast<double>((i % 100) + 1);
        }

        // Warm up both traversal methods.
        const double expectedRow = rowWiseSum(matrix, n);
        const double expectedColumn = columnWiseSum(matrix, n);

        if (expectedRow != expectedColumn) {
            std::cerr << "Correctness check failed for size " << n << '\n';
            return 1;
        }

        std::vector<double> rowTimes;
        std::vector<double> columnTimes;

        rowTimes.reserve(trials);
        columnTimes.reserve(trials);

        for (int trial = 0; trial < trials; ++trial) {
            Measurement row;
            Measurement column;

            // Alternate the measurement order to reduce ordering bias.
            if (trial % 2 == 0) {
                row = measureMilliseconds(
                    [&]() { return rowWiseSum(matrix, n); }
                );

                column = measureMilliseconds(
                    [&]() { return columnWiseSum(matrix, n); }
                );
            } else {
                column = measureMilliseconds(
                    [&]() { return columnWiseSum(matrix, n); }
                );

                row = measureMilliseconds(
                    [&]() { return rowWiseSum(matrix, n); }
                );
            }

            if (row.result != column.result ||
                row.result != expectedRow) {
                std::cerr << "Correctness check failed during trial.\n";
                return 1;
            }

            rowTimes.push_back(row.milliseconds);
            columnTimes.push_back(column.milliseconds);
        }

        const double rowAvg = average(rowTimes);
        const double rowMedian = median(rowTimes);
        const double columnAvg = average(columnTimes);
        const double columnMedian = median(columnTimes);

        const double memoryMiB =
            static_cast<double>(matrix.size() * sizeof(double))
            / (1024.0 * 1024.0);

        const double ratio =
            rowAvg > 0.0 ? columnAvg / rowAvg : 0.0;

        std::cout << std::left
                  << std::setw(10) << n
                  << std::setw(14) << memoryMiB
                  << std::setw(15) << rowAvg
                  << std::setw(18) << columnAvg
                  << ratio << "x\n";

        csv << n << ','
            << matrix.size() << ','
            << memoryMiB << ','
            << rowAvg << ','
            << rowMedian << ','
            << columnAvg << ','
            << columnMedian << ','
            << ratio << '\n';
    }

    csv.close();

    std::cout << "\nBenchmark completed successfully.\n";
    std::cout << "Results saved to results.csv\n";

    return 0;
}
