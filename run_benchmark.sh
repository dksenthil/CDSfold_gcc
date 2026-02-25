#!/bin/bash

echo "CDSfold Performance Benchmark Runner"
echo "===================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    local color=$1
    local message=$2
    echo -e "${color}${message}${NC}"
}

# Compile benchmark tool
print_status $YELLOW "Compiling benchmark tool..."
/usr/local/bin/g++-15 -std=c++17 -O3 -march=native -mtune=native \
    -Wall -Wextra -o benchmark benchmark.cpp

if [ $? -eq 0 ]; then
    print_status $GREEN "✓ Benchmark tool compiled successfully"
else
    print_status $RED "✗ Failed to compile benchmark tool"
    exit 1
fi

# Check if CDSfold is built
print_status $YELLOW "Checking CDSfold build status..."
if [ ! -f "./src/CDSfold" ]; then
    print_status $YELLOW "CDSfold not found. Attempting to build..."

    # Try to build CDSfold
    make clean
    make

    if [ $? -eq 0 ] && [ -f "./src/CDSfold" ]; then
        print_status $GREEN "✓ CDSfold built successfully"
    else
        print_status $RED "✗ CDSfold build failed (likely Vienna RNA not configured)"
        print_status $YELLOW "Running benchmark analysis anyway..."
    fi
else
    print_status $GREEN "✓ CDSfold executable found"
fi

# Run benchmark
print_status $YELLOW "Running performance benchmark..."
echo ""
./benchmark

# Show compiler info
echo ""
print_status $YELLOW "Compiler Information:"
/usr/local/bin/g++-15 --version | head -1
echo "Optimization flags: -std=c++17 -O3 -march=native -mtune=native"

# Show system info
echo ""
print_status $YELLOW "System Information:"
echo "CPU: $(sysctl -n machdep.cpu.brand_string 2>/dev/null || echo 'Unknown')"
echo "Architecture: $(uname -m)"
echo "OS: $(uname -s) $(uname -r)"

# Cleanup
print_status $YELLOW "Cleaning up..."
rm -f benchmark test_*.faa

print_status $GREEN "Benchmark complete!"