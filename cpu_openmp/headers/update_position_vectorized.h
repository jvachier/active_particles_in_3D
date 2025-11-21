/**
 * @file update_position_vectorized.h
 * @brief Declaration for vectorized position update with pre-computed forces
 * @author Active Particles 3D Simulation
 * @date 2024
 */

#ifndef UPDATE_POSITION_VECTORIZED_H
#define UPDATE_POSITION_VECTORIZED_H

#include <random>
#include <vector>

/**
 * @brief Update particle orientations using rotational diffusion
 * 
 * @param ex X-component of orientation vectors
 * @param ey Y-component of orientation vectors
 * @param ez Z-component of orientation vectors
 * @param prefactor_e Rotational diffusion prefactor sqrt(2*De*delta)
 * @param Particles Number of particles
 * @param generator Random number generator
 * @param distribution_e Uniform distribution for orientation noise
 */
void update_orientations(
    std::vector<double>& ex,
    std::vector<double>& ey,
    std::vector<double>& ez,
    double prefactor_e,
    int Particles,
    std::default_random_engine &generator,
    std::uniform_real_distribution<double> &distribution_e
);

/**
 * @brief Update particle positions using pre-computed forces
 * 
 * @param x X-coordinates
 * @param y Y-coordinates
 * @param z Z-coordinates
 * @param ex X-component of orientations
 * @param ey Y-component of orientations
 * @param ez Z-component of orientations
 * @param fx X-component of forces
 * @param fy Y-component of forces
 * @param fz Z-component of forces
 * @param delta Time step
 * @param Dt Translational diffusion coefficient
 * @param vs Self-propulsion velocity
 * @param Particles Number of particles
 * @param generator Random number generator
 * @param Gaussdistribution Gaussian distribution for translational noise
 */
void update_positions_with_forces(
    std::vector<double>& x,
    std::vector<double>& y,
    std::vector<double>& z,
    const std::vector<double>& ex,
    const std::vector<double>& ey,
    const std::vector<double>& ez,
    const std::vector<double>& fx,
    const std::vector<double>& fy,
    const std::vector<double>& fz,
    double delta,
    double Dt,
    double vs,
    int Particles,
    std::default_random_engine &generator,
    std::normal_distribution<double> &Gaussdistribution
);

#endif // UPDATE_POSITION_VECTORIZED_H
