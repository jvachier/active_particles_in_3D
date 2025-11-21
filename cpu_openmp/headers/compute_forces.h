/**
 * @file compute_forces.h
 * @brief Declaration of force computation functions
 * 
 * Provides both CPU (OpenMP) and GPU (Metal) implementations
 * of Lennard-Jones force calculations.
 * 
 * @author Active Particles 3D Simulation
 * @date 2024
 */

#ifndef COMPUTE_FORCES_H
#define COMPUTE_FORCES_H

#include <vector>

/**
 * @brief Compute Lennard-Jones forces using CPU (OpenMP)
 * 
 * @param x X-coordinates of particles
 * @param y Y-coordinates of particles
 * @param z Z-coordinates of particles
 * @param fx Output X-component of forces
 * @param fy Output Y-component of forces
 * @param fz Output Z-component of forces
 * @param prefactor Interaction strength (epsilon * delta)
 * @param numParticles Number of particles
 */
void compute_forces_cpu(
    const std::vector<double>& x,
    const std::vector<double>& y,
    const std::vector<double>& z,
    std::vector<double>& fx,
    std::vector<double>& fy,
    std::vector<double>& fz,
    double prefactor,
    int numParticles
);

#endif // COMPUTE_FORCES_H
