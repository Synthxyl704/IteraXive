#include "benchmarkAnalyzer.hpp"

// i hate preprocessors

#include <filesystem>
#include <functional>
#include <iomanip> // Include for std::setw and std::left/right
#include <strings.h>

namespace BenchmarkAnalyzer {
    namespace colors {
        const std::string RESET{"\033[0m"};
        const std::string BOLD{"\033[1m"};
        const std::string DIM{"\033[2m"};
        const std::string UNDERLINE{"\033[4m"};

        const std::string BLACK{"\033[30m"};
        const std::string RED{"\033[31m"};
        const std::string GREEN{"\033[32m"};
        const std::string YELLOW{"\033[33m"};
        const std::string BLUE{"\033[34m"};
        const std::string MAGENTA{"\033[35m"};
        const std::string CYAN{"\033[36m"};
        const std::string WHITE{"\033[37m"};

        const std::string BRIGHT_BLACK{"\033[90m"};
        const std::string BRIGHT_RED{"\033[91m"};
        const std::string BRIGHT_GREEN{"\033[92m"};
        const std::string BRIGHT_YELLOW{"\033[93m"};
        const std::string BRIGHT_BLUE{"\033[94m"};
        const std::string BRIGHT_MAGENTA{"\033[95m"};
        const std::string BRIGHT_CYAN{"\033[96m"};
        const std::string BRIGHT_WHITE{"\033[97m"};

        const std::string BG_BLACK{"\033[40m"};
        const std::string BG_RED{"\033[41m"};
        const std::string BG_GREEN{"\033[42m"};
        const std::string BG_YELLOW{"\033[43m"};
        const std::string BG_BLUE{"\033[44m"};
        const std::string BG_MAGENTA{"\033[45m"};
        const std::string BG_CYAN{"\033[46m"};
        const std::string BG_WHITE{"\033[47m"};
    }

    /*
        struct timeval {
            time_t tv_sec;       // s
            suseconds_t tv_usec; // micro s
        };
    */

    // /* [[__attribute_maybe_unused__]] */ double getTimeInSeconds(struct timeval
    // timeValStruct) { /* <sys/time.h> */ {
    // //     // return (timeValStruct.tv_sec + (timeValStruct.tv_usec /
    // static_cast<float>(1000000.0))); // force FP division
    // //     // if 10 seconds and 500000 micro s
    // //     // 10 + (500000 / 1000000.0) = 10.50(s)

    // //     return (timeValStruct.tv_sec + (timeValStruct.tv_usec / 1000000.0));
    // // force FP division
    // //     // [[time_t]]
    // //     // the fuck? nah let double do its conversion thing im not messing w/
    // this
    // // }   // this looks like ass im so sorry

    // // timeval uses NTP synchronization and is prone to whatever DST is

    // /*
    // struct timespec {
    //     time_t tv_sec;
    //     long tv_nsec;
    // };

    // https://www.educative.io/answers/what-is-timespec-in-c
    // */

    //     // CLOCK_MONOTONIC is a faithful b*tch to systime t-fore better for
    //     temporal elapse
    //     // double timeInSeconds {timeSpecStruct.tv_sec +  (timeSpecStruct.tv_nsec
    //     / 1E9)};

    //     // return /* YOU CAN USE CURLY BRACES ON RETURN??? */ {
    //         // static_cast<double>(timeSpecStruct.tv_sec +
    //         (timeSpecStruct.tv_nsec / 1E+9))
    //     // };

    //     return (timeValStruct.tv_sec + (timeValStruct.tv_usec / 1000000.0));
    // }

    double getTimeInSeconds(struct timeval timeValStruct) {
        return (timeValStruct.tv_sec + (timeValStruct.tv_usec / 1000000.0)); // force FP / div
    }

    // bool isAllowedCommand(const std::string& cmd) {
    //     static const std::vector<std::string> allowed = {
    //         // "ls", "whoami", "date"
    //     };

    //     return {
    //         std::find(allowed.begin(), allowed.end(), cmd) != allowed.end()
    //     };
    // }

    // std::string executeCommand(const std::string& command) {
    //     if (!isAllowedCommand(command)) {
    //         return "[[xih7o]]_P:FAILURE";
    //     }

    //     // int pipefd[2];
    //     std::array<int, 2> pipefd;
    //     if (pipe(pipefd.data()) == (-1)) { return "[[xih7o]]_PFAILURE"; }

    //     pid_t pid {fork()};
    //     if (pid == 0) {
    //         dup2(pipefd[1], STDOUT_FILENO); // macro 1 
    //         close(pipefd[0]);

    //         char *args[] = { const_cast<char*>(command.c_str()), nullptr };
    //         execvp(args[0], args);
    //         // _Exit(1);
    //         _exit(1);
    //     }

    //     close(pipefd[1]);

    //     char buffer[128];
    //     std::string result;
    //     ssize_t count;
    //     // SSIZE_MAX // LONG_MAX
    //     while ((/*ssize_t*/ count = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
    //         result.append(buffer, count);
    //     }

    //     close(pipefd[0]);
    //     waitpid(pid, nullptr, 0);
    //     return (result);
    // }

    std::string executeCommand(const std::string &command) {
        FILE *pipe{popen(command.c_str(), "r")};
        if (pipe == NULL || !(pipe)) {
            return "";
        }

        char buffer[128];
        std::string result {""};

        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }

