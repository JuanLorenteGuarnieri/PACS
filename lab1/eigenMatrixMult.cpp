#include <iostream>
#include <Eigen/Dense> // Ensure Eigen is correctly included
#include <chrono> // For measuring time
#include <cmath>    // For sqrt() and pow()
#include <vector>   // For std::vector

// Function to calculate the standard deviation
double calculateStandardDeviation(const std::vector<double>& times, double mean) {
    double sum = 0.0;
    for (double time : times) {
        sum += std::pow(time - mean, 2);
    }
    return std::sqrt(sum / times.size());
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <N> <M> <k>" << std::endl;
        return 1;
    }

    size_t N = std::stoul(argv[1]);  // Matrix size
    size_t M = std::stoul(argv[2]);  // Number of repetitions
    size_t k = std::stoul(argv[3]);  // Number of repetitions

    // Variable to accumulate total duration
    std::vector<double> durations;
    double totalDuration = 0.0;

    for (size_t i = 0; i < k; ++i) {
        // Using Eigen's MatrixXd for dynamic size matrices
        Eigen::MatrixXd A = Eigen::MatrixXd::Random(N, M); // Matrix A with random values
        Eigen::MatrixXd B = Eigen::MatrixXd::Random(M, N); // Matrix B with random values

        auto start = std::chrono::high_resolution_clock::now(); // Start time

        Eigen::MatrixXd C = A * B; // Matrix multiplication using Eigen

        auto end = std::chrono::high_resolution_clock::now(); // End time
        std::chrono::duration<double> duration = end - start; // Duration

        // Accumulate total duration
        durations.push_back(duration.count());
        totalDuration += duration.count();
    }

    // Calculate average duration
    double averageDuration = totalDuration / k;

    // Calculate standard deviation of the durations
    double stddev = calculateStandardDeviation(durations, averageDuration);

    std::cout << "Total execution time over: " << totalDuration << " seconds" << std::endl;
    std::cout << "Average execution time over " << k << " runs: " << averageDuration << " seconds" << std::endl;
    std::cout << "Standard deviation of execution times: " << stddev << " seconds" << std::endl;

    return 0;
}
