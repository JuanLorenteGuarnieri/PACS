#include <iomanip>
#include <iostream>
#include <limits>
#include <chrono>

// Allow to change the floating point type
using my_float = long double;

my_float pi_taylor(size_t steps) {
    my_float pi = 0.0;
    my_float sign = 1.0; // To alternate between positive and negative terms
    for (size_t i = 0; i < steps; ++i) {
        pi += sign / (2.0 * i + 1.0);
        sign = -sign; // Alternate the sign
    }
    return 4.0 * pi; // Taylor series for arctan(x=1)
}

int main(int argc, const char *argv[]) {

    // Read the number of steps from the command line
    if (argc != 2) {
        std::cerr << "Invalid syntax: pi_taylor <steps>" << std::endl;
        exit(1);
    }

    // Convert the command-line argument to size_t
    size_t steps = std::stoll(argv[1]);

    // Start timing
    auto start = std::chrono::high_resolution_clock::now();
    my_float pi = pi_taylor(steps);
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate elapsed time
    std::chrono::duration<double> elapsed_seconds = end - start;

    // Output the result with precision and time taken
    std::cout << "For " << steps << " steps, pi value: "
              << std::setprecision(std::numeric_limits<my_float>::digits10 + 1)
              << pi << std::endl;
    std::cout << "Time taken: " << elapsed_seconds.count() << " seconds\n";
}
