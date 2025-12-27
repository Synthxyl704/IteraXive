#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

#ifndef INPUT_SIZE
#define INPUT_SIZE 100'000          // 100 k elements → ~20-30 ms
#endif
#include <random>

volatile int sink = 0;              // prevent the sort from being optimised away

int main() {
    std::vector<int32_t> arr = {293, 342, 54, 434, 411};

    size_t v_ctr_x {arr.size()};
    while (v_ctr_x --> 0) {
        printf("%d | ", arr[v_ctr_x]);
    }

    return 0;
}