/**
 * @file test_boundary_conditions.cpp
 * @brief Unit tests for cylindrical reflective boundary conditions
 * @author Jeremy Vachier
 * @date 2025
 * 
 * Compile: g++-14 -std=c++17 -I../cpu_openmp test_boundary_conditions.cpp ../cpu_openmp/cylindrical_reflective_boundary_conditions.cpp -o test_boundary_conditions.out
 * Run: ./test_boundary_conditions.out
 */

#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include "../cpu_openmp/headers/cylindrical_reflective_boundary_conditions.h"

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
 * @brief Test radial boundary enforcement
 */
void test_radial_boundary() {
    cout << "\n=== Testing Radial Boundary Conditions ===" << endl;
    
    const double Wall = 10.0;
    const double height = 20.0;
    const int L = 1;
    
    // Test 1: Particle outside cylinder - should be projected onto surface
    {
        double x = 15.0;  // Beyond wall
        double y = 0.0;
        double z = 0.0;
        
        cylindrical_reflective_boundary_conditions(&x, &y, &z, 1, Wall, height, L);
        
        double r = sqrt(x*x + y*y);
        assert_test(fabs(r - Wall) < 1e-10, "Radial reflection: particle outside cylinder");
    }
    
    // Test 2: Particle inside cylinder - should remain unchanged
    {
        double x = 5.0;
        double y = 3.0;
        double z = 0.0;
        double x_orig = x, y_orig = y;
        
        cylindrical_reflective_boundary_conditions(&x, &y, &z, 1, Wall, height, L);
        
        assert_test(x == x_orig && y == y_orig, "Radial: particle inside cylinder unchanged");
    }
    
    // Test 3: Particle exactly on boundary
    {
        double x = Wall;
        double y = 0.0;
        double z = 0.0;
        
        cylindrical_reflective_boundary_conditions(&x, &y, &z, 1, Wall, height, L);
        
        double r = sqrt(x*x + y*y);
        assert_test(r <= Wall + 1e-10, "Radial: particle on boundary");
    }
    
    // Test 4: Particle far outside - extreme case
    {
        double x = 100.0;
        double y = 100.0;
        double z = 0.0;
        
        cylindrical_reflective_boundary_conditions(&x, &y, &z, 1, Wall, height, L);
        
        double r = sqrt(x*x + y*y);
        assert_test(fabs(r - Wall) < 1e-10, "Radial: extreme distance particle");
    }
}

/**
 * @brief Test axial (z-direction) boundary enforcement
 */
void test_axial_boundary() {
    cout << "\n=== Testing Axial Boundary Conditions ===" << endl;
    
    const double Wall = 10.0;
    const double height = 20.0;
    const int L = 1;
    const double height_L = height - L / 2.0;
    
    // Test 1: Particle above top boundary - small penetration
    {
        double x = 0.0;
        double y = 0.0;
        double z = height_L + 1.0;  // Small penetration
        
        cylindrical_reflective_boundary_conditions(&x, &y, &z, 1, Wall, height, L);
        
        assert_test(z < height_L, "Axial: particle above top boundary reflected");
    }
    
    // Test 2: Particle below bottom boundary - small penetration
    {
        double x = 0.0;
        double y = 0.0;
        double z = -height_L - 1.0;  // Small penetration
        
        cylindrical_reflective_boundary_conditions(&x, &y, &z, 1, Wall, height, L);
        
        assert_test(z > -height_L, "Axial: particle below bottom boundary reflected");
    }
    
    // Test 3: Particle inside z-bounds - should remain unchanged
    {
        double x = 0.0;
        double y = 0.0;
        double z = 5.0;
        double z_orig = z;
        
        cylindrical_reflective_boundary_conditions(&x, &y, &z, 1, Wall, height, L);
        
        assert_test(z == z_orig, "Axial: particle inside bounds unchanged");
    }
    
    // Test 4: Large penetration - should be repositioned
    {
        double x = 0.0;
        double y = 0.0;
        double z = height_L + 10.0;  // Large penetration
        
        cylindrical_reflective_boundary_conditions(&x, &y, &z, 1, Wall, height, L);
        
        assert_test(z < height_L, "Axial: large penetration repositioned");
    }
}

/**
 * @brief Test combined radial and axial boundaries
 */
void test_combined_boundary() {
    cout << "\n=== Testing Combined Boundary Conditions ===" << endl;
    
    const double Wall = 10.0;
    const double height = 20.0;
    const int L = 1;
    const double height_L = height - L / 2.0;
    
    // Test 1: Particle outside both radial and axial bounds
    {
        double x = 15.0;  // Outside radial
        double y = 0.0;
        double z = height_L + 2.0;  // Outside axial
        
        cylindrical_reflective_boundary_conditions(&x, &y, &z, 1, Wall, height, L);
        
        double r = sqrt(x*x + y*y);
        assert_test(r <= Wall + 1e-10 && z < height_L, 
                   "Combined: particle outside both boundaries corrected");
    }
    
    // Test 2: Multiple particles
    {
        double x[3] = {15.0, 5.0, 0.0};
        double y[3] = {0.0, 0.0, 0.0};
        double z[3] = {0.0, height_L + 2.0, -height_L - 2.0};
        
        cylindrical_reflective_boundary_conditions(x, y, z, 3, Wall, height, L);
        
        bool all_inside = true;
        for (int i = 0; i < 3; i++) {
            double r = sqrt(x[i]*x[i] + y[i]*y[i]);
            if (r > Wall + 1e-10 || z[i] > height_L || z[i] < -height_L) {
                all_inside = false;
            }
        }
        assert_test(all_inside, "Combined: multiple particles all confined");
    }
}

/**
 * @brief Test edge cases
 */
void test_edge_cases() {
    cout << "\n=== Testing Edge Cases ===" << endl;
    
    const double Wall = 10.0;
    const double height = 20.0;
    const int L = 1;
    
    // Test 1: Particle at origin
    {
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
        
        cylindrical_reflective_boundary_conditions(&x, &y, &z, 1, Wall, height, L);
        
        assert_test(x == 0.0 && y == 0.0 && z == 0.0, "Edge: particle at origin unchanged");
    }
    
    // Test 2: Very small cylinder
    {
        double x = 2.0;
        double y = 0.0;
        double z = 0.0;
        double small_wall = 1.0;
        
        cylindrical_reflective_boundary_conditions(&x, &y, &z, 1, small_wall, 2.0, L);
        
        double r = sqrt(x*x + y*y);
        assert_test(r <= small_wall + 1e-10, "Edge: small cylinder confinement");
    }
}

/**
 * @brief Main test runner
 */
int main() {
    cout << "=====================================" << endl;
    cout << "Boundary Conditions Unit Tests" << endl;
    cout << "=====================================" << endl;
    
    test_radial_boundary();
    test_axial_boundary();
    test_combined_boundary();
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
