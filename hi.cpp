#include <iostream>
#include <vector>
#include <algorithm>
#include <sys/resource.h>
#include <unistd.h>   // getpid()

using namespace std;

// Bubble sort
void bubbleSort(vector<int>& arr) {
    size_t n = arr.size();
    for (size_t i = 0; i < n - 1; ++i) {
        for (size_t j = 0; j < n - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }

            // Kernel mode work (system call)
            getpid();
        }
    }
}

// Convert timeval to seconds
double toSeconds(const timeval& tv) {
    return tv.tv_sec + tv.tv_usec / 1e6;
}

int main() {
    const size_t N = 8000;   // Start size (large on purpose)
    vector<int> arr(N);

    for (size_t i = 0; i < N; ++i)
        arr[i] = N - i;

    rusage start{}, end{};

    getrusage(RUSAGE_SELF, &start);

    // Repeat until time targets are reached
    do {
        bubbleSort(arr);
        reverse(arr.begin(), arr.end()); // keep it unsorted
        getrusage(RUSAGE_SELF, &end);
    } while (
        (toSeconds(end.ru_utime) - toSeconds(start.ru_utime) < 1.2) ||
        (toSeconds(end.ru_stime) - toSeconds(start.ru_stime) < 1.2)
    );

    cout << "User CPU time:   "
         << toSeconds(end.ru_utime) - toSeconds(start.ru_utime)
         << " seconds\n";

    cout << "Kernel CPU time: "
         << toSeconds(end.ru_stime) - toSeconds(start.ru_stime)
         << " seconds\n";

    return 0;
}
