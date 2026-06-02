#include "thread_pool.hpp"

#include <atomic>
#include <cassert>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

namespace {

void test_submit_returns_results() {
    ThreadPool pool(3);
    auto a = pool.submit([]() { return 21; });
    auto b = pool.submit([](int lhs, int rhs) { return lhs + rhs; }, 7, 8);

    assert(a.get() == 21);
    assert(b.get() == 15);
    pool.wait_for_idle();
}

void test_all_tasks_run() {
    ThreadPool pool(4);
    std::atomic<int> counter = 0;
    std::vector<std::future<void>> futures;

    for (int i = 0; i < 32; ++i) {
        futures.push_back(pool.submit([&counter]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            counter.fetch_add(1, std::memory_order_relaxed);
        }));
    }

    for (auto& future : futures) {
        future.get();
    }

    pool.wait_for_idle();
    assert(counter.load(std::memory_order_relaxed) == 32);
}

void test_shutdown_blocks_new_work() {
    ThreadPool pool(2);
    pool.shutdown();
    assert(pool.is_stopped());

    bool threw = false;
    try {
        auto future = pool.submit([]() { return 1; });
        (void)future;
    } catch (const std::runtime_error&) {
        threw = true;
    }

    assert(threw);
}

}  // namespace

int main() {
    test_submit_returns_results();
    test_all_tasks_run();
    test_shutdown_blocks_new_work();

    std::cout << "All thread pool tests passed.\n";
    return 0;
}
