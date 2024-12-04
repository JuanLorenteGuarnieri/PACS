#!/bin/bash

# Directories
IMAGE_DIR="./images"
KERNEL_DIR="./kernels"
LOG_DIR="./logs"
OUTPUT_DIR="./output"
EXECUTABLE="./bin/basic_environ"

# Check if directories exist
if [ ! -d "$IMAGE_DIR" ]; then
  echo "Error: The images directory ($IMAGE_DIR) does not exist."
  exit 1
fi

if [ ! -d "$KERNEL_DIR" ]; then
  echo "Error: The kernels directory ($KERNEL_DIR) does not exist."
  exit 1
fi

if [ ! -d "$LOG_DIR" ]; then
  echo "Creating the logs directory ($LOG_DIR)."
  mkdir -p "$LOG_DIR"
fi

if [ ! -d "$OUTPUT_DIR" ]; then
  echo "Creating the output directory ($OUTPUT_DIR)."
  mkdir -p "$OUTPUT_DIR"
fi

# Check if the executable exists
if [ ! -f "$EXECUTABLE" ]; then
  echo "Error: The executable ($EXECUTABLE) does not exist."
  exit 1
fi

# Process all images with all kernels
for image in "$IMAGE_DIR"/*; do
  # Ensure it's a file
  if [ -f "$image" ]; then
    image_name=$(basename "$image")
    output_image_name="${OUTPUT_DIR}/${image_name%.*}"
    echo "Processing image $image"

    for kernel in "$KERNEL_DIR"/kernel_*.txt; do
      if [ -f "$kernel" ]; then
        kernel_name=$(basename "$kernel" .txt)
        output_image="${output_image_name}_${kernel_name}.jpg"

        echo "Processing image $image with kernel $kernel..."
        "$EXECUTABLE" "$image" "$output_image" "$kernel" "$LOG_DIR"
      fi
    done
  fi
done

echo "Processing completed. Logs available in $LOG_DIR and results in $OUTPUT_DIR."
