/**
 * @file initialization.cpp
 * @brief Implementation of particle initialization functions
 * @author Jeremy Vachier
 * @date 2023
 */

#include "headers/initialization.h"

using namespace std;

/**
 * @brief Initialize all particles with random positions and orientations
 * 
 * Two-stage initialization:
 * 1. Generate random orientation vectors and normalize them to unit length
 * 2. Generate random positions uniformly distributed within the domain
 * 
 * Both stages use OpenMP SIMD parallelization for improved performance.
 */
void initialization(
  double *x, double *y, double *z,
  double *ex, double *ey, double *ez,
  int Particles,
  default_random_engine &generator,
  uniform_real_distribution<double> &distribution,
  uniform_real_distribution<double> &distribution_e) {
  double norm_e = 0.0, invers_norm_e = 0.0;

  // Stage 1: Initialize and normalize orientation vectors
#pragma omp parallel for simd
  for (int k = 0; k < Particles; k++) {
    // Generate random orientation components
    ex[k] = distribution_e(generator);
    ey[k] = distribution_e(generator);
    ez[k] = distribution_e(generator);

    // Normalize to unit vector: e = e / |e|
    norm_e = sqrt(ex[k] * ex[k] + ey[k] * ey[k] + ez[k] * ez[k]);
    invers_norm_e = 1.0 / norm_e;

    ex[k] = ex[k] * invers_norm_e;
    ey[k] = ey[k] * invers_norm_e;
    ez[k] = ez[k] * invers_norm_e;
  }

  // Stage 2: Initialize positions uniformly within cylindrical domain
#pragma omp parallel for simd
  for (int k = 0; k < Particles; k++) {
    x[k] = distribution(generator);
    y[k] = distribution(generator);
    z[k] = distribution(generator);
  }
}
