#include "benchmarkAnalyzer.hpp"

int main(int argc, char* argv[]) {
    using namespace BenchmarkAnalyzer;

    if (argc > 1) {
        // ----- non-interactive path -----
        // argv[1] is treated as the source file to benchmark
        benchmarkConfig config(argv[1]);   // uses the ctor that defaults to 5 runs, -O2 -std=c++17
        runBenchmark(config);
        return 0;
    }

    // ----- interactive path -----
    runInteractiveCLI();
    return 0;
}
