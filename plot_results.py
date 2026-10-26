
import csv
import matplotlib.pyplot as plt

sizes = []
row_times = []
column_times = []

with open("results.csv", "r", newline="") as file:
    reader = csv.DictReader(file)

    for record in reader:
        sizes.append(int(record["matrix_size"]))
        row_times.append(float(record["row_avg_ms"]))
        column_times.append(float(record["column_avg_ms"]))

plt.figure(figsize=(9, 5))

plt.plot(sizes, row_times, marker="o", label="Row-wise traversal")
plt.plot(sizes, column_times, marker="o", label="Column-wise traversal")

plt.xlabel("Matrix dimension (N for N × N)")
plt.ylabel("Average execution time (ms)")
plt.title("CacheWise: Memory Access Pattern Benchmark")
plt.xticks(sizes)
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()

plt.savefig("benchmark_results.png", dpi=200)
plt.show()


# Second graph: relative performance difference
ratios = [
    column / row if row > 0 else 0
    for row, column in zip(row_times, column_times)
]

plt.figure(figsize=(9, 5))
plt.plot(sizes, ratios, marker="o")

plt.axhline(y=1, linestyle="--", label="Equal execution time")

plt.xlabel("Matrix dimension (N for N × N)")
plt.ylabel("Column-wise time / Row-wise time")
plt.title("CacheWise: Relative Traversal Performance")
plt.xticks(sizes)
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()

plt.savefig("performance_ratio.png", dpi=200)
plt.show()
