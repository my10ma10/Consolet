#pragma once

#include <functional>
#include <vector>
#include <thread>

#include "thread_safe_queue.hpp"

class ThreadPool {
    ThreadSafeQueue<std::function<void()>> tasks_;
    std::vector<std::thread> workers_;

public:
    ThreadPool();
    ~ThreadPool();

    template <typename Func>
    void enqueue(Func&& func);
};

template <typename Func>
void ThreadPool::enqueue(Func&& func) {
    auto shared_func = std::make_shared<std::decay_t<Func>>(std::forward<Func>(func));
    tasks_.enqueue([shared_func]() { (*shared_func)(); });
}
