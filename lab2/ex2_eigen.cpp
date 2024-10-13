#include <Eigen/Dense> // Ensure Eigen is correctly included
#include <iostream>
#include <sys/time.h> // For gettimeofday()
#include <cmath>      // For sqrt() and pow()
#include <random>
#include <vector>     // For std::vector


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
        Eigen::MatrixXd A = Eigen::MatrixXd::Random(N, M); // Matrix A with random values
        Eigen::MatrixXd B = Eigen::MatrixXd::Random(M, N); // Matrix B with random values

        gettimeofday(&end, NULL);
        double initDuration = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
        durations_init.push_back(initDuration);

        // Perform matrix multiplication using Eigen

        gettimeofday(&start, NULL);
        // Perform matrix multiplication using Eigen
        Eigen::MatrixXd C = A * B;

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