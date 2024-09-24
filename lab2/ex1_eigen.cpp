#include <iostream>
#include <Eigen/Dense> // Ensure Eigen is correctly included
#include <ctime>      // For measuring time
#include <cmath>      // For sqrt() and pow()
#include <vector>     // For std::vector

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

    size_t N = std::stoul(argv[1]);  // Number of rows in the resulting matrix
    size_t M = std::stoul(argv[2]);  // Number of columns in the first matrix (and rows in the second)
    size_t k = std::stoul(argv[3]);  // Number of repetitions

    // Variable to accumulate total duration
    std::vector<double> durations;

    for (size_t i = 0; i < k; ++i) {
        // Start time measurement
        clock_t start = clock();

        // Using Eigen's MatrixXd for dynamic size matrices
        Eigen::MatrixXd A = Eigen::MatrixXd::Random(N, M); // Matrix A with random values
        Eigen::MatrixXd B = Eigen::MatrixXd::Random(M, N); // Matrix B with random values

        // Perform matrix multiplication using Eigen
        Eigen::MatrixXd C = A * B;

        // End time measurement
        clock_t end = clock();
        
        // Calculate elapsed time
        double duration = static_cast<double>(end - start) / CLOCKS_PER_SEC;
        durations.push_back(duration);
    }

    // Calculate average duration
    double totalDuration = 0.0;
    for (const double& time : durations) {
        totalDuration += time;
    }
    double averageDuration = totalDuration / k;

    // Calculate standard deviation of the durations
    double stddev = calculateStandardDeviation(durations, averageDuration);

    std::cout << "Total execution time over: " << totalDuration << " seconds" << std::endl;
    std::cout << "Average execution time over " << k << " runs: " << averageDuration << " seconds" << std::endl;
    std::cout << "Standard deviation of execution times: " << stddev << " seconds" << std::endl;

    return 0;
}
