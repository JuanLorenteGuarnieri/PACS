#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <string>
#include <thread>
#include <utility>
#include <vector>

using my_float = long double;

// Function to compute a chunk of the Taylor series for π
void pi_taylor_chunk(std::vector<my_float> &output, size_t thread_id, size_t start_step, size_t stop_step) {
    my_float partial_sum = 0.0;
    my_float sign = (start_step % 2 == 0) ? 1.0 : -1.0; // Alternate sign based on step
    for (size_t i = start_step; i < stop_step; ++i) {
        partial_sum += sign / (2.0 * i + 1.0);
        sign = -sign; // Alternate the sign for each term
    }
    output[thread_id] = partial_sum; // Store the result in the thread's output slot
}

// Function to handle command line arguments
std::pair<size_t, size_t> usage(int argc, const char *argv[]) {
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

int main(int argc, const char *argv[]) {
    auto ret_pair = usage(argc, argv);
    auto steps = ret_pair.first;
    auto threads = ret_pair.second;

    // Vector to store partial sums computed by each thread
    std::vector<my_float> partial_sums(threads, 0.0);

    // Create threads and assign each a chunk of work
    std::vector<std::thread> thread_pool;
    size_t chunk_size = steps / threads;
    size_t remaining_steps = steps % threads; // For any extra steps that don't divide evenly

    for (size_t i = 0; i < threads; ++i) {
        size_t start_step = i * chunk_size;
        size_t stop_step = start_step + chunk_size;
        if (i == threads - 1) stop_step += remaining_steps; // Last thread takes any remaining steps

        thread_pool.emplace_back(pi_taylor_chunk, std::ref(partial_sums), i, start_step, stop_step);
    }

    // Wait for all threads to finish
    for (auto &t : thread_pool) {
        t.join();
    }

    // Perform the reduction (sum all partial results)
    my_float pi = 4.0 * std::accumulate(partial_sums.begin(), partial_sums.end(), my_float(0.0));

    // Output the result
    std::cout << "For " << steps << " steps, pi value: "
              << std::setprecision(std::numeric_limits<my_float>::digits10 + 1)
              << pi << std::endl;
}
