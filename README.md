# IteraXive

A C/C++ benchmarker for Linux. It compiles a source file and runs the resulting binary multiple times, collecting execution time, memory usage, page faults, context switches, and CPU utilization. Prints everything with ANSI color formatting and gives you percentile breakdowns and a stability score.

AI tools were used during development. Every line was reviewed and refactored before committing.

## What it measures

After each benchmark run, you get:

- Compilation time and binary size
- Execution time -- average, median, min, max, standard deviation
- Percentile stability -- P25, P75, P95, P99, coefficient of variation
- Memory usage -- average, min, max across runs
- Page faults -- minor and major, total and per-run averages
- CPU time -- user-mode and kernel-mode for the last run
- CPU utilization percentage
- Context switches -- voluntary and involuntary
- Program exit code

The stability assessment ranks results as EX+ (excellent, CoV under 5%), AA (good, under 10%), B (moderate, under 20%), or D (trash, over 20%).

## Building

### CMake

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

The CMake build supports AddressSanitizer (`-DITERAXIVE_ENABLE_ASAN=ON`) and UndefinedBehaviorSanitizer (`-DITERAXIVE_ENABLE_UBSAN=ON`).

### Direct g++

```sh
g++ -std=c++17 -O2 src/main.cpp src/benchmarkAnalyzer.cpp -o iteraxive -lpthread
```

### Python script

```sh
python3 scripts/build.py
```

Tries CMake first. Falls back to direct compilation if CMake is not available.

## Usage

### Non-interactive

Pass a source file as the only argument:

```sh
./iteraxive test_programs/bm1.cpp
```

The tool detects which compilers are available (checks g++, clang++, gcc, clang in that order) and picks the first one found. Default compiler flags are `-O2 -std=c++17 -static` (static linking avoids shared library issues in restricted environments). You can override flags and pick a specific compiler in interactive mode.

```sh
./iteraxive --help
./iteraxive --version
```

### Interactive

Run with no arguments to get a menu:

```sh
./iteraxive
```

From there you can run standard benchmarks, enable core isolation (pin the process to a specific CPU), enable thread isolation, configure custom compiler flags and run counts, view system information, or read the built-in help.

## Test programs

The `test_programs/` directory has a few programs you can run through the benchmarker:

- `bm1.cpp` -- bubble sort that repeats until it burns 1.2 seconds of user CPU time. Heavy, useful for measuring consistent timing.
- `test_c.c` -- Fibonacci in C. Lightweight, fast to compile and run.
- `test_python.py` -- Fibonacci in Python. Not compiled by the benchmarker, included for external testing.
- `test_java.java` -- Fibonacci in Java. Same deal.

## Running the test suite

```sh
bash scripts/run_tests.sh
```

The test runner builds the project, runs benchmarks against the C++ and C test programs, verifies compiler detection works, checks that nonexistent source files are handled gracefully, confirms the exit code is 0 on success, and validates that CMakeLists.txt and the Python build script are present and functional.

## How it works

`compileProgram()` checks that the source file exists, then invokes the detected compiler via `fork()` + `execvp()` (no shell, no injection risk). The child process has CPU and memory limits applied via `setrlimit()` before the compiler runs.

`runSingularTimeSlice()` forks again. The child can be pinned to a specific core using `sched_setaffinity()`. Resource limits are applied -- 30 seconds of CPU time, 1GB of address space. The parent calls `wait4()` which fills a `rusage` struct with memory, page fault, context switch, and CPU time data.

`printDetailedStatistics()` computes averages, percentiles, standard deviation, and the coefficient of variation. It sorts execution times and picks out P25, P75, P95, and P99 values. The stability rank is based on CoV thresholds.

The compiled binary goes to `/tmp` with a name based on the PID and a timestamp. It gets cleaned up when the benchmark finishes.

## Compiler detection

The tool checks for `g++`, `clang++`, `gcc`, and `clang` by running `<compiler> --version` in a forked child. The first one that returns output is selected. In interactive mode you can override this and pick from the list manually.
