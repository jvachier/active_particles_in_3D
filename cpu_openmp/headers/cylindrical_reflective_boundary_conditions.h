/**
 * @file cylindrical_reflective_boundary_conditions.h
 * @brief Header file for cylindrical boundary condition enforcement
 * @author Jeremy Vachier
 * @date 2023
 */

#ifndef CYLINDRICAL_REFLECTIVE_BOUNDARY_CONDITIONS_H
#define CYLINDRICAL_REFLECTIVE_BOUNDARY_CONDITIONS_H

#include <time.h>
#include <stdio.h>
#include <omp.h>  // OpenMP for parallel processing
#include <iostream>
#include <random>
#include <cstring>
#include <cmath>

/**
 * @brief Apply reflective boundary conditions for cylindrical confinement
 * 
 * @details
 * Enforces cylindrical confinement by reflecting particles that move beyond
 * the boundaries back into the simulation domain. The cylinder has:
 * - Radius: Wall (radial confinement in x-y plane)
 * - Height: 2*height (axial confinement in z direction, from -height to +height)
 * 
 * Radial confinement (x-y plane):
 * - If distance from origin > Wall, particle is projected back onto cylinder surface
 * - Reflection maintains the angular position but limits the radial distance
 * 
 * Axial confinement (z direction):
 * - Uses adaptive reflection based on distance from wall
 * - For small penetration (< 4*L): reflects particle symmetrically
 * - For large penetration (>= 4*L): repositions to avoid deep penetration
 * 
 * @param[in,out] x Array of x-coordinates for all particles
 * @param[in,out] y Array of y-coordinates for all particles
 * @param[in,out] z Array of z-coordinates for all particles
 * @param[in] Particles Total number of particles
 * @param[in] Wall Cylinder radius (radial boundary)
 * @param[in] height Half-height of cylinder (z boundaries at ±height)
 * @param[in] L Particle diameter (used for boundary tolerance)
 * 
 * @note Uses OpenMP parallel for SIMD for performance
 * @note Reflections are elastic (no energy dissipation)
 */
void cylindrical_reflective_boundary_conditions(
  double *x, double *y, double *z, int Particles,
  double Wall, double height, int L
);

#endif  // CYLINDRICAL_REFLECTIVE_BOUNDARY_CONDITIONS_H
