#include "thread_pool.hpp"

#include "defines.hpp"
#include <spdlog/spdlog.h>

ThreadPool::ThreadPool() {
    for (std::size_t i = 0; i < NUMBER_OF_THREADS; ++i) {
        workers_.emplace_back([&]() {
            while (true) {
                auto task = tasks_.pop();
                if (!task) {
                    break;
                }

                (*task)();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    tasks_.stop();
    
    for (auto& th : workers_) {
        th.join();
    }
}
