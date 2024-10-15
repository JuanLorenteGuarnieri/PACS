#!/bin/bash

# Create the file bin if doesn't exist
mkdir -p ./bin

# Compile the files
g++ example.cpp -o ./bin/example -I "./CImg" -lm -lpthread -lX11 -ljpeg
g++ task1.cpp -o ./bin/task1 -I "./CImg" -lm -lpthread -lX11 -ljpeg
g++ task2.cpp -o ./bin/task2 -I "./CImg" -lm -lpthread -lX11 -ljpeg
