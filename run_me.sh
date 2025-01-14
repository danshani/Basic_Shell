#!/bin/bash

# Step 1: Compile the ex1.c file with -Wall warnings
gcc -Wall -o shell shell.c

# Check if the compilation was successful
if [ $? -eq 0 ]; then
  echo "Compilation successful. Running the program..."
  
  # Step 2: Run the compiled program
  ./shell
else
  echo "Compilation failed."
fi
