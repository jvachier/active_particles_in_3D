/**
 * @file update_position.h
 * @brief Header file for particle dynamics integration
 * @author Jeremy Vachier
 * @date 2023
 */

#ifndef UPDATE_POSITION_H
#define UPDATE_POSITION_H

#include <iostream>
#include <random>
#include <cstring>
#include <time.h>
#include <stdio.h>
#include <omp.h>
#include <cmath>

/**
 * @brief Update particle positions and orientations for one timestep
 * 
 * @details
 * Implements one step of the Euler-Maruyama numerical scheme to integrate
 * the coupled stochastic differential equations for particle dynamics.
 * 
 * Two-step process:
 * 1. Update orientations using rotational diffusion (Ito formulation)
 *    de/dt = sqrt(2*De*delta) * e x xi_e
 * 
 * 2. Update positions using active propulsion, interactions, and thermal noise
 *    dr/dt = vs*e + F_interaction + sqrt(2*Dt*delta)*xi_p
 * 
 * Interactions:
 * - Lennard-Jones repulsive force computed for all particle pairs
 * - Cutoff distance: r (typically 5*L)
 * - Force capped at maximum value to prevent numerical instability
 * 
 * @param[in,out] x Array of x-coordinates for all particles
 * @param[in,out] y Array of y-coordinates for all particles
 * @param[in,out] z Array of z-coordinates for all particles
 * @param[in,out] ex Array of x-components of orientation unit vectors
 * @param[in,out] ey Array of y-components of orientation unit vectors
 * @param[in,out] ez Array of z-components of orientation unit vectors
 * @param[in] prefactor_e Prefactor for orientation noise: sqrt(2*delta*De)
 * @param[in] Particles Total number of particles
 * @param[in] delta Time step size
 * @param[in] De Rotational diffusion coefficient
 * @param[in] Dt Translational diffusion coefficient
 * @param[in] xi_ex Rotational noise for ex component
 * @param[in] xi_ey Rotational noise for ey component
 * @param[in] xi_ez Rotational noise for ez component
 * @param[in] xi_px Translational noise in x direction
 * @param[in] xi_py Translational noise in y direction
 * @param[in] xi_pz Translational noise in z direction
 * @param[in] vs Self-propulsion velocity
 * @param[in] prefactor_xi_px Prefactor for x-position noise: sqrt(2*delta*Dt)
 * @param[in] prefactor_xi_py Prefactor for y-position noise: sqrt(2*delta*Dt)
 * @param[in] prefactor_xi_pz Prefactor for z-position noise: sqrt(2*delta*Dt)
 * @param[in] r Interaction cutoff radius
 * @param[in] prefactor_interaction Lennard-Jones force prefactor: epsilon*48
 * @param[in,out] generator Random number generator engine
 * @param[in] Gaussdistribution Gaussian distribution for translational noise
 * @param[in] distribution_e Uniform distribution for rotational noise
 * 
 * @note Orientation vectors are normalized after each update
 * @note Uses OpenMP parallel for SIMD for performance
 * @note Force is capped at maximum value of 1.0 to prevent instabilities
 */
void update_position(
  double *x, double *y, double *z,
  double *ex, double *ey, double *ez,
  double prefactor_e, int Particles,
  double delta, double De, double Dt,
  double xi_ex, double xi_ey, double xi_ez, double xi_px,
  double xi_py, double xi_pz, double vs,
  double prefactor_xi_px, double prefactor_xi_py, double prefactor_xi_pz,
  double r, double prefactor_interaction,
  std::default_random_engine &generator,
  std::normal_distribution<double> &Gaussdistribution,
  std::uniform_real_distribution<double> &distribution_e);

#endif  // UPDATE_POSITION_H
