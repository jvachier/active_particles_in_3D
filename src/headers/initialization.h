/**
 * @file initialization.h
 * @brief Header file for particle initialization functions
 * @author Jeremy Vachier
 * @date 2023
 */

#ifndef INITIALIZATION_H
#define INITIALIZATION_H

#include <time.h>
#include <stdio.h>
#include <omp.h>  // OpenMP for parallel processing
#include <iostream>
#include <random>
#include <cstring>
#include <cmath>

/**
 * @brief Initialize particle positions and orientations randomly
 * 
 * @details
 * This function randomly initializes all particles within the simulation domain.
 * Positions are uniformly distributed within the cylindrical boundaries.
 * Orientation vectors are randomly generated and normalized to unit length.
 * 
 * The initialization uses OpenMP SIMD pragmas for vectorized parallel execution.
 * 
 * @param[out] x Array of x-coordinates for all particles
 * @param[out] y Array of y-coordinates for all particles
 * @param[out] z Array of z-coordinates for all particles
 * @param[out] ex Array of x-components of orientation unit vectors
 * @param[out] ey Array of y-components of orientation unit vectors
 * @param[out] ez Array of z-components of orientation unit vectors
 * @param[in] Particles Total number of particles to initialize
 * @param[in,out] generator Random number generator engine
 * @param[in] distribution Uniform distribution for position initialization
 * @param[in] distribution_e Uniform distribution for orientation components
 * 
 * @note Orientation vectors are automatically normalized to unit length
 * @note Uses OpenMP parallel for SIMD for performance
 */
void initialization(
  double *x, double *y, double *z,
  double *ex, double *ey, double *ez,
  int Particles,
  std::default_random_engine &generator,
  std::uniform_real_distribution<double> &distribution,
  std::uniform_real_distribution<double> &distribution_e);

#endif  // INITIALIZATION_H
