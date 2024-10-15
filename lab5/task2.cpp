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

    // Get image dimensions
    int width = img.width();
    int height = img.height();

    // Draw a blue line in the middle of the image
    unsigned char blue[] = {0, 0, 255}; // Define a blue color
    img.draw_line(0, height / 2, width, height / 2, blue); // Horizontal line
    img.draw_line(width / 2, 0, width / 2, height, blue); // Vertical line

    // Display the modified image
    img.display("Image with Blue Lines");

    return 0;
}