        pclose(pipe);
        return result;
    }

    // long getFileSize(const std::string &filename) {
    //     std::ifstream file(filename, std::ios::binary | std::ios::ate);
    //     if (!file.is_open()) { return (-1); }

    //     return file.tellg();
    // }

    std::int64_t /* force 64 bit, dont care about multi-architecture right now */
    getFileSize(const std::string &filenameToParse) {
        std::ifstream file(filenameToParse, std::ios::binary | std::ios::ate); // whats with the "|" between enumeration flag constants?
        // open the executable in binary mode
        // add iterator ATE (at the end)
        // note - can be relative or absolute, need to add checks here later
        if (!file) {
            return (-1);
        }

        // int y {sizeof(std::int32_t)};
        return static_cast<std::int64_t>(file.tellg());
    }

    auto compileProgram( // not risking PxV because updating errors due to local copying, stick to [&]PxR
        const std::string &sourceFile, const std::string &binaryOutput,
        const std::string &compilerFlags, double &compileTime) -> bool {
        auto timeSavePoint_START {std::chrono::high_resolution_clock::now()};

        // g++ -0[X] sex.cpp -o sex 2>&1
        // ./sex
        std::string compileCommand{"g++ " + compilerFlags + " " + sourceFile + " -o " + binaryOutput + " 2>&1"};
        std::string output{executeCommand(compileCommand)};

        auto timeSavePoint_END {std::chrono::high_resolution_clock::now()};
        // TSP1 - TSP0 = CT
        compileTime = std::chrono::duration<double>(timeSavePoint_END - timeSavePoint_START).count();
        // juncture iterator arithmetic returns casted double to CT

        if (!output.empty() && output.find("error") != std::string::npos) {
            std::cerr << colors::BRIGHT_RED << "Compilation errors found:\n" << colors::RESET << output << std::endl;
            return false;
        } return true;
    }

    // std::string x {_SC_PRO}

    bool generateAssembly(const std::string &sourceFile, const std::string &assemblyOutput, const std::string &compilerFlags) {
        // g++ -0[x] -S sex.cpp -o sex 2>&1
        std::string asmCommand = "g++ " + compilerFlags + " -S " + sourceFile + " -o " + assemblyOutput + " 2>&1";
        // 2>&1 merges stderr into stdout so the function can capture compiler messages (errors)

        // if (!theCommandExecutingCode.empty() && theCommandExecutingCode.find("error") != std::string::npos) {
        //     std::cerr << "Assembly generation errors found:\n" << theCommandExecutingCode << std::endl;
        //     return false;
        // }

        int exitCode {system(asmCommand.c_str())};
        if (exitCode != 0) {
            std::cerr << colors::BRIGHT_RED << "\n\nAssembly file generation failure\n\n" << colors::RESET;
            return false;
        }

        std::cout << colors::BRIGHT_CYAN << "  Assembly run command: " << colors::RESET << assemblyOutput << std::endl;
        /* long */ std::int64_t generatedAssemblyFileSize {getFileSize(assemblyOutput)};

        if (generatedAssemblyFileSize >= 0) {
            printMetric("Assembly File Size", formatBytes(generatedAssemblyFileSize));
        } return true;
    }

    auto runSingularTimeSlice(const std::string &binaryPath, const benchmarkConfig &config) -> analyzerStatisticParameters {
        // analyzerStatisticParameters statisticStruct = runSingularTimeSlice();
        analyzerStatisticParameters statisticStruct = {};

        // pid_t pid {_POSIX_SPAWN}; // fork();
        pid_t pid {fork()};

        if (pid == 0) {
            // CHILD PROCESS - apply core isolation here so the benchmark runs on specified core
            if (config.enableCoreIsolation && config.targetCoreId >= 0) {
                cpu_set_t totalProcessorSet;
                CPU_ZERO(&totalProcessorSet);
                CPU_SET(config.targetCoreId, &totalProcessorSet);
                sched_setaffinity(0, sizeof(cpu_set_t), &totalProcessorSet);
            }

            // apply thread isolation (same as core for single-threaded child)
            if (config.enableThreadIsolation && config.targetCoreId >= 0) {
                cpu_set_t totalProcessorSet;
                CPU_ZERO(&totalProcessorSet);
                CPU_SET(config.targetCoreId, &totalProcessorSet);
                pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &totalProcessorSet);
            }

            execl(binaryPath.c_str(), binaryPath.c_str(), nullptr);
            exit(1);
        } else if (pid > 0) {
            auto startProgramExecution {std::chrono::high_resolution_clock::now()};

            int status; // POSIX wait status (SIGTERM, core dump, successful exit, etc) 
                        // the core dump contains RAM memory, CPU state, stack & heap pointers, IP, TS, etx. 
                        // i really do go on side quests learning this shit

            struct rusage usage; // struct rusage is defined inside <sys/resource.h>

            // pid_t wait4(pid_t pid, int *status, int options, struct rusage *rusage);
            wait4(pid, &status, 0, &usage); // wait4 is a linux syscall which waits for c-process termination

            auto endProgramExecution {std::chrono::high_resolution_clock::now()};

            statisticStruct.executionTime                 = std::chrono::duration<double>(endProgramExecution - startProgramExecution).count();
            statisticStruct.programExitCode               = (WIFEXITED(status)) ? WEXITSTATUS(status) : (-1);
            statisticStruct.maximumMemoryUsageInKB        = usage.ru_maxrss;                    // maximum resident set size = max main memory usage in kilobytes (base 10?)
            statisticStruct.userModeProcessorTime         = getTimeInSeconds(usage.ru_utime);   // user mode processor execution
            statisticStruct.systemKernelModeProcessorTime = getTimeInSeconds(usage.ru_stime);   // kernel mode processor execution
            statisticStruct.voluntaryContextSwitches      = usage.ru_nvcsw;                     // voluntary CTxS (conditionals, mutexes futexes whatever)
            statisticStruct.involuntaryContextSwitches    = usage.ru_nivcsw;                    // involuntary CTxS (time slice expiration, hierarchy stuff etc)
                                                                                                // WHO THE FUCK OFFICIALIZED THIS CONVENTION??
            // a page fault happens when a process tries to access a virtual memory page
            // that is not currently mapped in its page table
            // handled by the OS, NOT an error
            statisticStruct.minorPageFaults               = usage.ru_minflt;
            statisticStruct.majorPageFaults               = usage.ru_majflt;
            // long int / int
            // i64_TESTED_LINUX_DT minorPageFaults;
            // i64_TESTED_LINUX_DT majorPageFaults;
        } else {
            std::cerr << colors::BRIGHT_RED << "Fork failure due to unknown inference." << colors::RESET << std::endl;
        } return statisticStruct;
    }

    // ---

    void printSeparator(int width) {
        std::cout << colors::DIM << std::string(width, '-') << colors::RESET << std::endl;
    }

    void printHeader(const std::string &title) {
        std::cout << "\n" << colors::BOLD << colors::BRIGHT_MAGENTA;
        std::cout << "[ " << title << " ]";
        std::cout << colors::RESET << "\n";
        printSeparator(60);
    }

    void printMetric(const std::string &name, const std::string &value, const std::string &unit) {
        std::cout << colors::BRIGHT_YELLOW << std::left << std::setw(35) << name << colors::RESET;
        std::cout << colors::BRIGHT_WHITE << std::right << std::setw(15) << value << colors::RESET;
        if (!unit.empty()) {
            std::cout << " " << colors::DIM << unit << colors::RESET;
        } std::cout << std::endl;
    }

    // ---

    std::string formatBytes(long bytes) {
        // const char *units[] = {"B", "KB", "MB", "GB"};
        std::vector<std::string> units = {"B", "KB", "MB", "GB"};
        int unitIndex {0};
        double size{static_cast<double>(bytes)};
        // double size {bytes};

        while (size >= 1024 && unitIndex < 3) {
            size /= (1024); // really dont care, just change in case if the KiB size is different in your architecture
            unitIndex += 1;
        }

        std::ostringstream outputStringStream;
        outputStringStream << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
        return (outputStringStream.str());
    }

    template <typename StatsContainer> // whatever happened to <typename T> fuck whoever writes typename T
    void printDetailedStatistics(const StatsContainer &allStatisticStructVector, int numberOfExecRuns) {
        std::vector<double> numOfExecutionTimes;
        // index(, int c)
        std::vector<long> memoryUsages;

        double totalExecutionTime {0};

        long   totalMemory {0};
        long minimumMemory {LONG_MAX}; // ?
        long maximumMemory {0};

        for (const auto &statisticStruct : allStatisticStructVector) {
            numOfExecutionTimes.emplace_back(statisticStruct.executionTime);
            memoryUsages.emplace_back(statisticStruct.maximumMemoryUsageInKB);

            totalExecutionTime += statisticStruct.executionTime;
            totalMemory += statisticStruct.maximumMemoryUsageInKB;

            minimumMemory = std::min(minimumMemory, statisticStruct.maximumMemoryUsageInKB);
            maximumMemory = std::max(maximumMemory, statisticStruct.maximumMemoryUsageInKB);
        }

        double averageExecutionTime {totalExecutionTime / numberOfExecRuns};
        double averageMemoryUsage {static_cast<double>(totalMemory) / numberOfExecRuns};

        // statisticStruct.executionTime = std::chrono::duration<double>(endProgramExecution - startProgramExecution).count();
        // https://stackoverflow.com/questions/33268513/calculating-standard-deviation-variance-in-c

        double variance {0};
        for (double individualTimeIndex : numOfExecutionTimes) {
            variance += (individualTimeIndex - averageExecutionTime) * (individualTimeIndex - averageExecutionTime);
            // variance += pow((individualTimeIndex - averageExecutionTime), 2);
            // i dont really know what this does in a grand scheme of things 
            // but it will add all the squared differences so large variance number would be a more inconsistent execution time, vice versa for small variance
        }

        double standardDeviation {std::sqrt((variance / numberOfExecRuns) /* overthinking pro max */ )}; 
        // std devn is the square root of variance also brings back in seconds so the sqrt is important
        // if your average run time is 5 seconds and std dev is 0.5 seconds, most runs will be close to ( 5 seconds ± 0.1 seconds)

        std::sort(numOfExecutionTimes.begin(), numOfExecutionTimes.end()); // introsort O(log N)

        double medianTime{numOfExecutionTimes[numberOfExecRuns / 2]}; // middle value 
        double minimumExecutionTime{numOfExecutionTimes.front()}; // minimum value at front
        double maximumExecutionTime{numOfExecutionTimes.back()}; // maximum value at last

        int p25Index {static_cast<int>(numberOfExecRuns * 0.25)};
        int p75Index {static_cast<int>(numberOfExecRuns * 0.75)};
        int p95Index {static_cast<int>(numberOfExecRuns * 0.95)};
        int p99Index {static_cast<int>(numberOfExecRuns * 0.99)};

        double p25Time {numOfExecutionTimes[p25Index]};
        double p75Time {numOfExecutionTimes[p75Index]};
        double p95Time {(p95Index < numberOfExecRuns) ? numOfExecutionTimes[p95Index] : maximumExecutionTime};
        double p99Time {(p99Index < numberOfExecRuns) ? numOfExecutionTimes[p99Index] : maximumExecutionTime};

        // double coefficientOfVariation {(double)(standardDeviation / averageExecutionTime) * 100};
        double coefficientOfVariation {((standardDeviation / averageExecutionTime) * 100)}; // im the backet god
        
        printHeader("TIMING METRICS");
        printMetric("Average execution time", std::to_string(averageExecutionTime * 1000), "ms");
        printMetric("Median execution time", std::to_string(medianTime * 1000), "ms");
        printMetric("Minimum execution time", std::to_string(minimumExecutionTime * 1000), "ms");
        printMetric("Maximum execution time", std::to_string(maximumExecutionTime * 1000), "ms");
        printMetric("Standard deviation", std::to_string(standardDeviation * 1000), "ms");

        printHeader("PERCENTILE STABILITY");
        printMetric("25th percentile [%25]", std::to_string(p25Time * 1000), "ms");
        printMetric("75th percentile [%75]", std::to_string(p75Time * 1000), "ms");
        printMetric("95th percentile [%95]", std::to_string(p95Time * 1000), "ms");
        printMetric("99th percentile [%99]", std::to_string(p99Time * 1000), "ms");
        printMetric("Coeff. of variation (CoV)", std::to_string(coefficientOfVariation), "%");

        std::string stabilityAssessment;
        // if (coefficientOfVariation < 5.0) {
        //     stabilityAssessment = "[EX+]"; // excellent
        // } else if (coefficientOfVariation < 10.0) {
        //     stabilityAssessment = "[AA]";  // good
        // } else if (coefficientOfVariation < 20.0) {
        //     stabilityAssessment = "[B]";   // moderate
        // } else {
        //    stabilityAssessment = "[D]";   // trash
        // }

        stabilityAssessment = (coefficientOfVariation < 5.0)    ? "[EX+]" // excellent
                              : (coefficientOfVariation < 10.0) ? "[AA]"  // good
                              : (coefficientOfVariation < 20.0) ? "[B]"   // moderate?
                                                                : "[D]";  // trash!

        printMetric("Stability assessment rank", stabilityAssessment);

        printHeader("MEMORY METRICS");
        printMetric("Average Memory Usage", formatBytes(static_cast<long>(averageMemoryUsage * 1024)));
        printMetric("Minimum Memory Usage", formatBytes(minimumMemory * 1024));
        printMetric("Maximum Memory Usage", formatBytes(maximumMemory * 1024));

        printHeader("PROCESSOR METRICS (latest run)");
        const auto &lastRun{allStatisticStructVector.back()};
        printMetric("User CPU Time (user-mode)", std::to_string(lastRun.userModeProcessorTime * 1000), "ms");
        printMetric("System CPU Time (kernel-mode)", std::to_string(lastRun.systemKernelModeProcessorTime * 1000), "ms");

        double totalActualProcessorTime = (lastRun.userModeProcessorTime + lastRun.systemKernelModeProcessorTime) * 1000; // convert to ms
        double processorUtilization = (totalActualProcessorTime / (lastRun.executionTime * 1000)) * 100;

        // double processorUtilization {
        //     ((lastRun.systemKernelModeProcessorTime + lastRun.systemKernelModeProcessorTime) / (lastRun.executionTime * 1000)) * 100
        // };

        printMetric("CPU Utilization", std::to_string(processorUtilization), "%");

        printHeader("CONTEXT SWITCHES (Last Run)");
        printMetric("Voluntary CTxS", std::to_string(lastRun.voluntaryContextSwitches));
        printMetric("Involuntary CTxS", std::to_string(lastRun.involuntaryContextSwitches));

        printHeader("PROGRAM EXIT STATUS");
        if (lastRun.programExitCode == 0) {
            // int exitCode {system(asmCommand.c_str())};
            printMetric("Exit Code / ExC", std::to_string(lastRun.programExitCode) + " [SUCCESS]");
        } else {
            std::cout << colors::BRIGHT_RED;
            printMetric("Exit Code / ExC", std::to_string(lastRun.programExitCode) + " [FAILURE]");
            std::cout << colors::RESET;
        }
        std::cout << "\n";
    }

    void runBenchmark(const benchmarkConfig &benchMarkConfigStruct) {
        printHeader("Welcome 2 IteraXive");

        printMetric("Source File", benchMarkConfigStruct.sourceFile);
        printMetric("Compiler Flags", benchMarkConfigStruct.compilerFlags);
        printMetric("Number of Runs", std::to_string(benchMarkConfigStruct.totalExecutionRuns));
        printMetric("Warmup Iterations", std::to_string(benchMarkConfigStruct.warmupIterations));

        if (benchMarkConfigStruct.generateAssembly) {
            printMetric("Assembly Output", benchMarkConfigStruct.assemblyOutputFile);
        }

        if (benchMarkConfigStruct.enableCoreIsolation) {
            std::string coreInfo{"ENABLED (Core " + std::to_string(benchMarkConfigStruct.targetCoreId) + ")"};
            printMetric("Core Isolation", coreInfo);
        }

        if (benchMarkConfigStruct.enableThreadIsolation) {
            printMetric("Thread Isolation", "ENABLED");
        }

        // if (benchMarkConfigStruct.analyzeComplexity) {
        //     std::cout << colors::BRIGHT_YELLOW << "  Complexity Analysis : " << colors::RESET
        //               << colors::BRIGHT_GREEN << "ENABLED" << colors::RESET << "\n";
        // }
        std::cout << '\n';

        if (benchMarkConfigStruct.enableCoreIsolation && benchMarkConfigStruct.targetCoreId >= 0) {
            if (setProcessorAffinity(benchMarkConfigStruct.targetCoreId)) {
                std::cout << colors::BRIGHT_GREEN << "  CPU affinity set to core " 
                          << benchMarkConfigStruct.targetCoreId << colors::RESET << "\n";
                printProcessorAffinityInfo();
            } else {
                std::cerr << colors::BRIGHT_RED << "  Failed to set CPU affinity to core " 
                          << benchMarkConfigStruct.targetCoreId << colors::RESET << "\n";
            }
        }

        const std::filesystem::path temporaryDirectory {std::filesystem::temp_directory_path()};
        // C:\Users\<user>\someDir1\hentai
        // /tmp
        const std::string binaryName {"main_" + std::to_string(getpid()) + "_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())};

        const std::filesystem::path binaryPath {(temporaryDirectory / binaryName)}; // concatenation conduction

        const auto cleanup = [&]() { // RAII cleanup
            std::error_code cleanUpErrorCode;
            std::filesystem::remove(binaryPath, cleanUpErrorCode); // ignore errors
            if (benchMarkConfigStruct.generateAssembly) {
                std::filesystem::remove(benchMarkConfigStruct.assemblyOutputFile, cleanUpErrorCode);
            }
        };

        struct cleanupGuard {
            std::function<void()> fx_CC_callDuringScopeExit;
            ~cleanupGuard() { fx_CC_callDuringScopeExit(); }
            // i didnt know structs could have destrutors
        } guard{[&]() { cleanup(); }};

        printHeader("COMPILATION PHASE");

        double compileTime {0.0};
        if (!compileProgram(benchMarkConfigStruct.sourceFile, binaryPath.string(), benchMarkConfigStruct.compilerFlags, compileTime)) {
            std::cerr << colors::BRIGHT_RED << "Compilation failure due to variant inference." << colors::RESET << "\n";
            return;
        }

        const long binarySize {getFileSize(binaryPath.string())};
        printMetric("Compilation Time", std::to_string(compileTime), "s");
        printMetric("Binary Size", formatBytes(binarySize));

        if (benchMarkConfigStruct.generateAssembly) {
            std::cout << '\n';
            printHeader("ASSEMBLY GENERATION");
            if (!generateAssembly(benchMarkConfigStruct.sourceFile, benchMarkConfigStruct.assemblyOutputFile, benchMarkConfigStruct.compilerFlags)) {
                std::cerr << colors::BRIGHT_RED << "Assembly generation failed - continuing without it." << colors::RESET << "\n";
            } else {
                const long asmSize{getFileSize(benchMarkConfigStruct.assemblyOutputFile)};
                if (asmSize >= 0) {
                    printMetric("Assembly File Size", formatBytes(asmSize));
                }
            }
        }

        if (benchMarkConfigStruct.warmupIterations > 0) {
            std::cout << "\n";
            printHeader("WARMUP PHASE");
            for (int inx {0}; inx < benchMarkConfigStruct.warmupIterations; inx += 1) {
                runSingularTimeSlice(binaryPath.string(), benchMarkConfigStruct);
                std::cout << colors::BRIGHT_CYAN << "  Warmup iteration " << colors::RESET 
                          << colors::BRIGHT_WHITE << (inx + 1) 
                          << colors::DIM << '/' << benchMarkConfigStruct.warmupIterations << colors::RESET 
                          << colors::BRIGHT_GREEN << " completed." << colors::RESET << "\n";
            }
        }

        std::cout << "\n";
        printHeader("EXECUTION PHASE");
        std::vector<analyzerStatisticParameters> allStatistics;
        allStatistics.reserve(benchMarkConfigStruct.totalExecutionRuns);

        for (int inx {1}; inx <= benchMarkConfigStruct.totalExecutionRuns; inx += 1) {
            auto stats {runSingularTimeSlice(binaryPath.string(), benchMarkConfigStruct)};

            stats.compileTime = compileTime;
            stats.binarySize = binarySize;

            allStatistics.push_back(std::move(stats));
            std::cout << colors::BRIGHT_BLUE << "  Run " << colors::RESET 
                      << colors::BRIGHT_WHITE << inx 
                      << colors::DIM << '/' << benchMarkConfigStruct.totalExecutionRuns << colors::RESET 
                      << colors::BRIGHT_GREEN << " completed." << colors::RESET << "\n";
        }

        std::cout << "\n";
        printHeader("DETAILED STATISTICS");
        printDetailedStatistics(allStatistics, benchMarkConfigStruct.totalExecutionRuns);
        // cleanup is performed automatically by the guard
    }

    // --- PROCESSOR AFFINITY CUSTOM THING HERE ---
    // https://stackoverflow.com/questions/280909/how-to-set-cpu-affinity-for-a-process-from-c-or-c-in-linux
    // http://www.polarhome.com/service/man/?qf=CPU_ZERO&tf=2&of=CentOS&sf=
    // https://www.reddit.com/r/CodingHelp/comments/16xih7o/changing_processor_affinity_in_program_on_terminal/

    auto setProcessorAffinity(int coreId) -> bool {
        cpu_set_t totalProcessorSet; // this is strictly POSIX compliant DST, wont work on wind*ws systems

        // https://linux.die.net/man/3/cpu_zero
        CPU_ZERO(&totalProcessorSet);
        CPU_SET(coreId, &totalProcessorSet); // setting singular processor affinity implements core isolation

        pid_t pid {getpid()};
        // int sched_setaffinity(pid_t pid, size_t totalProcessorSetsize, cpu_set_t *mask);
        // int result {sched_setaffinity(pid, sizeof(totalProcessorSet), &cpuset)}; // minimal core numeric
        int result {sched_setaffinity(
            pid, sizeof(cpu_set_t), &totalProcessorSet
        )}; // maximal core numeric
        // schedule isolated processor affinity to extracted PID

        if (result != 0) {
            perror("sched_setaffinity");
            return false;
        }   return true;
    }

    // int getTotalProcessorCores() {
    //     if (static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN)) < 0) {
    //         return {
    //             static_cast<int>(sysconf(_SC_NPROCESSORS_CONF)) // runtime 32-bit syscall
    //         };
    //     } else {
    //         return {
    //             static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN))
    //         };
    //     }
    // }

    auto getTotalProcessorCores() -> int {
        long currentOnlineProcessors {sysconf(_SC_NPROCESSORS_ONLN)};
        return (static_cast<int>(currentOnlineProcessors < 0 ? sysconf(_SC_NPROCESSORS_CONF) 
                                                             : currentOnlineProcessors));
    }

    void printProcessorAffinityInfo() {
        cpu_set_t totalProcessorSet;
        CPU_ZERO(&totalProcessorSet);
        pid_t pid {getpid()}; // so convenient
        if (sched_getaffinity(pid, sizeof(cpu_set_t), &totalProcessorSet) == 0) {
            std::cout << colors::BRIGHT_CYAN << "  Current CPU affinity mask: " << colors::RESET;
            for (int inx {0}; inx < getTotalProcessorCores(); inx += 1) {
                // https://linux.die.net/man/3/cpu_isset
                // CPU_ISSET() - Test to see if CPU cpu is a member of set.
                if (CPU_ISSET(inx, &totalProcessorSet)) {
                    std::cout << colors::BRIGHT_WHITE << inx << " " << colors::RESET;
                }
            } std::cout << "\n";
        }
    }

    auto setThreadAffinity(pthread_t programThread, int coreId) -> bool { // POSIX THREAD
        cpu_set_t totalProcessorSet;
        CPU_ZERO(&totalProcessorSet);
        CPU_SET(coreId, &totalProcessorSet);

        int result {pthread_setaffinity_np(
            programThread, sizeof(cpu_set_t), &totalProcessorSet
        )};
        // inference a thread to a specfic processor core but is not portable will not work on w*ndows

        if (result != 0) {
            std::cerr << colors::BRIGHT_RED << "execution(pthread_setaffinity_np) failure, R != 0: " << strerror(result) << colors::RESET << "\n";
            return false;
        }   return true;
    }

    void displayMainMenu() {
        std::cout << "\n";
        std::cout << colors::BOLD << colors::BRIGHT_MAGENTA;
        std::cout << "Some shitty C++ performance analyzer program";
        std::cout << colors::RESET << "\n";
        printSeparator(50);
        std::cout << "\n";
        std::cout << colors::DIM << "MAIN MENU" << colors::RESET << "\n\n";
        std::cout << colors::BRIGHT_CYAN << "  1." << colors::RESET << " Run standard benchmark\n";
        std::cout << colors::BRIGHT_CYAN << "  2." << colors::RESET << " Run with core isolation\n";
        std::cout << colors::BRIGHT_CYAN << "  3." << colors::RESET << " Run with thread isolation\n";
        std::cout << colors::BRIGHT_CYAN << "  4." << colors::RESET << " Configure custom benchmark\n";
        std::cout << colors::BRIGHT_CYAN << "  5." << colors::RESET << " System information\n";
        std::cout << colors::BRIGHT_CYAN << "  6." << colors::RESET << " Help & Documentation\n";
        std::cout << colors::BRIGHT_RED << "  0." << colors::RESET << " Exit\n";
        std::cout << "\n" << colors::BRIGHT_YELLOW << "  Enter numeric: " << colors::RESET;
    }

    benchmarkConfig configureInteractiveBenchmark() {
        std::string sourceFile, assemblyFile, compilerFlags;
        int totalReiterativeExecutionRuns, warmupIterations;
        char assemblyFileGenerationFlag;

        std::cout << "\n" << colors::BOLD << colors::BRIGHT_CYAN << "Benchmark Configuration" << colors::RESET << "\n";
        printSeparator(40);

        std::cout << colors::BRIGHT_YELLOW << "  Source file path: " << colors::RESET;
        std::cin >> sourceFile;

        std::cout << colors::BRIGHT_YELLOW << "  Compiler flags (default: -O2 -std=c++17): " << colors::RESET;
        std::cin.ignore();
        std::getline(std::cin, compilerFlags);
        if (compilerFlags.empty()) {
            compilerFlags = "-O2 -std=c++17";
        }

        std::cout << colors::BRIGHT_YELLOW << "  Number of runs (default: 5): " << colors::RESET;
        std::string runsInput;
        std::getline(std::cin, runsInput);
        totalReiterativeExecutionRuns = (runsInput.empty()) ? 5 : std::stoi(runsInput);

        std::cout << colors::BRIGHT_YELLOW << "  Warmup iterations (default: 2): " << colors::RESET;
        std::string warmupInput;
        std::getline(std::cin, warmupInput);
        warmupIterations = (warmupInput.empty()) ? 2 : std::stoi(warmupInput); // toe

        std::cout << colors::BRIGHT_YELLOW << "  Generate assembly output? [yY/nN]: " << colors::RESET;
        std::cin >> assemblyFileGenerationFlag;

        if (assemblyFileGenerationFlag == 'y' || assemblyFileGenerationFlag == 'Y') {
            std::cout << colors::BRIGHT_YELLOW << "  Input assembly output file name [+extension]: " << colors::RESET;
            std::cin >> assemblyFile;
        }

        return benchmarkConfig(sourceFile, assemblyFile, compilerFlags, totalReiterativeExecutionRuns, warmupIterations);
    }

    void runInteractiveCLI() {
        int volitionSym {-1}; // HES DEPENDING ON A SIGNED INTEGER!!!

        while (volitionSym != 0) { // doodoo
            displayMainMenu();
            std::cin >> volitionSym;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            switch (volitionSym) {
            case 1: {
                // standard benchmrking
                std::string sourceFile;
                std::cout << "\n" << colors::BRIGHT_YELLOW << "  Enter source file path: " << colors::RESET;
                std::getline(std::cin, sourceFile);

                benchmarkConfig config(sourceFile);
                runBenchmark(config);

                std::cout << "\n" << colors::DIM << "  [_ENTER_TO_CONTINUE_!_]" << colors::RESET;
                std::cin.get();
                break;
            }

            case 2: {
                // core isolation
                std::string sourceFile;
                int coreId;

                std::cout << "\n" << colors::BRIGHT_CYAN << "  Total amount of CPU cores: " << colors::RESET << colors::BRIGHT_WHITE << getTotalProcessorCores() << colors::RESET << "\n";
                std::cout << colors::BRIGHT_YELLOW << "  Enter source file/code path: " << colors::RESET;
                std::getline(std::cin, sourceFile);
                std::cout << colors::BRIGHT_YELLOW << "  Input core ID to pin [0-" << (getTotalProcessorCores() - 1) << "]: " << colors::RESET;
                std::cin >> coreId;

                if (coreId < 0 || coreId >= getTotalProcessorCores()) {
                    std::cerr << colors::BRIGHT_RED << "  Invalid core ID inputted!" << colors::RESET << "\n";
                } else {
                    benchmarkConfig config(sourceFile);
                    config.enableCoreIsolation = true;
                    config.targetCoreId = coreId;
                    runBenchmark(config);
                }

                std::cout << "\n" << colors::DIM << "  [ENTER TO CONTINUE!]" << colors::RESET;
                std::cin.ignore();
                std::cin.get();
                break;
            }

            case 3: {
                // thread isolation
                std::string sourceFile;
                int coreId;

                std::cout << "\n" << colors::BRIGHT_CYAN << "  Available CPU cores: " << colors::RESET << colors::BRIGHT_WHITE << getTotalProcessorCores() << colors::RESET << "\n";
                std::cout << colors::BRIGHT_YELLOW << "  Enter source file/cde path: " << colors::RESET;
                std::getline(std::cin, sourceFile);
                std::cout << colors::BRIGHT_YELLOW << "  Enter core ID for thread isolation [0-" << (getTotalProcessorCores() - 1) << "]: " << colors::RESET;
                std::cin >> coreId;

                if (coreId < 0 || coreId >= getTotalProcessorCores()) {
                    std::cerr << colors::BRIGHT_RED << "  Invalid core ID!" << colors::RESET << "\n";
                } else {
                    benchmarkConfig config(sourceFile);
                    config.enableCoreIsolation = true;
                    config.enableThreadIsolation = true;
                    config.targetCoreId = coreId;
                    runBenchmark(config);
                }

                std::cout << "\n" << colors::DIM << "  [_ENTER_TO_CONTINUE_]" << colors::RESET;
                std::cin.ignore();
                std::cin.get();
                break;
            }

            case 4: {
                // custom configuration
                auto config{configureInteractiveBenchmark()};

                char shouldEnableCoreIso, shouldEnableThreadIso;
                std::cout << "\n" << colors::BRIGHT_YELLOW << "  Enable core isolation? [yY/nN]: " << colors::RESET;
                std::cin >> shouldEnableCoreIso;

                if (shouldEnableCoreIso == 'y' || shouldEnableCoreIso == 'Y') {
                    int coreId;
                    std::cout << colors::BRIGHT_YELLOW << "  Core ID (0-" << (getTotalProcessorCores() - 1) << "): " << colors::RESET;
                    std::cin >> coreId;

                    config.enableCoreIsolation = true;
                    config.targetCoreId = coreId;

                    std::cout << colors::BRIGHT_YELLOW << "  Enable thread isolation? (y/n): " << colors::RESET;
                    std::cin >> shouldEnableThreadIso;

                    if (shouldEnableThreadIso == 'y' || shouldEnableThreadIso == 'Y') {
                        config.enableThreadIsolation = true;
                    }
                }

                runBenchmark(config);

                std::cout << "\n" << colors::DIM << "  [_ENTER_TO_CONTINUE_!_]" << colors::RESET;
                std::cin.ignore();
                std::cin.get();
                break;
            }

            case 5: {
                // sysinfo
                printHeader("SYSTEM INFORMATION");
                // THIS SYSTEM WAS A FUCKIN MISTAKE
                printMetric("Number of CPU cores", std::to_string(getTotalProcessorCores()));
                printMetric("Process ID", std::to_string(getpid()));
                printProcessorAffinityInfo();

                std::cout << "\n" << colors::DIM << "  Press Enter to continue..." << colors::RESET;
                std::cin.get();
                break;
            }

            case 6: {
                // assistance
                std::cout << "\n" << colors::BOLD << colors::BRIGHT_MAGENTA;
                std::cout << "HELP & DOCUMENTATION";
                std::cout << colors::RESET << "\n";
                printSeparator(70);
                std::cout << "\n";

                std::cout << colors::BOLD << colors::BRIGHT_CYAN << "TERMINOLOGY" << colors::RESET << "\n\n";

                std::cout << colors::BRIGHT_YELLOW << "  Core Isolation:" << colors::RESET << "\n";
                std::cout << colors::DIM << "    Pins the benchmark process to a specific CPU core using\n";
                std::cout << "    sched_setaffinity(). This reduces scheduling overhead and\n";
                std::cout << "    context switches between cores, leading to more consistent\n";
                std::cout << "    timing measurements. Prevents OS from migrating your process\n";
                std::cout << "    across different cores during execution." << colors::RESET << "\n\n";

                std::cout << colors::BRIGHT_YELLOW << "  Thread Isolation:" << colors::RESET << "\n";
                std::cout << colors::DIM << "    Isolates individual threads to specific cores using\n";
                std::cout << "    pthread_setaffinity_np() (requires core isolation enabled).\n";
                std::cout << "    Best for multi-threaded benchmarks where you want each thread\n";
                std::cout << "    running on a dedicated core without interference." << colors::RESET << "\n\n";

                std::cout << colors::BRIGHT_YELLOW << "  Percentile Stability:" << colors::RESET << "\n";
                std::cout << colors::DIM << "    Shows distribution of execution times across all runs.\n";
                std::cout << "    P25/P75 = middle 50% of measurements (interquartile range)\n";
                std::cout << "    P95/P99 = worst-case performance (tail latency)\n";
                std::cout << "    Coefficient of Variation = (std_dev / mean) * 100%\n";
                std::cout << "    Lower CoV means more consistent/stable performance." << colors::RESET << "\n\n";

                std::cout << colors::BOLD << colors::BRIGHT_CYAN << "COMMAND LINE USAGE" << colors::RESET << "\n\n";

                std::cout << colors::BRIGHT_YELLOW << "  Compilation:" << colors::RESET << "\n";
                std::cout << colors::DIM << "    g++ -std=c++17 main.cpp benchmarkAnalyzer.cpp -o main" << colors::RESET << "\n\n";

                std::cout << colors::BRIGHT_YELLOW << "  Tips for Best Results:" << colors::RESET << "\n";
                std::cout << colors::DIM << "    - Close background applications\n";
                std::cout << "    - Use core isolation on dedicated core\n";
                std::cout << "    - Run 10+ iterations for statistical significance\n";
                std::cout << "    - Use warmup iterations (2-3 minimum)\n";
                std::cout << "    - Check CV% in stability metrics (<5% is excellent (EX+))" << colors::RESET << "\n\n";

                std::cout << colors::DIM << "  [ENTER TO CONTINUE!]" << colors::RESET;
                std::cin.get();
                break;
            }

            case 0:
                std::cout << "\n" << colors::BRIGHT_GREEN << "  EXITING...!" << colors::RESET << "\n\n";
                break;

            default:
                std::cout << "\n" << colors::BRIGHT_RED << "  Invalid input choice sir." << colors::RESET << "\n";
                std::cout << colors::DIM << "  [_ENTER_TO_CONTINUE_!_]" << colors::RESET;
                std::cin.get();
            }
        }
    }
} 
