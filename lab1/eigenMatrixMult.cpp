#include <iostream>
#include <Eigen/Dense> // Ensure Eigen is correctly included
#include <chrono> // For measuring time

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <matrix_size> <num_repetitions>" << std::endl;
        return 1;
    }

    size_t N = std::stoul(argv[1]);  // Matrix size
    size_t m = std::stoul(argv[2]);  // Number of repetitions

    // Variable to accumulate total duration
    double totalDuration = 0.0;

    for (size_t i = 0; i < m; ++i) {
        // Using Eigen's MatrixXd for dynamic size matrices
        Eigen::MatrixXd A = Eigen::MatrixXd::Random(N, N); // Matrix A with random values
        Eigen::MatrixXd B = Eigen::MatrixXd::Random(N, N); // Matrix B with random values

        auto start = std::chrono::high_resolution_clock::now(); // Start time

        Eigen::MatrixXd C = A * B; // Matrix multiplication using Eigen

        auto end = std::chrono::high_resolution_clock::now(); // End time
        std::chrono::duration<double> duration = end - start; // Duration

        // Accumulate total duration
        totalDuration += duration.count();
    }

    // Calculate average duration
    double averageDuration = totalDuration / m;
    std::cout << "Average execution time over " << m << " runs: " << averageDuration << " seconds" << std::endl;

    return 0;
}
