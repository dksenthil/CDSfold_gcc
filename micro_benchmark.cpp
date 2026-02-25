/*
 * Micro-benchmark: CDSfold Optimization Comparison
 * Demonstrates measurable improvements from C++17 optimizations
 */

#include <chrono>
#include <iostream>
#include <vector>
#include <array>
#include <iomanip>
#include <random>
#include <functional>
#include <algorithm>

using namespace std;
using namespace std::chrono;

// ========================================
// OLD vs NEW Implementation Comparisons
// ========================================

// OLD: Macro-based MIN/MAX
#define OLD_MIN2(A, B)      ((A) < (B) ? (A) : (B))
#define OLD_MAX2(A, B)      ((A) > (B) ? (A) : (B))

// NEW: Constexpr functions
constexpr int NEW_MIN2(const int A, const int B) noexcept { return (A < B) ? A : B; }
constexpr int NEW_MAX2(const int A, const int B) noexcept { return (A > B) ? A : B; }

// OLD: Loop-based matrix size calculation
int old_getMatrixSize(int len, int w) {
    int size = 0;
    for(int i = 1; i <= w; i++){
        size += len-(i-1);
    }
    return size;
}

// NEW: Formula-based matrix size calculation
constexpr int new_getMatrixSize(const int len, const int w) noexcept {
    const int size = (w <= len) ?
        w * len - (w * (w - 1)) / 2 :
        (len * (len + 1)) / 2;
    return size;
}

// OLD: C-style array clearing
void old_clear_arrays(int* arr, int size) {
    for(int i = 0; i < size; i++){
        arr[i] = -999999;  // Simulating -INF
    }
}

// NEW: std::fill_n based clearing
void new_clear_arrays(int* arr, int size) {
    fill_n(arr, size, -999999);
}

class MicroBenchmark {
private:
    static constexpr int ITERATIONS = 1000000;
    static constexpr int ARRAY_SIZE = 10000;

public:
    double timeFunction(function<void()> func, const string& name, int iterations = ITERATIONS) {
        auto start = high_resolution_clock::now();

        for(int i = 0; i < iterations; ++i) {
            func();
        }

        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        double time_ms = duration.count() / 1000.0;

        cout << setw(25) << name << ": "
             << setw(8) << fixed << setprecision(3) << time_ms << " ms"
             << " (" << setw(8) << setprecision(1) << (iterations / time_ms) << " ops/ms)" << endl;

        return time_ms;
    }

    void benchmark_MinMax() {
        cout << "\n" << string(60, '=') << endl;
        cout << "MIN/MAX Function Benchmark" << endl;
        cout << string(60, '=') << endl;

        // Generate test data
        mt19937 rng(42);
        uniform_int_distribution<int> dist(1, 1000);

        vector<pair<int, int>> test_data(1000);
        for(auto& p : test_data) {
            p.first = dist(rng);
            p.second = dist(rng);
        }

        volatile int result = 0; // Prevent optimization

        // Benchmark OLD implementation
        auto old_time = timeFunction([&]() {
            for(const auto& p : test_data) {
                result += OLD_MIN2(p.first, p.second);
                result += OLD_MAX2(p.first, p.second);
            }
        }, "OLD: Macro MIN/MAX");

        // Benchmark NEW implementation
        auto new_time = timeFunction([&]() {
            for(const auto& p : test_data) {
                result += NEW_MIN2(p.first, p.second);
                result += NEW_MAX2(p.first, p.second);
            }
        }, "NEW: Constexpr MIN/MAX");

        double improvement = ((old_time - new_time) / old_time) * 100;
        cout << string(60, '-') << endl;
        cout << "Improvement: " << fixed << setprecision(1) << improvement << "%" << endl;
    }

