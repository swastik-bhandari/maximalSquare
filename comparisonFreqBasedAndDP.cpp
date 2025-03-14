#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
using namespace std;
using namespace std::chrono;

// Standard space-optimized DP solution
class StandardSolution {
public:
    int maximalSquare(vector<vector<char>>& matrix) {
        if (matrix.empty() || matrix[0].empty()) return 0;
        int rows = matrix.size(), cols = matrix[0].size();
        int maxSide = 0;
        vector<int> dp(cols + 1, 0), prev(cols + 1, 0);
        for (int i = 1; i <= rows; ++i) {
            for (int j = 1; j <= cols; ++j) {
                if (matrix[i - 1][j - 1] == '1') {
                    dp[j] = min({ prev[j - 1], prev[j], dp[j - 1] }) + 1;
                    maxSide = max(maxSide, dp[j]);
                } else {
                    dp[j] = 0;
                }
            }
            swap(dp, prev);
        }
        return maxSide * maxSide;
    }
};

// User's solution using frequency vector and counters
class UserSolution {
public:
    int maximalSquare(vector<vector<char>>& v) {
        if(v.empty() || v[0].empty()) return 0;
        int row_size = v[0].size();
        int col_size = v.size();
        int found_max_width = 0;
        int check_max_width = 1;
        int check_max_height = 1;
        int counter = 0;
        vector<int> freq(row_size, 0);

        for (int i = 0; i < col_size; i++) {
            for (int j = 0; j < row_size; j++) {
                if (v[i][j] == '1') {
                    freq[j]++;
                    if (check_max_height <= freq[j]) {
                        counter++;
                        if (check_max_width == counter) {
                            found_max_width++;
                            check_max_width++;
                            check_max_height++;
                            counter = 0;
                        }
                        continue;
                    }
                } else {
                    freq[j] = 0;
                }
                counter = 0;
            }
            counter = 0;
        }
        return found_max_width * found_max_width;
    }
};

// Function to generate a matrix with specified dimensions and density of '1's
vector<vector<char>> generateMatrix(int rows, int cols, double density, mt19937& gen) {
    vector<vector<char>> matrix(rows, vector<char>(cols, '0'));
    uniform_real_distribution<> dis(0.0, 1.0);
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = (dis(gen) < density) ? '1' : '0';
        }
    }
    return matrix;
}

// Function to measure execution time for a solution on a given matrix
template<typename Solution>
double measureTime(Solution& solution, vector<vector<char>>& matrix, int iterations) {
    vector<double> times;
    
    for (int i = 0; i < iterations; i++) {
        auto start = high_resolution_clock::now();
        volatile int result = solution.maximalSquare(matrix);
        auto end = high_resolution_clock::now();
        times.push_back(duration<double>(end - start).count());
    }
    
    // Remove outliers (optional) - remove top and bottom 10%
    sort(times.begin(), times.end());
    int trim = iterations / 10;
    vector<double> trimmed(times.begin() + trim, times.end() - trim);
    
    // Calculate mean
    double sum = accumulate(trimmed.begin(), trimmed.end(), 0.0);
    return sum / trimmed.size();
}

// Calculate statistical metrics
struct Statistics {
    double mean;
    double median;
    double stdDev;
    double min;
    double max;
    
    static Statistics calculate(const vector<double>& data) {
        Statistics stats;
        if (data.empty()) return stats;
        
        // Copy data for sorting
        vector<double> sorted = data;
        sort(sorted.begin(), sorted.end());
        
        stats.min = sorted.front();
        stats.max = sorted.back();
        
        // Median
        if (sorted.size() % 2 == 0) {
            stats.median = (sorted[sorted.size()/2 - 1] + sorted[sorted.size()/2]) / 2.0;
        } else {
            stats.median = sorted[sorted.size()/2];
        }
        
        // Mean
        stats.mean = accumulate(sorted.begin(), sorted.end(), 0.0) / sorted.size();
        
        // Standard deviation
        double variance = 0.0;
        for (double value : sorted) {
            variance += pow(value - stats.mean, 2);
        }
        stats.stdDev = sqrt(variance / sorted.size());
        
        return stats;
    }
};

