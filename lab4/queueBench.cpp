#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>

#include <lockFreeQueue.hpp>

// Producer function
void producer(threadsafe_queue<int> &sharedQueue, int id, long long numItems) {
    for (long long i = 0; i < numItems; ++i) {
        int value = id * 1000; // Unique value per thread for illustration
        sharedQueue.push(value);
        //std::cout << "Producer " << id << " pushed " << value << std::endl;
        //std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate work
    }
}

// Consumer function
void consumer(threadsafe_queue<int> &sharedQueue, int id) {

    while (id > -1) {
        int value;
        if (sharedQueue.try_pop(value)) {
            //std::cout << "Consumer " << id << " popped " << value << std::endl;
            //std::this_thread::sleep_for(std::chrono::milliseconds(150)); // Simulate work
        } else {
            break;
        }
    }
}

int main() {
    threadsafe_queue<int> sharedQueue;

    const int numThreads = 8;
    const int numProducers = numThreads / 2;
    const int numConsumers = numThreads / 2;
    const long long itemsPerProducer = 5e6;

    std::vector<std::thread> threads;

    auto init = std::chrono::steady_clock::now();

    // Launch producer threads
    for (int i = 0; i < numProducers; ++i) {
        threads.emplace_back(producer, std::ref(sharedQueue), i, itemsPerProducer);
    }

    // Launch consumer threads
    for (int i = 0; i < numConsumers; ++i) {
        threads.emplace_back(consumer, std::ref(sharedQueue), i);
    }

    // Wait for all threads to complete
    for (auto &t : threads) {
        t.join();
    }

    auto end = std::chrono::steady_clock::now();

    std::cout << "Numbers per ms: " << (numProducers * itemsPerProducer) / 
        std::chrono::duration_cast<std::chrono::milliseconds>(end - init).count() << std::endl;

    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - init).count() << " ms" << std::endl;
    return 0;
}
