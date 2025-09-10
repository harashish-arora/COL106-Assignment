#!/bin/bash
set -e

g++ -std=c++11 -O2 main.cpp -o main

echo "Build complete. Run with ./main"
