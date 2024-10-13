#include <iostream>
#include <sys/time.h> // For gettimeofday()
#include <cmath>      // For sqrt() and pow()
#include <random>
#include <vector>     // For std::vector

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
        std::cerr << "Usage: " << argv[0] << " <N> <M> <k>" << std::endl;
        return 1;
    }

    size_t N = std::stoul(argv[1]);  // Number of rows in the resulting matrix
    size_t M = std::stoul(argv[2]);  // Number of columns in the first matrix (and rows in the second)
    size_t k = std::stoul(argv[3]);  // Number of repetitions

    // Variable to accumulate total duration
    std::vector<double> durations_init, durations_mult;
    struct timeval start, end;

    for (size_t i = 0; i < k; ++i) {
        // Start time measurement
        gettimeofday(&start, NULL);

        // Using Eigen's MatrixXd for dynamic size matrices
        std::vector<std::vector<double>> A = generateMatrix(N, M);
        std::vector<std::vector<double>> B = generateMatrix(M, N);

        gettimeofday(&end, NULL);
        double initDuration = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
        durations_init.push_back(initDuration);

        // Perform matrix multiplication using Eigen

        gettimeofday(&start, NULL);
        std::vector<std::vector<double>> C = multiplyMatrices(A, B);

        // End time measurement        
        gettimeofday(&end, NULL);

        
        // Calculate elapsed time
        double multDuration = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
        durations_mult.push_back(multDuration);
    }

    // Calculate average duration
    double totalDuration = 0.0;
    for (const double& time : durations_init) {
        totalDuration += time;
    }
    double averageDuration_init = totalDuration / k;
    totalDuration = 0.0;
    for (const double& time : durations_mult) {
        totalDuration += time;
    }
    double averageDuration_mult = totalDuration / k;

    std::cout << "Initialization time: " << averageDuration_init << " seconds\n";
    std::cout << "Multiplication time: " << averageDuration_mult << " seconds\n";

    return 0;
}