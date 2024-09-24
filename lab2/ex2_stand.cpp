#include <iostream>
#include <vector>
#include <random>
#include <sys/time.h> // For gettimeofday()
#include <cmath>      // For sqrt() and pow()

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

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <matrix_size_rows> <matrix_size_columns> <num_repetitions>" << std::endl;
        return 1;
    }

    int N = std::stoi(argv[1]);
    int M = std::stoi(argv[2]);
    int K = std::stoi(argv[3]);

    // Measuring time for matrix initialization
    struct timeval start, end;

    // Generate random matrices
    gettimeofday(&start, NULL);
    std::vector<std::vector<double>> A = generateMatrix(N, M);
    std::vector<std::vector<double>> B = generateMatrix(M, N);
    gettimeofday(&end, NULL);
    double initDuration = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    // Prepare result matrix
    std::vector<std::vector<double>> C(N, std::vector<double>(N, 0.0));

    // Measuring time for matrix multiplication
    gettimeofday(&start, NULL);
    for (int i = 0; i < K; ++i) {
        std::vector<std::vector<double>> C = multiplyMatrices(A, B);
    }
    gettimeofday(&end, NULL);
    double multDuration = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    std::cout << "Initialization time: " << initDuration << " seconds\n";
    std::cout << "Multiplication time over " << K << " runs: " << multDuration << " seconds\n";

    return 0;
}
