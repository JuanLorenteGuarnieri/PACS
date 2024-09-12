#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>

// Function to generate a random matrix with size rows x cols
std::vector<std::vector<double>> generateMatrix(int rows, int cols) {
    std::vector<std::vector<double>> matrix(rows, std::vector<double>(cols));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 10.0); // Random numbers between 0 and 10

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = dis(gen);
        }
    }

    return matrix;
}

// Function to multiply two matrices A (NxM) and B (MxN), result will be NxN
std::vector<std::vector<double>> multiplyMatrices(const std::vector<std::vector<double>>& A, const std::vector<std::vector<double>>& B) {
    int N = A.size();
    int M = A[0].size();
    std::vector<std::vector<double>> result(N, std::vector<double>(N, 0.0));

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            for (int k = 0; k < M; ++k) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return result;
}

// Function to compute mean of a vector
double computeMean(const std::vector<double>& times) {
    double sum = 0.0;
    for (double time : times) {
        sum += time;
    }
    return sum / times.size();
}

// Function to compute standard deviation of a vector
double computeStdDev(const std::vector<double>& times, double mean) {
    double variance = 0.0;
    for (double time : times) {
        variance += (time - mean) * (time - mean);
    }
    return std::sqrt(variance / times.size());
}

int main(int argc, char* argv[]) {
    int N, M, K;
   
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <matrix_size> <num_repetitions> <num_matrices>" << std::endl;
        return 1;
    }
    
    N = std::stoi(argv[1]);
    M = std::stoi(argv[2]);
    K = std::stoi(argv[3]);

    std::vector<double> times; // To store times of each multiplication

    for (int i = 0; i < K; ++i) {
        // Generate random matrices A (NxM) and B (MxN)
        std::vector<std::vector<double>> A = generateMatrix(N, M);
        std::vector<std::vector<double>> B = generateMatrix(M, N);

        // Start time measurement
        auto start = std::chrono::high_resolution_clock::now();

        // Multiply matrices
        std::vector<std::vector<double>> C = multiplyMatrices(A, B);

        // End time measurement
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        // Store the elapsed time in seconds
        times.push_back(elapsed.count());
    }

    // Compute mean and standard deviation of times
    double mean = computeMean(times);
    double stdDev = computeStdDev(times, mean);

    std::cout << "Average time taken for matrix multiplication: " << mean << " seconds\n";
    std::cout << "Standard deviation of time: " << stdDev << " seconds\n";

    return 0;
}
