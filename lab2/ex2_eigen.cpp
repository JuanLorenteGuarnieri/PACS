#include <iostream>
#include <Eigen/Dense> // Ensure Eigen is correctly included
#include <sys/time.h> // For gettimeofday()
#include <vector>     // For std::vector

// Function to generate random Eigen matrices
void generateEigenMatrices(Eigen::MatrixXd& A, Eigen::MatrixXd& B, size_t N, size_t M) {
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <N> <M> <k>" << std::endl;
        return 1;
    }

    size_t N = std::stoul(argv[1]);  // Number of rows in the resulting matrix
    size_t M = std::stoul(argv[2]);  // Number of columns in the first matrix (and rows in the second)
    size_t k = std::stoul(argv[3]);  // Number of repetitions

    // Measuring time for matrix initialization
    struct timeval start, end;
    
    gettimeofday(&start, NULL);
    Eigen::MatrixXd A = Eigen::MatrixXd::Random(N, M); // Matrix A with random values
    Eigen::MatrixXd B = Eigen::MatrixXd::Random(M, N); // Matrix B with random values
    gettimeofday(&end, NULL);
    double initDuration = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    Eigen::MatrixXd C; // Result matrix

    // Measuring time for matrix multiplication
    gettimeofday(&start, NULL);
    for (size_t i = 0; i < k; ++i) {
        C = A * B; // Matrix multiplication using Eigen
    }
    gettimeofday(&end, NULL);
    double multDuration = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    std::cout << "Initialization time: " << initDuration << " seconds\n";
    std::cout << "Multiplication time over " << k << " runs: " << multDuration << " seconds\n";

    return 0;
}
