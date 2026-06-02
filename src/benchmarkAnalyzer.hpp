#ifndef BENCHMARK_ANALYZER_HPP
#define BENCHMARK_ANALYZER_HPP

#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <climits>
#include <cmath>
#include <sched.h>
#include <pthread.h>
#include <cstdint>
#include <time.h>



namespace BenchmarkAnalyzer {
    auto isCompilerAvailable(const std::string &compiler) -> bool;
    std::string getCompilerVersion(const std::string &compiler);
    std::string getCompilerType(const std::string &compiler);
    std::string detectBestCompiler();

    struct analyzerStatisticParameters {

        // compile time
        // run time
        // memory usage
        // binary size
        // context switches

        double compileTime;    
        double executionTime;
        double userModeProcessorTime;
        double systemKernelModeProcessorTime;

        std::int64_t binarySize;
        std::int64_t maximumMemoryUsageInKB;
        std::int64_t voluntaryContextSwitches;
        std::int64_t involuntaryContextSwitches;
        std::int64_t minorPageFaults;
        std::int64_t majorPageFaults;
        // suck that long d*Ck

        int programExitCode;
    };

    // struct ComplexityAnalysis {
    //     std::string upperBound;        // O(?) - asymptotic upper bound
    //     std::string lowerBound;        // L(?) - asymptotic lower bound
    //     std::string tightBound;        // X(?) - asymptotic tight bound

    //     // never knew the theory i learnt in college would help LOL
    //     // not that it matters anyway because i wouldve researched the signs anyway 

    //     double confidenceScore;        // how confident are we? (0 <--> 1)
    //     std::vector<std::pair<int, double>> dataPoints; // (input_size, time)
    // };\

    // auto compileProgram(
    //     const std::string &sourceFile, 
    //     const std::string &binaryOutput,
    //     const std::string &compilerFlags, 
    //     double &compileTime,
    //     const std::string &compiler = "g++"
    // ) -> bool;

    using BenchmarkFunc = int(*)(int, char**);

    struct benchmarkConfig {
        bool generateAssembly;
        bool enableCoreIsolation;
        bool enableThreadIsolation;   
        bool analyzeComplexity;
        
        int totalExecutionRuns;
        int warmupIterations;
        int targetCoreId;              //  (-1 = no pinning)

        std::string sourceFile;
        std::string assemblyOutputFile;
        std::string compilerFlags;
        std::string compiler;

        // std::vector<int> inputSizes;

        benchmarkConfig(const std::string &src,
                        const std::string &asmOut = "",
                        const std::string &flags = "-O2 -std=c++17 -shared -fPIC",
                        int runs = 5,
                        int warmup = 2,
                        const std::string &comp = "")
            : sourceFile(src), assemblyOutputFile(asmOut)
            , compilerFlags(flags), totalExecutionRuns(runs)
            , warmupIterations(warmup)
            , generateAssembly(!asmOut.empty())
            , enableCoreIsolation(false), targetCoreId(-1)
            , enableThreadIsolation(false)
            , analyzeComplexity(false)
        {
            compiler = comp.empty() ? detectBestCompiler() : comp;
        }
    };

    auto getTimeInSeconds(struct timeval tv) -> double;

    auto executeCommand(const std::string &command) -> std::string;
    // std::string executeCommand(const std::string &program, const std::vector<std::string> &argumentList);
    auto formatBytes(std::int64_t bytes) -> std::string;

    std::int64_t getFileSize(const std::string &filename);

    void printSeparator(int width = 70);
    void printHeader(const std::string &title);
    void printMetric(const std::string &name, const std::string &value, const std::string &unit = "");

    auto compileProgram(
        const std::string &sourceFile, 
        const std::string &binaryOutput,
        const std::string &compilerFlags, 
        double &compileTime,
        const std::string &compiler = "g++"
    ) -> bool;

    bool generateAssembly(
        const std::string &sourceFile,
        const std::string &assemblyOutput,
        const std::string &compilerFlags,
        const std::string &compiler = "g++"
    );

    analyzerStatisticParameters runSingularTimeSlice(BenchmarkFunc func, const benchmarkConfig &config);

    template<typename StatsContainer> // why do i have to do this 2 times

    void printDetailedStatistics(const StatsContainer &allStats, int numRuns);
    void runBenchmark(const benchmarkConfig &config);

    bool setProcessorAffinity(int coreId);
    bool setThreadAffinity(pthread_t thread, int coreId);
    
    int getTotalProcessorCores();
    
    void printProcessorAffinityInfo();
    void displayMainMenu();
    void runInteractiveCLI();

    benchmarkConfig configureInteractiveBenchmark();

} 

#endif
