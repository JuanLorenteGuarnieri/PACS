#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

// Template for a thread-safe queue
template<typename T>
class threadsafe_queue
{
private:
    mutable std::mutex mtx; // Mutex to protect shared data
    std::queue<T> data_queue; // The underlying queue to store tasks
    std::condition_variable data_cond; // Condition variable for coordination between threads

public:
    // Default constructor
    threadsafe_queue() {}

    // Copy constructor
    threadsafe_queue(const threadsafe_queue& other)
    {
        std::lock_guard<std::mutex> lock(other.mtx); // Lock the mutex of the other queue
        data_queue = other.data_queue; // Copy the queue content
    }

    // Deleted assignment operator to avoid accidental copying
    threadsafe_queue& operator=(const threadsafe_queue&) = delete;

    // Push a new value into the queue
    void push(T new_value)
    {
        std::lock_guard<std::mutex> lock(mtx); // Lock the mutex before modifying the queue
        data_queue.push(std::move(new_value)); // Push the new task into the queue
        data_cond.notify_one(); // Notify one waiting thread that new data is available
    }

    // Try to pop a value from the queue without blocking
    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
        if (data_queue.empty()) // If the queue is empty, return false
            return false;
        value = std::move(data_queue.front()); // Get the front element
        data_queue.pop(); // Remove it from the queue
        return true;
    }

    // Block until an item is available and then pop it
    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lock(mtx); // Lock the mutex
        data_cond.wait(lock, [this]{ return !data_queue.empty(); }); // Wait for data to be available
        value = std::move(data_queue.front()); // Get the front element
        data_queue.pop(); // Remove it from the queue
    }

    // Block until an item is available, then pop it and return as a shared_ptr
    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lock(mtx); // Lock the mutex
        data_cond.wait(lock, [this]{ return !data_queue.empty(); }); // Wait for data
        std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front()))); // Create shared_ptr to front element
        data_queue.pop(); // Remove the element from the queue
        return res;
    }

    // Check if the queue is empty
    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
        return data_queue.empty(); // Check if the underlying queue is empty
    }
};