#include <iostream>
#include <vector>
#include <cstdlib>  // For rand()
#include <chrono>   // For measuring time
#include <cmath>    // For sqrt() and pow()

// Function to multiply two matrices
void matrixMultiply(const std::vector<std::vector<double>>& A,
                    const std::vector<std::vector<double>>& B,
                    std::vector<std::vector<double>>& C) {
    size_t N = A.size();
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            C[i][j] = 0;
            for (size_t k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Function to generate a random matrix
void generateRandomMatrix(std::vector<std::vector<double>>& matrix, size_t N) {
    matrix.resize(N, std::vector<double>(N));
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            matrix[i][j] = static_cast<double>(rand()) / RAND_MAX; // Random value between 0 and 1
        }
    }
}

// Function to calculate the standard deviation
double calculateStandardDeviation(const std::vector<double>& times, double mean) {
    double sum = 0.0;
    for (double time : times) {
        sum += std::pow(time - mean, 2);
    }
    return std::sqrt(sum / times.size());
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <matrix_size> <num_repetitions>" << std::endl;
        return 1;
    }

    size_t N = std::stoul(argv[1]);  // Matrix size
    size_t M = std::stoul(argv[2]);  // Number of repetitions

    std::vector<std::vector<double>> A, B, C;

    // Variable to accumulate total duration
    std::vector<double> durations;
    double totalDuration = 0.0;

    // Run matrix multiplication m times and calculate average time
    for (size_t i = 0; i < M; ++i) {
        // Generate random matrices
        generateRandomMatrix(A, N);
        generateRandomMatrix(B, N);
        C.resize(N, std::vector<double>(N));

        // Measure time for this iteration
        auto start = std::chrono::high_resolution_clock::now(); // Start time

        matrixMultiply(A, B, C); // Perform matrix multiplication

        auto end = std::chrono::high_resolution_clock::now(); // End time
        std::chrono::duration<double> duration = end - start; // Duration

        // Accumulate total duration
        durations.push_back(duration.count());
        totalDuration += duration.count();
    }

    // Calculate average duration
    double averageDuration = totalDuration / M;

    // Calculate standard deviation of the durations
    double stddev = calculateStandardDeviation(durations, averageDuration);

    std::cout << "Total execution time over: " << totalDuration << " seconds" << std::endl;
    std::cout << "Average execution time over " << M << " runs: " << averageDuration << " seconds" << std::endl;
    std::cout << "Standard deviation of execution times: " << stddev << " seconds" << std::endl;

    return 0;
}
