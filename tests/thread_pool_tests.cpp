#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <future>
#include <numeric>
#include <thread>
#include <barrier>
#include "thread_pool/thread_pool.hpp"
#include "thread_pool/thread_safe_queue.hpp"
#include "defines.hpp"

using namespace std::chrono_literals;

// ────────────────────────────────────────────────────────────
//  ThreadSafeQueue
// ────────────────────────────────────────────────────────────

TEST(ThreadSafeQueueTest, InitiallyEmptyAndSizeZero) {
    ThreadSafeQueue<int> q;
    
    EXPECT_TRUE(q.empty());
    EXPECT_EQ(q.size(), 0);
}

TEST(ThreadSafeQueueTest, EnqueueIncreasesSize) {
    ThreadSafeQueue<int> q;

    q.enqueue(1);
    q.enqueue(2);

    EXPECT_EQ(q.size(), 2);
    EXPECT_FALSE(q.empty());
}

TEST(ThreadSafeQueueTest, PopFifoPrinciple) {
    ThreadSafeQueue<int> q;
    q.enqueue(10);
    q.enqueue(20);

    auto first = q.pop();
    ASSERT_TRUE(first.has_value());
    EXPECT_EQ(*first, 10);

    auto second = q.pop();
    ASSERT_TRUE(second.has_value());
    EXPECT_EQ(*second, 20);
}

TEST(ThreadSafeQueueTest, PopDecreasesSize) {
    ThreadSafeQueue<int> q;
    q.enqueue(1);

    q.pop();

    EXPECT_TRUE(q.empty());
    EXPECT_EQ(q.size(), 0);
}

TEST(ThreadSafeQueueTest, PopReturnsNulloptAfterStop) {
    ThreadSafeQueue<int> q;
    
    q.stop();

    auto val = q.pop();
    EXPECT_FALSE(val.has_value());
}

TEST(ThreadSafeQueueTest, EnqueueAfterStopThrowsRuntimeError) {
    ThreadSafeQueue<int> q;

    q.stop();

    EXPECT_THROW(q.enqueue(1), std::runtime_error);
}

TEST(ThreadSafeQueueTest, StopIsIdempotent) {
    ThreadSafeQueue<int> q;

    EXPECT_NO_THROW({
        q.stop();
        q.stop();
    });
}

TEST(ThreadSafeQueueTest, PopBlocksUntilItemEnqueued) {
    // Arrange
    ThreadSafeQueue<int> q;
    std::promise<int> promise;
    auto future = promise.get_future();

    std::thread consumer([&] {
        auto val = q.pop();
        if (val) promise.set_value(*val);
    });

    // Act
    std::this_thread::sleep_for(15ms);
    q.enqueue(42);

    // Assert
    ASSERT_EQ(future.wait_for(2s), std::future_status::ready);
    EXPECT_EQ(future.get(), 42);
    consumer.join();
}

TEST(ThreadSafeQueueTest, StopUnblocksWaitingConsumers) {
    ThreadSafeQueue<int> q;
    std::atomic<int> unblocked{0};
    const int consumers = 4;
    std::vector<std::thread> threads;

    // Arrange
    for (int i = 0; i < consumers; ++i) {
        threads.emplace_back([&] {
            auto val = q.pop();
            if (!val) ++unblocked;
        });
    }

    // Act
    std::this_thread::sleep_for(15ms);
    q.stop();

    // Assert
    for (auto& t : threads) t.join();
    EXPECT_EQ(unblocked.load(), consumers);
}

TEST(ThreadSafeQueueTest, MoveConstructorTransfersItems) {
    ThreadSafeQueue<int> q1;
    q1.enqueue(7);
    
    ThreadSafeQueue<int> q2(std::move(q1));
    auto val = q2.pop();

    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 7);
}

TEST(ThreadSafeQueueTest, MoveAssignmentTransfersItems) {
    ThreadSafeQueue<int> q1;
    q1.enqueue(99);

    ThreadSafeQueue<int> q2;
    q2 = std::move(q1);
    auto val = q2.pop();

    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 99);
}

TEST(ThreadSafeQueueTest, ConcurrentProducersAndConsumers) {
    // Arrange
    ThreadSafeQueue<int> q;
    const int N = 200;
    std::atomic<int> sum{0};
    int expectedSum = N * (N + 1) / 2;

    std::thread producer([&] {
        for (int i = 1; i <= N; ++i) q.enqueue(i);
    });

    // Act
    std::thread consumer([&] {
        for (int i = 0; i < N; ++i) {
            auto val = q.pop();
            if (val) sum += *val;
        }
    });

    producer.join();
    consumer.join();

    // Assert
    EXPECT_EQ(sum.load(), expectedSum);
}

