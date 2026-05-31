#include "benchmarkAnalyzer.hpp"
#include <iostream>

static void printHelp() {
    using namespace BenchmarkAnalyzer;
    std::cout << "\n";
    printHeader("IteraXive - C/C++ Benchmarker");
    std::cout << "  Compiles and runs C/C++ programs, collecting:\n";
    std::cout << "    - Execution time (avg, median, min, max, std dev)\n";
    std::cout << "    - Percentile stability (P25, P75, P95, P99, CoV)\n";
    std::cout << "    - Memory usage\n";
    std::cout << "    - Page faults (minor/major)\n";
    std::cout << "    - CPU time (user + kernel)\n";
    std::cout << "    - Context switches (voluntary/involuntary)\n";
    std::cout << "\n";
    std::cout << "  Usage:\n";
    std::cout << "    ./iteraxive <source-file>     Benchmark a C/C++ source file\n";
    std::cout << "    ./iteraxive                   Launch interactive menu\n";
    std::cout << "    ./iteraxive --help            Show this message\n";
    std::cout << "    ./iteraxive --version         Show version info\n";
    std::cout << "\n";
    std::cout << "  The tool auto-detects available compilers (g++/clang++/gcc/clang).\n";
    std::cout << "  Results are printed to stdout with ANSI color formatting.\n";
    std::cout << "\n";
    printSeparator(60);
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    using namespace BenchmarkAnalyzer;

    if (argc > 1) {
        std::string arg(argv[1]);
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        }
        if (arg == "--version" || arg == "-v") {
            std::cout << "IteraXive v1.0.0\n";
            return 0;
        }
        benchmarkConfig config(argv[1]);
        runBenchmark(config);
        return 0;
    }

    runInteractiveCLI();
    return 0;
}