int main() {
    // Set random seed for reproducibility
    unsigned seed = 42;
    mt19937 gen(seed);
    
    StandardSolution stdSol;
    UserSolution userSol;
    
    // Test configurations
    vector<pair<int, int>> matrixSizes = {
        {10, 10}, {50, 50}, {100, 100}, {500, 500}, {1000, 1000}
    };
    
    vector<double> densities = {0.1, 0.3, 0.5, 0.7, 0.9};
    const int iterations = 50;
    const int warmupIterations = 5;
    
    cout << setw(10) << "Size" << setw(10) << "Density" 
         << setw(15) << "Std Time(ms)" << setw(15) << "User Time(ms)" 
         << setw(15) << "Speedup" << setw(15) << "Same Result?" << endl;
    cout << string(80, '-') << endl;
    
    // Run tests for each configuration
    for (const auto& size : matrixSizes) {
        int rows = size.first;
        int cols = size.second;
        
        for (double density : densities) {
            // Generate test matrix
            auto matrix = generateMatrix(rows, cols, density, gen);
            
            // Warmup runs
            for (int i = 0; i < warmupIterations; i++) {
                stdSol.maximalSquare(matrix);
                userSol.maximalSquare(matrix);
            }
            
            // Verify correctness
            int stdResult = stdSol.maximalSquare(matrix);
            int userResult = userSol.maximalSquare(matrix);
            bool sameResult = (stdResult == userResult);
            
            // Measure performance
            vector<double> stdTimes, userTimes;
            
            for (int i = 0; i < iterations; i++) {
                auto start = high_resolution_clock::now();
                volatile int res = stdSol.maximalSquare(matrix);
                auto end = high_resolution_clock::now();
                stdTimes.push_back(duration<double, milli>(end - start).count());
                
                start = high_resolution_clock::now();
                res = userSol.maximalSquare(matrix);
                end = high_resolution_clock::now();
                userTimes.push_back(duration<double, milli>(end - start).count());
            }
            
            // Calculate statistics
            Statistics stdStats = Statistics::calculate(stdTimes);
            Statistics userStats = Statistics::calculate(userTimes);
            
            // Calculate speedup (Standard time divided by User time)
            double speedup = stdStats.mean / userStats.mean;
            
            // Print results
            cout << setw(5) << rows << "x" << setw(4) << cols
                 << setw(10) << fixed << setprecision(2) << density
                 << setw(15) << fixed << setprecision(3) << stdStats.mean
                 << setw(15) << fixed << setprecision(3) << userStats.mean
                 << setw(15) << fixed << setprecision(2) << speedup << "x"
                 << setw(15) << (sameResult ? "Yes" : "No") << endl;
        }
        cout << string(80, '-') << endl;
    }
    
    // Additional tests for edge cases
    cout << "\nEdge Cases:\n";
    
    // Test 1: Empty matrix
    vector<vector<char>> emptyMatrix;
    
    // Test 2: All zeros
    auto allZeros = generateMatrix(100, 100, 0.0, gen);
    
    // Test 3: All ones
    auto allOnes = generateMatrix(100, 100, 1.0, gen);
    
    // Test 4: Single row
    auto singleRow = generateMatrix(1, 1000, 0.5, gen);
    
    // Test 5: Single column
    auto singleCol = generateMatrix(1000, 1, 0.5, gen);
    
    vector<pair<string, vector<vector<char>>>> edgeCases = {
        {"Empty", emptyMatrix},
        {"All 0s", allZeros},
        {"All 1s", allOnes},
        {"Single Row", singleRow},
        {"Single Col", singleCol}
    };
    
    cout << setw(15) << "Case" << setw(15) << "Std Time(ms)" 
         << setw(15) << "User Time(ms)" << setw(15) << "Speedup" 
         << setw(15) << "Same Result?" << endl;
    cout << string(80, '-') << endl;
    
    for (const auto& testCase : edgeCases) {
        string name = testCase.first;
        const auto& matrix = testCase.second;
        
        // Skip the empty matrix if implementations don't handle it
        if (name == "Empty" && matrix.empty()) {
            cout << setw(15) << name << setw(45) << "Skipped (empty matrix)" << endl;
            continue;
        }
        
        // Verify correctness
        int stdResult = stdSol.maximalSquare(const_cast<vector<vector<char>>&>(matrix));
        int userResult = userSol.maximalSquare(const_cast<vector<vector<char>>&>(matrix));
        bool sameResult = (stdResult == userResult);
        
        // Measure performance
        double stdTime = measureTime(stdSol, const_cast<vector<vector<char>>&>(matrix), iterations);
        double userTime = measureTime(userSol, const_cast<vector<vector<char>>&>(matrix), iterations);
        
        // Calculate speedup
        double speedup = stdTime / userTime;
        
        // Print results
        cout << setw(15) << name
             << setw(15) << fixed << setprecision(3) << (stdTime * 1000)
             << setw(15) << fixed << setprecision(3) << (userTime * 1000)
             << setw(15) << fixed << setprecision(2) << speedup << "x"
             << setw(15) << (sameResult ? "Yes" : "No") << endl;
    }
    
    return 0;
}
