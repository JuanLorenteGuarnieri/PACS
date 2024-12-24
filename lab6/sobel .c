////////////////////////////////////////////////////////////////////
//File: sobel.c
//
//Description: sobel program
//
// 
////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

#define cimg_use_jpeg
#include <iostream>
#include "CImg.h" 
using namespace cimg_library;

#ifdef __APPLE__
  #include <OpenCL/opencl.h>
#else
  #include <CL/cl.h>
#endif

#include "cl_lib.cpp"
  

////////////////////////////////////////////////////////////////////////////////

// Function to load a kernel from a file
std::vector<std::vector<float>> loadKernel(const char* kernel_path) {
    std::vector<std::vector<float>> kernel;
    std::ifstream kernel_file(kernel_path);

    if (!kernel_file.is_open()) {
        std::cerr << "Error: Can't open the kernel file: " << kernel_path << "\n";
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(kernel_file, line)) {
        std::istringstream iss(line);
        std::vector<float> row;
        float value;

        // Read all the values of the actual line
        while (iss >> value) {
            row.push_back(value);
        }
        kernel.push_back(row);
    }

    kernel_file.close();
    return kernel;
}

// Function to get the current date and time as a string
std::string getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* local_time = std::localtime(&now_time);

    std::ostringstream oss;
    oss << std::put_time(local_time, "%Y%m%d_%H%M%S");
    return oss.str();
}

int main(int argc, char** argv) {
    if (argc < 6) {
        fprintf(stderr, "Usage: %s <input_image_path> <output_image_path> <kernel_gx_path> <kernel_gy_path> <log_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* input_path = argv[1];
    const char* output_path = argv[2];
    const char* kernel_gx_path = argv[3];
    const char* kernel_gy_path = argv[4];
    const char* log_path = argv[5];

    cl_int err;
    Cl_runtime runtime;

    // Load image
    CImg<float> image(input_path);

    // Convert the image to grayscale if it's RGB
    if (image.spectrum() > 1) {
        image = image.RGBtoYCbCr().channel(0);
    }

    const int rows = image.height();
    const int cols = image.width();
    std::vector<std::vector<float>> matrix(rows, std::vector<float>(cols));

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            matrix[y][x] = image(x, y);
        }
    }

    // Load Sobel kernels for gx and gy
    std::vector<std::vector<float>> kernel_gx = loadKernel(kernel_gx_path);
    std::vector<std::vector<float>> kernel_gy = loadKernel(kernel_gy_path);

    // Measure the execution time of the overall program
    auto program_start = std::chrono::high_resolution_clock::now();

    auto kernel_start = std::chrono::high_resolution_clock::now();
    // Perform convolution for gx (horizontal edges)
    auto gx = runtime.convolution(matrix, kernel_gx);

    // Perform convolution for gy (vertical edges)
    auto gy = runtime.convolution(matrix, kernel_gy);
    auto kernel_end = std::chrono::high_resolution_clock::now();
    
    std::vector<float> flattenGx(gx.size()*gx[0].size());
    std::vector<float> flattenGy(gy.size()*gy[0].size());

    for (size_t i = 0; i < gx.size(); i++){
        for (size_t j = 0; j < gx[0].size(); j++){
            flattenGx[i*gx[0].size() + j] = gx[i][j];
        }
    }
    for (size_t i = 0; i < gy.size(); i++){
        for (size_t j = 0; j < gy[0].size(); j++){
            flattenGy[i*gy[0].size() + j] = gy[i][j];
        }
    }

    auto kernel_start2 = std::chrono::high_resolution_clock::now();
    // auto gx2 = runtime.pow2(gx);
    // auto gy2 = runtime.pow2(gy);
    // auto gradient_magnitude = runtime.sumSqrt(gx2, gy2);

    auto gradient_magnitude = runtime.gradientMagnitudes(gx, gy);
    auto kernel_end2 = std::chrono::high_resolution_clock::now();


    // Save the gradient magnitude image
    CImg<float> output_image(cols, rows, 1, 1);
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            output_image(x, y) = gradient_magnitude[y][x];
        }
    }
    output_image.save(output_path);

    auto program_end = std::chrono::high_resolution_clock::now();

    // Calculate metrics
    double program_execution_time = std::chrono::duration<double>(program_end - program_start).count();
    double kernel_execution_time = std::chrono::duration<double>(kernel_end - kernel_start).count();
    double kernel_execution_time2 = std::chrono::duration<double>(kernel_end2 - kernel_start2).count();
    kernel_execution_time += kernel_execution_time2;
    size_t memory_transfer = rows * cols * sizeof(float) + kernel_gx.size() * kernel_gx[0].size() * sizeof(float) +
                             kernel_gy.size() * kernel_gy[0].size() * sizeof(float);
    double memory_bandwidth = memory_transfer / kernel_execution_time;
    double throughput = rows * cols / kernel_execution_time; // Pixels per second

    // Memory footprint
    size_t memory_footprint = matrix.size() * matrix[0].size() * sizeof(float) +
                              kernel_gx.size() * kernel_gx[0].size() * sizeof(float) +
                              kernel_gy.size() * kernel_gy[0].size() * sizeof(float);

    // Create log file
    std::string log_filename = std::string(log_path) + "/sobel_" + getCurrentDateTime() + ".log";
    std::ofstream log_file(log_filename);

    if (!log_file.is_open()) {
        std::cerr << "Error: Unable to create log file at " << log_filename << "\n";
        return EXIT_FAILURE;
    }

    // Write metrics to the log file
    log_file << "Program Parameters: \n";
    log_file << "Execution time (overall program): " << program_execution_time << " seconds\n";
    log_file << "Execution time (kernel): " << kernel_execution_time << " seconds\n";
    log_file << "Memory bandwidth: " << memory_bandwidth / (1024 * 1024) << " MB/s\n";
    log_file << "Kernel throughput: " << throughput / (1024 * 1024) << " MPixels/s\n";
    log_file << "Memory footprint: " << memory_footprint / (1024 * 1024) << " MB\n";

    log_file.close();

    std::cout << "Log saved to: " << log_filename << "\n";

    return 0;
}