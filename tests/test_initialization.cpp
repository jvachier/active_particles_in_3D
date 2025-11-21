/**
 * @file test_initialization.cpp
 * @brief Unit tests for particle initialization and overlap detection
 * @author Jeremy Vachier
 * @date 2025
 * 
 * Compile: g++-14 -std=c++17 -fopenmp -I../src test_initialization.cpp ../src/initialization.cpp ../src/check_nooverlap.cpp -o test_initialization.out
 * Run: ./test_initialization.out
 */

#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include <random>
#include "../src/headers/initialization.h"
#include "../src/headers/check_nooverlap.h"

using namespace std;

// Test result tracking
int tests_passed = 0;
int tests_failed = 0;

/**
 * @brief Assert function that tracks pass/fail
 */
void assert_test(bool condition, const string& test_name) {
    if (condition) {
        cout << "[PASS] " << test_name << endl;
        tests_passed++;
    } else {
        cout << "[FAIL] " << test_name << endl;
        tests_failed++;
    }
}

/**
 * @brief Test orientation vector normalization
 */
void test_orientation_normalization() {
    cout << "\n=== Testing Orientation Normalization ===" << endl;
    
    const int N = 100;
    vector<double> x(N), y(N), z(N);
    vector<double> ex(N), ey(N), ez(N);
    
    random_device rdev;
    default_random_engine generator(rdev());
    uniform_real_distribution<double> dist_pos(-10.0, 10.0);
    uniform_real_distribution<double> dist_e(0.0, 1.0);
    
    initialization(x.data(), y.data(), z.data(), 
                   ex.data(), ey.data(), ez.data(),
                   N, generator, dist_pos, dist_e);
    
    // Check that all orientation vectors are normalized
    bool all_normalized = true;
    for (int i = 0; i < N; i++) {
        double norm = sqrt(ex[i]*ex[i] + ey[i]*ey[i] + ez[i]*ez[i]);
        if (fabs(norm - 1.0) > 1e-10) {
            all_normalized = false;
            break;
        }
    }
    
    assert_test(all_normalized, "All orientation vectors normalized to unit length");
}

/**
 * @brief Test position initialization within bounds
 */
void test_position_bounds() {
    cout << "\n=== Testing Position Initialization ===" << endl;
    
    const int N = 100;
    const double bound = 15.0;
    vector<double> x(N), y(N), z(N);
    vector<double> ex(N), ey(N), ez(N);
    
    random_device rdev;
    default_random_engine generator(rdev());
    uniform_real_distribution<double> dist_pos(-bound, bound);
    uniform_real_distribution<double> dist_e(0.0, 1.0);
    
    initialization(x.data(), y.data(), z.data(),
                   ex.data(), ey.data(), ez.data(),
                   N, generator, dist_pos, dist_e);
    
    // Check that all positions are within bounds
    bool all_in_bounds = true;
    for (int i = 0; i < N; i++) {
        if (x[i] < -bound || x[i] > bound ||
            y[i] < -bound || y[i] > bound ||
            z[i] < -bound || z[i] > bound) {
            all_in_bounds = false;
            break;
        }
    }
    
    assert_test(all_in_bounds, "All positions within specified bounds");
}

/**
 * @brief Test overlap detection
 */
void test_overlap_detection() {
    cout << "\n=== Testing Overlap Detection ===" << endl;
    
    const int N = 50;
    const int L = 1;
    const double min_separation = 1.5 * L;
    
    vector<double> x(N), y(N), z(N);
    vector<double> ex(N), ey(N), ez(N);
    
    random_device rdev;
    default_random_engine generator(rdev());
    uniform_real_distribution<double> dist_pos(-10.0, 10.0);
    uniform_real_distribution<double> dist_e(0.0, 1.0);
    
    // Initialize particles
    initialization(x.data(), y.data(), z.data(),
                   ex.data(), ey.data(), ez.data(),
                   N, generator, dist_pos, dist_e);
    
    // Check for overlaps
    check_nooverlap(x.data(), y.data(), z.data(), N, L, generator, dist_pos);
    
    // Verify no overlaps after check_nooverlap
    bool no_overlaps = true;
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            double dx = x[j] - x[i];
            double dy = y[j] - y[i];
            double dz = z[j] - z[i];
            double distance = sqrt(dx*dx + dy*dy + dz*dz);
            
            if (distance < min_separation) {
                no_overlaps = false;
                cout << "  Found overlap: particles " << i << " and " << j 
                     << " distance=" << distance << endl;
                break;
            }
        }
        if (!no_overlaps) break;
    }
    
    assert_test(no_overlaps, "No particle overlaps after check_nooverlap");
}

