#pragma once

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

class ThreadPool {
public:
    explicit ThreadPool(std::size_t thread_count = std::thread::hardware_concurrency());
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    template <class Fn, class... Args>
    auto submit(Fn&& fn, Args&&... args)
        -> std::future<std::invoke_result_t<Fn, Args...>>;

    void wait_for_idle();
    void shutdown();

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool is_stopped() const noexcept;

private:
    void worker_loop();

    mutable std::mutex mutex_;
    std::condition_variable work_ready_;
    std::condition_variable idle_cv_;
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::size_t active_workers_ = 0;
    bool stop_requested_ = false;
};

template <class Fn, class... Args>
auto ThreadPool::submit(Fn&& fn, Args&&... args)
    -> std::future<std::invoke_result_t<Fn, Args...>> {
    using Result = std::invoke_result_t<Fn, Args...>;

    auto task = std::make_shared<std::packaged_task<Result()>>(
        std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));
    std::future<Result> future = task->get_future();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (stop_requested_) {
            throw std::runtime_error("cannot submit task to a stopped ThreadPool");
        }

        tasks_.emplace([task]() { (*task)(); });
    }

    work_ready_.notify_one();
    return future;
}
