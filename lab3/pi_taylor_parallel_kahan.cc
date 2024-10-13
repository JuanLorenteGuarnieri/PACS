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
void pi_taylor_chunk(std::vector<my_float> &output, 
                           size_t thread_id, 
                           size_t start_step, 
                           size_t stop_step) {
    auto start = std::chrono::high_resolution_clock::now(); // Start the timer
    
    // Rest of the implementation
    my_float sum = 0.0;
    my_float c = 0.0;
    my_float sign = (start_step % 2 == 0) ? 1.0 : -1.0;
    
    for (size_t i = start_step; i < stop_step; ++i) {
        my_float y = sign / (2.0 * i + 1.0) - c;
        my_float t = sum + y;
        c = (t - sum) - y;
        sum = t;
        sign = -sign;
    }
    output[thread_id] = sum;

    auto end = std::chrono::high_resolution_clock::now();  // End the timer
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Thread " << thread_id << " executed in: " << elapsed.count() << " seconds\n";
}


std::pair<size_t, size_t>
usage(int argc, const char *argv[]) {
    // Read the number of steps from the command line
    if (argc != 3) {
        std::cerr << "Invalid syntax: pi_taylor <steps> <threads>" << std::endl;
        exit(1);
    }

    size_t steps = std::stoll(argv[1]);
    size_t threads = std::stoll(argv[2]);

    if (steps < threads ){
        std::cerr << "The number of steps should be larger than the number of threads" << std::endl;
        exit(1);
    }
    return std::make_pair(steps, threads);
}

int main(int argc, const char *argv[]) {
    auto ret_pair = usage(argc, argv);
    auto steps = ret_pair.first;
    auto threads = ret_pair.second;

    std::vector<std::thread> thread_pool;
    std::vector<my_float> results(threads, 0.0);  // An array to store partial results

    size_t chunk_size = steps / threads;

    for (size_t t = 0; t < threads; ++t) {
        size_t start = t * chunk_size;
        size_t stop = (t == threads - 1) ? steps : start + chunk_size;
        
        thread_pool.emplace_back(pi_taylor_chunk, std::ref(results), t, start, stop);
    }

    // Wait for all threads to finish
    for (auto &t : thread_pool) {
        t.join();
    }

    // Final summation using Kahan to combine results from all threads
    my_float pi = 0.0;
    my_float c = 0.0;
    
    for (auto &partial_sum : results) {
        my_float y = partial_sum - c;
        my_float t = pi + y;
        c = (t - pi) - y;
        pi = t;
    }

    pi *= 4.0;  // Multiply by 4 to obtain π

    std::cout << "For " << steps << " steps and " << threads << " threads, pi value: "
              << std::setprecision(std::numeric_limits<my_float>::digits10 + 1)
              << pi << std::endl;
}
