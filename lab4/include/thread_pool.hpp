#pragma once

#include <atomic>
#include <functional>
#include <vector>
#include <thread>
#include "join_threads.hpp"
#include "threadsafe_queue.hpp"

class thread_pool
{
    std::atomic_bool done;                     // Flag to indicate when the pool should stop
    threadsafe_queue<std::function<void()>> work_queue; // Thread-safe queue for storing tasks
    std::vector<std::thread> threads;          // Worker threads
    join_threads _joiner;                      // Helper to join threads when the pool is destroyed

    // Worker function for each thread
    void worker_thread()
    {
        while (!done)
        {
            std::function<void()> task;
            // Try to pop a task from the queue. If no task is available, yield control to the OS.
            if (work_queue.try_pop(task)) {
                task(); // Execute the task
            } else {
                std::this_thread::yield(); // Yield if no task is available
            }
        }
    }

public:
    // Constructor initializes the pool with a specified number of threads (default is hardware concurrency)
    thread_pool(size_t num_threads = std::thread::hardware_concurrency())
        : done(false), _joiner(threads) // Initialize the done flag and joiner
    {
        try {
            // Create worker threads
            for (size_t i = 0; i < num_threads; ++i) {
                threads.emplace_back(&thread_pool::worker_thread, this);
            }
        }
        catch (...) {
            done = true;
            throw;
        }
    }

    // Destructor calls wait and stops the threads
    ~thread_pool()
    {
        done = true;  // Signal all threads to finish
        // wait();       // Ensure all tasks are completed
    }

    // Wait for all tasks to complete (active waiting)
    void wait()
    {
        while (!work_queue.empty()) {
            std::this_thread::yield(); // Active wait by yielding until the queue is empty
        }
    }

    // Submit a new task to the pool
    template<typename F>
    void submit(F f)
    {
        work_queue.push(std::function<void()>(f)); // Push the task into the queue
    }
};