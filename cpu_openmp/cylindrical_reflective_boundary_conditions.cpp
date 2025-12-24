/**
 * @file cylindrical_reflective_boundary_conditions.cpp
 * @brief Implementation of cylindrical boundary condition enforcement
 * @author Jeremy Vachier
 * @date 2023
 */

#include "headers/cylindrical_reflective_boundary_conditions.h"

using namespace std;

/**
 * @brief Apply reflective boundary conditions for cylindrical confinement
 * 
 * Enforces confinement in two stages:
 * 
 * 1. Radial confinement (x-y plane):
 *    If particle is outside cylinder (r > Wall), project it back onto
 *    the cylindrical surface while maintaining angular position.
 *    Formula: (x,y) -> Wall * (x,y) / r
 * 
 * 2. Axial confinement (z direction):
 *    Two reflection modes based on penetration depth:
 *    a) Small penetration (D_AW_z <= 4*L): Symmetric reflection
 *    b) Large penetration (D_AW_z > 4*L): Direct repositioning near boundary
 * 
 * Uses OpenMP SIMD parallelization for performance.
 */
void cylindrical_reflective_boundary_conditions(
  double *x, double *y, double *z, int Particles,
  double Wall, double height, int L) {
    double distance_squared = 0.0, Wall_squared = Wall * Wall;
    double height_L = height - L / 2.0;  // Effective boundary position
    double D_AW_z = 0.0;  // Distance from axial wall

#pragma omp parallel for simd
    for (int k = 0; k < Particles; k++) {
      // --- Radial boundary (x-y plane) ---
      // Calculate squared radial distance from cylinder axis
      distance_squared = x[k] * x[k] + y[k] * y[k];
      
      // If particle is outside cylindrical wall, project back to surface
      if (distance_squared > Wall_squared) {
        double scale_factor = sqrt(Wall_squared / distance_squared);
        x[k] = scale_factor * x[k];
        y[k] = scale_factor * y[k];
      }
      
      // --- Axial boundary (z direction) ---
      if (z[k] > height_L) {
        // Particle exceeded top boundary
        D_AW_z = abs(z[k] - height_L);  // Distance from top boundary
        
        if (D_AW_z > 4.0 * L) {
          // Large penetration: hard reset near boundary
          z[k] = height - 2.0 * L;
        } else {
          // Small penetration: elastic reflection
          z[k] = height_L - D_AW_z;
        }
      } else if (z[k] < -height_L) {
        // Particle exceeded bottom boundary
        D_AW_z = abs(z[k] + height_L);  // Distance from bottom boundary
        
        if (D_AW_z > 4.0 * L) {
          // Large penetration: hard reset near boundary
          z[k] = 2.0 * L - height;
        } else {
          // Small penetration: elastic reflection
          z[k] = -height_L + D_AW_z;
        }
      }
    }
}