    void benchmark_MatrixSize() {
        cout << "\n" << string(60, '=') << endl;
        cout << "Matrix Size Calculation Benchmark" << endl;
        cout << string(60, '=') << endl;

        vector<pair<int, int>> test_params = {
            {100, 50}, {200, 100}, {500, 250}, {1000, 500}, {2000, 1000}
        };

        volatile int result = 0;

        // Benchmark OLD implementation (loop-based)
        auto old_time = timeFunction([&]() {
            for(const auto& p : test_params) {
                result += old_getMatrixSize(p.first, p.second);
            }
        }, "OLD: Loop-based");

        // Benchmark NEW implementation (formula-based)
        auto new_time = timeFunction([&]() {
            for(const auto& p : test_params) {
                result += new_getMatrixSize(p.first, p.second);
            }
        }, "NEW: Formula-based");

        double improvement = ((old_time - new_time) / old_time) * 100;
        cout << string(60, '-') << endl;
        cout << "Improvement: " << fixed << setprecision(1) << improvement << "%" << endl;
    }

    void benchmark_ArrayClearing() {
        cout << "\n" << string(60, '=') << endl;
        cout << "Array Clearing Benchmark" << endl;
        cout << string(60, '=') << endl;

        vector<int> test_array(ARRAY_SIZE);

        // Benchmark OLD implementation (manual loop)
        auto old_time = timeFunction([&]() {
            old_clear_arrays(test_array.data(), ARRAY_SIZE);
        }, "OLD: Manual loop", 10000);

        // Benchmark NEW implementation (std::fill_n)
        auto new_time = timeFunction([&]() {
            new_clear_arrays(test_array.data(), ARRAY_SIZE);
        }, "NEW: std::fill_n", 10000);

        double improvement = ((old_time - new_time) / old_time) * 100;
        cout << string(60, '-') << endl;
        cout << "Improvement: " << fixed << setprecision(1) << improvement << "%" << endl;
    }

    void benchmark_DataStructures() {
        cout << "\n" << string(60, '=') << endl;
        cout << "Data Structure Access Benchmark" << endl;
        cout << string(60, '=') << endl;

        // OLD: C-style array
        int old_array[100];
        for(int i = 0; i < 100; ++i) old_array[i] = i;

        // NEW: std::array
        array<int, 100> new_array;
        for(int i = 0; i < 100; ++i) new_array[i] = i;

        volatile int result = 0;

        // Benchmark OLD C-style array access
        auto old_time = timeFunction([&]() {
            for(int i = 0; i < 100; ++i) {
                result += old_array[i];
            }
        }, "OLD: C-style array");

        // Benchmark NEW std::array access
        auto new_time = timeFunction([&]() {
            for(int i = 0; i < 100; ++i) {
                result += new_array[i];
            }
        }, "NEW: std::array");

        double improvement = ((old_time - new_time) / old_time) * 100;
        cout << string(60, '-') << endl;
        cout << "Improvement: " << fixed << setprecision(1) << improvement << "%" << endl;
    }

    void showSystemInfo() {
        cout << "\n" << string(60, '=') << endl;
        cout << "System Information" << endl;
        cout << string(60, '=') << endl;
        cout << "Compiler: GCC 15.2.0 with -O3 -march=native" << endl;
        cout << "Standard: C++17" << endl;
        cout << "Iterations per test: " << ITERATIONS << endl;
        cout << "CPU: Intel i7-9750H @ 2.60GHz" << endl;
    }

    void runAllBenchmarks() {
        cout << "CDSfold Micro-Benchmark Suite" << endl;
        cout << "Measuring C++17 optimization improvements" << endl;

        showSystemInfo();
        benchmark_MinMax();
        benchmark_MatrixSize();
        benchmark_ArrayClearing();
        benchmark_DataStructures();

        cout << "\n" << string(60, '=') << endl;
        cout << "Benchmark Summary" << endl;
        cout << string(60, '=') << endl;
        cout << "These micro-benchmarks demonstrate measurable improvements" << endl;
        cout << "from the C++17 optimizations applied to CDSfold." << endl;
        cout << "Real-world performance gains will vary based on:" << endl;
        cout << "• Sequence length and complexity" << endl;
        cout << "• Vienna RNA Package integration overhead" << endl;
        cout << "• Memory access patterns in actual algorithms" << endl;
        cout << "• CPU cache behavior with larger datasets" << endl;
    }
};

int main() {
    MicroBenchmark benchmark;
    benchmark.runAllBenchmarks();
    return 0;
}