#pragma once
#include <mutex>
#include <queue>
#include <optional>
#include <condition_variable>
#include <spdlog/spdlog.h>


template <typename T>
class ThreadSafeQueue {
    mutable std::mutex mtx_;
    mutable std::condition_variable cv_;
    std::queue<T> queue_;
    bool stop_ = false;

public: 
    ThreadSafeQueue() = default;
    ~ThreadSafeQueue();

    ThreadSafeQueue(const ThreadSafeQueue& other) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue& other) = delete;
    
    ThreadSafeQueue(ThreadSafeQueue&& other);
    ThreadSafeQueue& operator=(ThreadSafeQueue&& other);

    template <typename U>
    void enqueue(U&& value);
    std::optional<T> pop();

    void stop();

    bool empty() const;
    size_t size() const;
};

template <typename T>
ThreadSafeQueue<T>::~ThreadSafeQueue() {
    stop();
}

template <typename T>
ThreadSafeQueue<T>::ThreadSafeQueue(ThreadSafeQueue&& other) {
    mtx_ = std::move(other.mtx_);
    queue_ = std::move(other.queue_);
    cv_ = std::move(other.cv_);
    stop_ = other.stop_;

    other.queue_.empty();
    other.stop();
}

template <typename T>
ThreadSafeQueue<T>& ThreadSafeQueue<T>::operator=(ThreadSafeQueue&& other) {
    if (this != &other) {
        this->stop();

        mtx_ = std::move(other.mtx_);
        queue_ = std::move(other.queue_);
        cv_ = std::move(other.cv_);
        stop_ = other.stop_;

        other.stop();
    }
    return *this;
}

template <typename T>
std::optional<T> ThreadSafeQueue<T>::pop() {
    std::unique_lock lock(mtx_);

    cv_.wait(lock, [&](){
        return !queue_.empty() || stop_;
    });

    return std::optional<T>();
}

template <typename T>
void ThreadSafeQueue<T>::stop()
{
    {
        std::scoped_lock lock(mtx_);
        stop_ = true;
    }
    cv_.notify_all();
}

template <typename T>
bool ThreadSafeQueue<T>::empty() const
{
    std::scoped_lock lock(mtx_);
    return queue_.empty();
}

template <typename T>
size_t ThreadSafeQueue<T>::size() const {
    std::scoped_lock lock(mtx_);
    return queue_.size();
}

template <typename T>
template <typename U>
void ThreadSafeQueue<T>::enqueue(U&& value) {
    {
        std::scoped_lock lock(mtx_);
        if (stop_) throw std::runtime_error("Queue stopped");

        queue_.push(std::forward<U>(value));
    }
    cv_.notify_one();
}
