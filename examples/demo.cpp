#include "thread_pool.hpp"

#include <chrono>
#include <iostream>
#include <numeric>
#include <vector>

int main() {
    ThreadPool pool(4);
    std::vector<std::future<int>> futures;

    for (int value = 1; value <= 8; ++value) {
        futures.push_back(pool.submit([value]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
            return value * value;
        }));
    }

    int sum = 0;
    for (std::future<int>& future : futures) {
        sum += future.get();
    }

    pool.wait_for_idle();

    std::cout << "Computed 8 squares across " << pool.size()
              << " workers. Sum = " << sum << '\n';

    return 0;
}
