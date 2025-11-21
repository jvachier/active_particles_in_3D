/**
 * @file check_nooverlap.cpp
 * @brief Implementation of particle overlap detection and resolution
 * @author Jeremy Vachier
 * @date 2023
 */

#include "headers/check_nooverlap.h"

using namespace std;

/**
 * @brief Detect and resolve particle overlaps after initialization
 * 
 * Uses nested loops to check all particle pairs. For each pair with
 * separation distance R < 1.5*L, repositions particle j randomly.
 * Repositioning is attempted up to 3 times per overlap. If unsuccessful,
 * the simulation terminates indicating the particle density is too high.
 * 
 * Algorithm:
 * - Outer loop: iterate over all particles k
 * - Inner loop: check particle k against all other particles j
 * - If overlap detected: reposition j and recompute distance
 * - Limit: 3 repositioning attempts per overlap
 */
void check_nooverlap(
  double *x, double *y, double *z, int Particles,
  int L,
  default_random_engine &generator,
  uniform_real_distribution<double> &distribution) {
    int count = 0;
    double R = 0.0;  // Inter-particle distance
    
#pragma omp parallel for simd
    for (int k = 0; k < Particles; k++) {
      for (int j = 0; j < Particles; j++) {
        if (k != j) {  // Skip self-interaction
          // Calculate Euclidean distance between particles k and j
          R = sqrt((x[j] - x[k]) * (x[j] - x[k]) \
            + (y[j] - y[k]) * (y[j] - y[k]) \
            + (z[j] - z[k]) * (z[j] - z[k]));
          
          count = 0;
          // Reposition particle j if too close to particle k
          while (R < 1.5 * L) {
            // Generate new random position for particle j
            x[j] = distribution(generator);
            y[j] = distribution(generator);
            
            // Recompute distance with new position
            R = sqrt((x[j] - x[k]) * (x[j] - x[k]) \
              + (y[j] - y[k]) * (y[j] - y[k])\
              + (z[j] - z[k]) * (z[j] - z[k]));
            
            count += 1;
            // Exit if repositioning fails after 3 attempts
            if (count > 3) {
              printf("Error: Number of particles too high for given volume.\n");
              printf("Reduce particle count or increase confinement size.\n");
              exit(0);
            }
          }
        }
      }
    }
}