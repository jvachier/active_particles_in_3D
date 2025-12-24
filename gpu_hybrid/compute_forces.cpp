/**
 * @file compute_forces.cpp
 * @brief CPU implementation of force calculations
 * 
 * OpenMP-parallelized Lennard-Jones force computation.
 * 
 * @author Active Particles 3D Simulation
 * @date 2024
 */

#include "headers/compute_forces.h"
#include <cmath>
#include <omp.h>

/**
 * @brief Compute LJ forces on CPU with OpenMP
 */
void compute_forces_cpu(
    const std::vector<double>& x,
    const std::vector<double>& y,
    const std::vector<double>& z,
    std::vector<double>& fx,
    std::vector<double>& fy,
    std::vector<double>& fz,
    double prefactor,
    int numParticles)
{
    // Zero out force arrays
    #pragma omp parallel for
    for (int i = 0; i < numParticles; i++) {
        fx[i] = 0.0;
        fy[i] = 0.0;
        fz[i] = 0.0;
    }
    
    // Compute pairwise forces
    #pragma omp parallel for
    for (int i = 0; i < numParticles; i++) {
        double fx_local = 0.0;
        double fy_local = 0.0;
        double fz_local = 0.0;
        
        for (int j = 0; j < numParticles; j++) {
            if (i == j) continue;
            
            // Distance vector
            double dx = x[j] - x[i];
            double dy = y[j] - y[i];
            double dz = z[j] - z[i];
            
            // Distance squared and powers
            double R2 = dx*dx + dy*dy + dz*dz;
            double R4 = R2 * R2;
            double R8 = R4 * R4;
            double R14 = R8 * R4 * R2;

            // Lennard-Jones force magnitude (repulsive part: 2/R^14 - attractive part: 1/R^8)
            // F = prefactor * (2/R^14 - 1/R^8) where prefactor = 24*epsilon
            double force_mag = prefactor * (2.0 / R14 - 1.0 / R8);
            
            // Accumulate force components
            fx_local += force_mag * dx;
            fy_local += force_mag * dy;
            fz_local += force_mag * dz;
        }
        
        fx[i] = fx_local;
        fy[i] = fy_local;
        fz[i] = fz_local;
    }
}
