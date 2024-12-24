#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include "CImg.h"
#include "cl_lib.cpp"

using namespace cimg_library;
using namespace std;

std::vector<std::vector<float>> convolve(const CImg<float>& image, 
                                         const std::vector<std::vector<float>>& kernel) {
    int rows = image.height();
    int cols = image.width();
    int kernel_rows = kernel.size();
    int kernel_cols = kernel[0].size();
    int kr = kernel_rows / 2;
    int kc = kernel_cols / 2;

    std::vector<std::vector<float>> output(rows, std::vector<float>(cols, 0.0f));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < kernel_rows; ++k) {
                for (int l = 0; l < kernel_cols; ++l) {
                    int x = i + k - kr;
                    int y = j + l - kc;

                    if (x >= 0 && x < rows && y >= 0 && y < cols) {
                        sum += image(x, y) * kernel[k][l];
                    }
                }
            }
            output[i][j] = sum;
        }
    }
    return output;
}

void sobel_cpu(const CImg<float>& image, CImg<float>& output, 
                                          const std::vector<std::vector<float>>& kernel_gx, 
                                          const std::vector<std::vector<float>>& kernel_gy) {
    int rows = image.height();
    int cols = image.width();

    auto gx = convolve(image, kernel_gx);
    auto gy = convolve(image, kernel_gy);

    std::vector<std::vector<float>> gradient_magnitude(rows, std::vector<float>(cols, 0.0f));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            output(i, j) = std::sqrt(gx[i][j] * gx[i][j] + gy[i][j] * gy[i][j]);
        }
    }
}

void sobel_gpu(Cl_runtime *runtime, const CImg<float>& image, CImg<float>& output, 
                                          const std::vector<std::vector<float>>& kernel_gx, 
                                          const std::vector<std::vector<float>>& kernel_gy,
                                          double &communicationTime, double &computationTime) {



    int rows = image.height();
    int cols = image.width();

    std::vector<std::vector<float>> matrix(rows, std::vector<float>(cols));
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            matrix[y][x] = image(x, y);
        }
    }

    auto gx = runtime->convolution(matrix, kernel_gx, communicationTime, computationTime);
    auto gy = runtime->convolution(matrix, kernel_gy, communicationTime, computationTime);
    auto gradient_magnitude = runtime->gradientMagnitudes(gx, gy, communicationTime, computationTime);

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            output(x, y) = gradient_magnitude[y][x];
        }
    }
}

vector<CImg<float>> create_image_stream(const CImg<float>& image, int num_images) {
    vector<CImg<float>> image_stream;
    for (int i = 0; i < num_images; ++i) {
        image_stream.push_back(image);
    }
    return image_stream;
}

void sobel_cpu_batch(const vector<CImg<float>>& images, vector<CImg<float>>& outputs,
                     const vector<vector<float>>& kernel_gx, const vector<vector<float>>& kernel_gy,
                     atomic<int>& global_execution_count, double& total_time, int& local_count, int max_images) {
    while (global_execution_count.load() < max_images) {
        for (size_t i = 0; i < images.size() && global_execution_count.load() < max_images; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            sobel_cpu(images[i], outputs[i], kernel_gx, kernel_gy);
            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> duration = end - start;
            total_time += duration.count();

            global_execution_count++;
            local_count++;
        }
    }
}

void sobel_gpu_batch(const vector<CImg<float>>& images, vector<CImg<float>>& outputs,
                     const vector<vector<float>>& kernel_gx, const vector<vector<float>>& kernel_gy,
                     atomic<int>& global_execution_count, double& total_time, int& local_count, int max_images,
                                          double &communicationTime, double &computationTime) {

    
    while (global_execution_count.load() < max_images) {

        for (size_t i = 0; i < images.size() && global_execution_count.load() < max_images; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            Cl_runtime runtime;
            sobel_gpu(&runtime, images[i], outputs[i], kernel_gx, kernel_gy, communicationTime, computationTime);
            
            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> duration = end - start;
            total_time += duration.count();
            
            global_execution_count++;
            local_count++;
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_image>" << endl;
        return 1;
    }

    const char* input_path = argv[1];
    CImg<float> image(input_path);

    if (image.spectrum() > 1) {
        image = image.RGBtoYCbCr().channel(0);
    }

    const int total_images_to_process = 50;
    const int num_images = 10;
    auto image_stream = create_image_stream(image, num_images);

    vector<CImg<float>> cpu_outputs(num_images, CImg<float>(image.width(), image.height(), 1, 1, 0));
    vector<CImg<float>> gpu_outputs(num_images, CImg<float>(image.width(), image.height(), 1, 1, 0));

    vector<vector<float>> kernel_gx = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    vector<vector<float>> kernel_gy = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};

    // Variables compartidas entre threads
    atomic<int> global_execution_count(0);
    double cpu_total_time(0.0), gpu_total_time(0.0);
    int cpu_execution_count(0), gpu_execution_count(0);

    double communicationTime = 0.0;
    double computationTime = 0.0;

    // Iniciar threads
    thread cpu_thread(sobel_cpu_batch, ref(image_stream), ref(cpu_outputs), ref(kernel_gx), ref(kernel_gy),
                      ref(global_execution_count), ref(cpu_total_time), ref(cpu_execution_count), total_images_to_process);
    thread gpu_thread(sobel_gpu_batch, ref(image_stream), ref(gpu_outputs), ref(kernel_gx), ref(kernel_gy),
                      ref(global_execution_count), ref(gpu_total_time), ref(gpu_execution_count), total_images_to_process, ref(communicationTime), ref(computationTime));

    gpu_thread.join();
    cpu_thread.join();

    // Guardar algunas imágenes procesadas
    cpu_outputs[0].save("../output/output_cpu_sample.jpg");
    gpu_outputs[0].save("../output/output_gpu_sample.jpg");

    // Resultados
    double cpu_average_time = cpu_total_time / cpu_execution_count;
    double gpu_average_time = gpu_total_time / gpu_execution_count;
    double gpu_average_communication_time = communicationTime / gpu_execution_count;
    double gpu_average_computation_time = computationTime / gpu_execution_count;
    

    cout << "CPU Execution count: " << cpu_execution_count << endl;
    cout << "GPU Execution count: " << gpu_execution_count << endl;
    cout << "Total time (CPU): " << cpu_total_time << " seconds" << endl;
    cout << "Total time (GPU): " << gpu_total_time << " seconds" << endl;
    cout << "Total communication time (GPU): " << communicationTime << " seconds" << endl;
    cout << "Total computation time (GPU): " << computationTime << " seconds" << endl;
    cout << "----------------------------------------------" << endl;
    cout << "Average time per image (CPU): " << cpu_average_time << " seconds" << endl;
    cout << "Average time per image (GPU): " << gpu_average_time << " seconds" << endl;
    cout << "Average communication time per image (GPU): " << gpu_average_communication_time << " seconds" << endl;
    cout << "Average computation time per image (GPU): " << gpu_average_computation_time << " seconds" << endl;

    return 0;
}