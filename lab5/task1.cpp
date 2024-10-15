#define cimg_use_jpeg
#include <iostream>
#include "CImg.h"  // Ensure the path to CImg is correct
using namespace cimg_library;

int main() {
    // Load image file "image.jpg" into object img
    CImg<unsigned char> img("../image.jpg");

    // Dump some characteristics of the loaded image
    std::cout << "Image width: " << img.width() << std::endl;
    std::cout << "Image height: " << img.height() << std::endl;
    std::cout << "Number of slices: " << img.depth() << std::endl;
    std::cout << "Number of channels: " << img.spectrum() << std::endl;

    // Prompt for pixel coordinates
    int i, j;
    std::cout << "Enter pixel coordinates (i j): ";
    std::cin >> i >> j;

    // Print pixel value for each channel (Red, Green, Blue)
    std::cout << "Pixel value at (" << i << ", " << j << "):" << std::endl;
    std::cout << "Red: " << std::hex << (int) img(i, j, 0, 0) << std::endl;   // Red channel
    std::cout << "Green: " << std::hex << (int) img(i, j, 0, 1) << std::endl; // Green channel
    std::cout << "Blue: " << std::hex << (int) img(i, j, 0, 2) << std::endl;  // Blue channel

    // Display the image in a display window
    img.display("Loaded Image");

    return 0;
}
