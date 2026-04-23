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
    tasks_.enqueue(std::forward<Func>(func));
}
