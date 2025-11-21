/**
 * @file check_nooverlap.h
 * @brief Header file for particle overlap detection and resolution
 * @author Jeremy Vachier
 * @date 2023
 */

#ifndef CHECK_NOOVERLAP_H
#define CHECK_NOOVERLAP_H

#include <time.h>
#include <stdio.h>
#include <omp.h>  // OpenMP for parallel processing
#include <iostream>
#include <random>
#include <cstring>
#include <cmath>

/**
 * @brief Ensure no particles overlap after initialization
 * 
 * @details
 * Checks all particle pairs for overlap (distance < 1.5*L) and repositions
 * overlapping particles to new random locations. This prevents unphysical
 * initial configurations that could cause numerical instabilities.
 * 
 * The function uses a nested loop to check all particle pairs. If overlap is
 * detected, the second particle (j) is repositioned randomly up to 3 times.
 * If repositioning fails after 3 attempts, the simulation terminates with an
 * error message indicating too many particles for the given volume.
 * 
 * @param[in,out] x Array of x-coordinates for all particles
 * @param[in,out] y Array of y-coordinates for all particles
 * @param[in,out] z Array of z-coordinates for all particles
 * @param[in] Particles Total number of particles
 * @param[in] L Particle diameter (characteristic length scale)
 * @param[in,out] generator Random number generator engine
 * @param[in] distribution Uniform distribution for repositioning
 * 
 * @note Minimum separation enforced: 1.5 * L (1.5 particle diameters)
 * @note Program exits if repositioning fails after 3 attempts
 * @note Uses OpenMP parallel for SIMD for performance
 * 
 * @warning High particle densities may cause initialization failure
 */
void check_nooverlap(
  double *x, double *y, double *z, int Particles,
  int L,
  std::default_random_engine &generator,
  std::uniform_real_distribution<double> &distribution);

#endif  // CHECK_NOOVERLAP_H
