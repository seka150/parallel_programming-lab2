import csv
import re
import subprocess
import sys
from pathlib import Path

import matplotlib.pyplot as plt

ROOT = Path(__file__).resolve().parent
PROGRAM = ROOT / "build" / "matrix"
GENERATOR = ROOT / "gen.py"

SIZES = [50, 100, 200, 300, 500]
THREAD_COUNTS = [1, 2, 4]
REPEATS = 3

if not PROGRAM.exists():
    print("Error: program not found. Run: cmake --build build")
    sys.exit(1)

results = []

for n in SIZES:
    subprocess.run(
        [sys.executable, str(GENERATOR), str(n)],
        cwd=ROOT,
        check=True
    )

    for thread_count in THREAD_COUNTS:
        sequential_times = []
        parallel_times = []

        for _ in range(REPEATS):
            process = subprocess.run(
                [str(PROGRAM), str(thread_count)],
                cwd=ROOT,
                capture_output=True,
                text=True,
                check=True
            )

            sequential_match = re.search(
                r"Sequential time:\s*([\d.]+)\s*ms",
                process.stdout
            )

            parallel_match = re.search(
                r"Parallel time:\s*([\d.]+)\s*ms",
                process.stdout
            )

            if sequential_match is None or parallel_match is None:
                print("Error: execution times not found")
                print(process.stdout)
                sys.exit(1)

            sequential_times.append(float(sequential_match.group(1)))
            parallel_times.append(float(parallel_match.group(1)))

        sequential_average = sum(sequential_times) / REPEATS
        parallel_average = sum(parallel_times) / REPEATS
        speedup = sequential_average / parallel_average

        results.append((
            n,
            thread_count,
            sequential_average,
            parallel_average,
            speedup
        ))

        print(
            f"Size: {n}x{n}, threads: {thread_count}, "
            f"sequential: {sequential_average:.3f} ms, "
            f"parallel: {parallel_average:.3f} ms, "
            f"speedup: {speedup:.3f}x"
        )

with open(ROOT / "results.csv", "w", newline="") as file:
    writer = csv.writer(file)
    writer.writerow([
        "size",
        "threads",
        "sequential_time_ms",
        "parallel_time_ms",
        "speedup"
    ])
    writer.writerows(results)

for thread_count in THREAD_COUNTS:
    selected = [
        item for item in results
        if item[1] == thread_count
    ]

    sizes = [item[0] for item in selected]
    times = [item[3] for item in selected]

    plt.plot(
        sizes,
        times,
        marker="o",
        label=f"{thread_count} thread(s)"
    )

plt.xlabel("Matrix size (N x N)")
plt.ylabel("Average parallel execution time (ms)")
plt.title("Matrix multiplication performance")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig(ROOT / "performance.png", dpi=150)
plt.show()

print("Saved results.csv and performance.png")