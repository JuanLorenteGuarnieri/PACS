#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <vector>
#include <thread>
#include <future>

class thread_pool_std
{
    std::vector<std::future<void>> futures;
public:
    // Constructor initializes the pool with a specified number of threads (default is hardware concurrency)
    thread_pool_std() {}

    // Destructor calls wait and stops the threads
    ~thread_pool_std()
    {
        wait();       // Ensure all tasks are completed
    }

    // Wait for all tasks to complete (active waiting)
    void wait()
    {
        futures.clear();
    }

    // Submit a new task to the pool
    template<typename F>
    void submit(F f)
    {
        futures.push_back(std::async(std::launch::async, f));
    }
};