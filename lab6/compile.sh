#!/bin/bash

# Create the file bin if doesn't exist
mkdir -p ./bin

# Compile the files
g++ sobel_concurrent.c -o ./bin/sobel_concurrent -I "./CImg" -lm -lpthread -lX11 -ljpeg -lOpenCL
