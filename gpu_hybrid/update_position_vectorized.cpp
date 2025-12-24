/**
 * @file update_position_vectorized.cpp
 * @brief Vectorized implementation of particle dynamics with pre-computed forces
 * @author Active Particles 3D Simulation
 * @date 2024
 */

#include "headers/update_position_vectorized.h"
#include <cmath>
#include <omp.h>

/**
 * @brief Update orientations via rotational diffusion
 */
void update_orientations(
    std::vector<double>& ex,
    std::vector<double>& ey,
    std::vector<double>& ez,
    double prefactor_e,
    int Particles,
    std::default_random_engine &generator,
    std::uniform_real_distribution<double> &distribution_e)
{
    #pragma omp parallel for simd
    for (int k = 0; k < Particles; k++) {
        // Generate random orientation noise
        double xi_ex = distribution_e(generator);
        double xi_ey = distribution_e(generator);
        double xi_ez = distribution_e(generator);

        // Ito formulation: de = prefactor_e * (e x xi_e)
        double new_ex = prefactor_e * (ey[k] * xi_ez - ez[k] * xi_ey) + ex[k];
        double new_ey = prefactor_e * (ez[k] * xi_ex - ex[k] * xi_ez) + ey[k];
        double new_ez = prefactor_e * (ex[k] * xi_ey - ey[k] * xi_ex) + ez[k];

        // Normalize orientation vector
        double norm_e = sqrt(new_ex*new_ex + new_ey*new_ey + new_ez*new_ez);
        double invers_norm_e = 1.0 / norm_e;

        ex[k] = new_ex * invers_norm_e;
        ey[k] = new_ey * invers_norm_e;
        ez[k] = new_ez * invers_norm_e;
    }
}

/**
 * @brief Update positions using pre-computed forces
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
    std::normal_distribution<double> &Gaussdistribution)
{
    double prefactor_noise = sqrt(2.0 * delta * Dt);

    #pragma omp parallel for simd
    for (int k = 0; k < Particles; k++) {
        // Generate Gaussian white noise
        double xi_px = Gaussdistribution(generator);
        double xi_py = Gaussdistribution(generator);
        double xi_pz = Gaussdistribution(generator);

        // Euler-Maruyama position update:
        // dr = [vs*e + F] * dt + sqrt(2*Dt*dt) * xi_p
        x[k] += vs * ex[k] * delta + fx[k] * delta + xi_px * prefactor_noise;
        y[k] += vs * ey[k] * delta + fy[k] * delta + xi_py * prefactor_noise;
        z[k] += vs * ez[k] * delta + fz[k] * delta + xi_pz * prefactor_noise;
    }
}