TEST(ThreadSafeQueueTest, MultipleProducersConcurrently) {
    ThreadSafeQueue<int> q;
    const int producerCount = 4;
    const int itemsPerProducer = 25;
    std::vector<std::thread> producers;

    for (int p = 0; p < producerCount; ++p) {
        producers.emplace_back([&] {
            for (int i = 0; i < itemsPerProducer; ++i) q.enqueue(1);
        });
    }
    for (auto& t : producers) t.join();

    EXPECT_EQ(q.size(), producerCount * itemsPerProducer);
}

// ────────────────────────────────────────────────────────────
//  ThreadPool
// ────────────────────────────────────────────────────────────

TEST(ThreadPoolTest, SingleTaskExecutes) {
    std::promise<bool> promise;
    auto future = promise.get_future();

    {
        ThreadPool pool;
        pool.enqueue([&] { promise.set_value(true); });
        ASSERT_EQ(future.wait_for(2s), std::future_status::ready);
    }

    EXPECT_TRUE(future.get());
}

TEST(ThreadPoolTest, AllEnqueuedTasksExecute) {
    // Arrange
    const int N = 20;
    std::atomic<int> counter{0};
    std::vector<std::promise<void>> promises(N);
    std::vector<std::future<void>> futures;
    for (auto& p : promises) futures.push_back(p.get_future());

    // Act
    {
        ThreadPool pool;
        for (int i = 0; i < N; ++i) {
            pool.enqueue([&counter, &promises, i] {
                ++counter;
                promises[i].set_value();
            });
        }
        for (auto& f : futures)
            ASSERT_EQ(f.wait_for(3s), std::future_status::ready);
    }

    // Assert
    EXPECT_EQ(counter.load(), N);
}

TEST(ThreadPoolTest, TasksRunInParallel) {
    if (NUMBER_OF_THREADS < 2) GTEST_SKIP() << "Requires NUMBER_OF_THREADS >= 2";

    // Arrange
    const int N = NUMBER_OF_THREADS;
    std::atomic<int> running{0};
    std::atomic<int> peakThreadsConcurrency{0};
    std::barrier<> startBarrier(N);
    
    std::vector<std::promise<void>> promises(N);
    std::vector<std::future<void>> futures;
    for (auto& p : promises) futures.push_back(p.get_future());

    // Act
    {
        ThreadPool pool;
        for (int i = 0; i < N; ++i) {
            pool.enqueue([&, i] {
                int curRunningThreadsNumber = ++running;

                // Compare and swap (CAS)
                int expected = peakThreadsConcurrency.load();
                while (curRunningThreadsNumber > expected && 
                    !peakThreadsConcurrency.compare_exchange_weak(
                        expected, curRunningThreadsNumber
                    )
                );

                std::this_thread::sleep_for(15ms);
                
                --running;
                promises[i].set_value();
            });
        }
        for (auto& f : futures)
            ASSERT_EQ(f.wait_for(5s), std::future_status::ready);
    }

    // Assert
    EXPECT_GT(peakThreadsConcurrency.load(), 1);
}

TEST(ThreadPoolTest, DestructorWaitsForPendingTasks) {
    // Arrange
    const int N = 10;
    std::atomic<int> completed{0};

    // Act
    {
        ThreadPool pool;
        for (int i = 0; i < N; ++i) {
            pool.enqueue([&] {
                std::this_thread::sleep_for(10ms);
                ++completed;
            });
        }
    } // destructor must block until all tasks finish

    // Assert
    EXPECT_EQ(completed.load(), N);
}

TEST(ThreadPoolTest, LargeNumberOfTasksAllComplete) {
    const int N = 1000;
    std::atomic<int> counter{0};

    {
        ThreadPool pool;
        for (int i = 0; i < N; ++i)
            pool.enqueue([&] { ++counter; });
    }

    EXPECT_EQ(counter.load(), N);
}

TEST(ThreadPoolTest, SharedMutableStateIsUpdatedCorrectly) {
    // Arrange
    const int N = 100;
    std::atomic<int> sum{0};

    // Act
    {
        ThreadPool pool;
        for (int i = 1; i <= N; ++i)
            pool.enqueue([&sum, i] { sum += i; });
    }

    // Assert
    EXPECT_EQ(sum.load(), N * (N + 1) / 2);
}

TEST(ThreadPoolTest, TasksWithCapturedMoveOnlyObjects) {
    std::promise<int> promise;
    auto future = promise.get_future();
    auto uniqueVal = std::make_unique<int>(77);

    {
        ThreadPool pool;
        pool.enqueue([p = std::move(uniqueVal), &promise] {
            promise.set_value(*p);
        });
        ASSERT_EQ(future.wait_for(2s), std::future_status::ready);
    }

    EXPECT_EQ(future.get(), 77);
}

TEST(ThreadPoolTest, MultiplePoolInstancesAreIndependent) {
    std::atomic<int> counter1{0}, counter2{0};

    {
        ThreadPool pool1;
        ThreadPool pool2;
        pool1.enqueue([&] { ++counter1; });
        pool2.enqueue([&] { ++counter2; });
    }

    EXPECT_EQ(counter1.load(), 1);
    EXPECT_EQ(counter2.load(), 1);
}