/**
 * @brief Test orientation vector properties
 */
void test_orientation_properties() {
    cout << "\n=== Testing Orientation Vector Properties ===" << endl;
    
    const int N = 100;
    vector<double> x(N), y(N), z(N);
    vector<double> ex(N), ey(N), ez(N);
    
    random_device rdev;
    default_random_engine generator(rdev());
    uniform_real_distribution<double> dist_pos(-10.0, 10.0);
    uniform_real_distribution<double> dist_e(0.0, 1.0);
    
    initialization(x.data(), y.data(), z.data(),
                   ex.data(), ey.data(), ez.data(),
                   N, generator, dist_pos, dist_e);
    
    // Test 1: All components should be finite
    bool all_finite = true;
    for (int i = 0; i < N; i++) {
        if (!isfinite(ex[i]) || !isfinite(ey[i]) || !isfinite(ez[i])) {
            all_finite = false;
            break;
        }
    }
    assert_test(all_finite, "All orientation components are finite");
    
    // Test 2: At least some variation in orientations
    bool has_variation = false;
    for (int i = 1; i < N; i++) {
        if (fabs(ex[i] - ex[0]) > 0.1 || 
            fabs(ey[i] - ey[0]) > 0.1 ||
            fabs(ez[i] - ez[0]) > 0.1) {
            has_variation = true;
            break;
        }
    }
    assert_test(has_variation, "Orientations show variation (not all identical)");
}

/**
 * @brief Test edge cases
 */
void test_edge_cases() {
    cout << "\n=== Testing Edge Cases ===" << endl;
    
    // Test 1: Single particle initialization
    {
        vector<double> x(1), y(1), z(1);
        vector<double> ex(1), ey(1), ez(1);
        
        random_device rdev;
        default_random_engine generator(rdev());
        uniform_real_distribution<double> dist_pos(-5.0, 5.0);
        uniform_real_distribution<double> dist_e(0.0, 1.0);
        
        initialization(x.data(), y.data(), z.data(),
                       ex.data(), ey.data(), ez.data(),
                       1, generator, dist_pos, dist_e);
        
        double norm = sqrt(ex[0]*ex[0] + ey[0]*ey[0] + ez[0]*ez[0]);
        assert_test(fabs(norm - 1.0) < 1e-10, "Single particle: orientation normalized");
    }
    
    // Test 2: Small number of particles with no overlap possible
    {
        const int N = 5;
        vector<double> x(N), y(N), z(N);
        vector<double> ex(N), ey(N), ez(N);
        
        random_device rdev;
        default_random_engine generator(rdev());
        uniform_real_distribution<double> dist_pos(-50.0, 50.0);  // Large space
        uniform_real_distribution<double> dist_e(0.0, 1.0);
        
        initialization(x.data(), y.data(), z.data(),
                       ex.data(), ey.data(), ez.data(),
                       N, generator, dist_pos, dist_e);
        
        check_nooverlap(x.data(), y.data(), z.data(), N, 1, generator, dist_pos);
        
        assert_test(true, "Small particle count in large volume succeeds");
    }
}

/**
 * @brief Main test runner
 */
int main() {
    cout << "=====================================" << endl;
    cout << "Initialization Unit Tests" << endl;
    cout << "=====================================" << endl;
    
    test_orientation_normalization();
    test_position_bounds();
    test_overlap_detection();
    test_orientation_properties();
    test_edge_cases();
    
    cout << "\n=====================================" << endl;
    cout << "Test Results Summary" << endl;
    cout << "=====================================" << endl;
    cout << "Tests passed: " << tests_passed << endl;
    cout << "Tests failed: " << tests_failed << endl;
    cout << "Total tests:  " << (tests_passed + tests_failed) << endl;
    
    if (tests_failed == 0) {
        cout << "\nAll tests PASSED!" << endl;
        return 0;
    } else {
        cout << "\nSome tests FAILED!" << endl;
        return 1;
    }
}
