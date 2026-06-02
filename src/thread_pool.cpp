#include "thread_pool.hpp"

ThreadPool::ThreadPool(std::size_t thread_count) {
    if (thread_count == 0) {
        thread_count = 1;
    }

    workers_.reserve(thread_count);
    for (std::size_t index = 0; index < thread_count; ++index) {
        workers_.emplace_back([this]() { worker_loop(); });
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::wait_for_idle() {
    std::unique_lock<std::mutex> lock(mutex_);
    idle_cv_.wait(lock, [this]() {
        return tasks_.empty() && active_workers_ == 0;
    });
}

void ThreadPool::shutdown() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (stop_requested_) {
            return;
        }

        stop_requested_ = true;
    }

    work_ready_.notify_all();

    for (std::thread& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

std::size_t ThreadPool::size() const noexcept {
    return workers_.size();
}

bool ThreadPool::is_stopped() const noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    return stop_requested_;
}

void ThreadPool::worker_loop() {
    for (;;) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(mutex_);
            work_ready_.wait(lock, [this]() {
                return stop_requested_ || !tasks_.empty();
            });

            if (stop_requested_ && tasks_.empty()) {
                return;
            }

            task = std::move(tasks_.front());
            tasks_.pop();
            ++active_workers_;
        }

        task();

        {
            std::lock_guard<std::mutex> lock(mutex_);
            --active_workers_;
            if (tasks_.empty() && active_workers_ == 0) {
                idle_cv_.notify_all();
            }
        }
    }
}
