#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <string>
#include <thread>
#include <utility>
#include <vector>

using my_float = float; // Changed to float to enhance Kahan's advantage


// Kahan summation algorithm
void KahanSum(const std::vector<my_float>& input, my_float& sum, my_float& c) {
    for (size_t i = 0; i < input.size(); ++i) {
        my_float y = input[i] - c;      // Compensate for lost low-order bits
        my_float t = sum + y;           // Perform the summation
        c = (t - sum) - y;              // Calculate the lost low-order bits
        sum = t;                        // Update the sum
    }
}

// Function to compute π using a portion of the Taylor series
void pi_taylor_chunk(size_t thread_id, 
                     size_t start_step, 
                     size_t stop_step, 
                     my_float& result) {
    auto start = std::chrono::high_resolution_clock::now(); // Start the timer
    my_float sum = 0.0; // Local sum for this thread
    my_float c = 0.0;   // Compensation for lost low-order bits

    for (size_t i = start_step; i < stop_step; ++i) {
        my_float term = 1.0 / (2.0 * i + 1.0) * (i % 2 == 0 ? 1.0 : -1.0); // Taylor series term
        my_float y = term - c;     // Compensate for lost low-order bits
        my_float t = sum + y;      // Perform the summation
        c = (t - sum) - y;         // Calculate the lost low-order bits
        sum = t;                   // Update the sum
    }

    result = sum; // Store the result for this thread
    
    auto end = std::chrono::high_resolution_clock::now();  // End the timer
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Thread " << thread_id << " executed in: " << elapsed.count() << " seconds\n";
}


// void pi_taylor_chunk2(size_t thread_id, size_t start_step, size_t stop_step, my_float& result) {
//     my_float partial_sum = 0.0;
//     my_float sign = (start_step % 2 == 0) ? 1.0 : -1.0; // Alternate sign based on step
//     for (size_t i = start_step; i < stop_step; ++i) {
//         partial_sum += sign / (2.0 * i + 1.0);
//         sign = -sign; // Alternate the sign for each term
//     }
//     result = partial_sum; // Store the result in the thread's output slot
// }

std::pair<size_t, size_t> usage(int argc, const char* argv[]) {
    // Read the number of steps and threads from the command line
    if (argc != 3) {
        std::cerr << "Invalid syntax: pi_taylor <steps> <threads>" << std::endl;
        exit(1);
    }

    size_t steps = std::stoll(argv[1]);
    size_t threads = std::stoll(argv[2]);

    if (steps < threads) {
        std::cerr << "The number of steps should be larger than the number of threads" << std::endl;
        exit(1);
    }
    return std::make_pair(steps, threads);
}

int main(int argc, const char* argv[]) {
    auto ret_pair = usage(argc, argv);
    auto steps = ret_pair.first;
    auto threads = ret_pair.second;

    std::vector<std::thread> thread_pool;
    std::vector<my_float> results(threads, 0.0);  // An array to store partial results

    size_t chunk_size = steps / threads;

    // Start threads to compute partial sums
    for (size_t t = 0; t < threads; ++t) {
        size_t start = t * chunk_size;
        size_t stop = (t == threads - 1) ? steps : start + chunk_size;
        
        thread_pool.emplace_back(pi_taylor_chunk, t, start, stop, std::ref(results[t]));
    }

    // Wait for all threads to finish
    for (auto& t : thread_pool) {
        t.join();
    }

    // Final summation using Kahan to combine results from all threads
    my_float pi = 0.0;
    my_float c = 0.0;

    KahanSum(results, pi, c); // Combine the results with Kahan summation

    pi *= 4.0;  // Multiply by 4 to obtain π

    std::cout << "For " << steps << " steps and " << threads << " threads, pi value: "
              << std::setprecision(30)
              << pi << std::endl;

    return 0;
}