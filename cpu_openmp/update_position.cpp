/**
 * @file update_position.cpp
 * @brief Implementation of particle dynamics integration
 * @author Jeremy Vachier
 * @date 2023
 */

#include "headers/update_position.h"

using namespace std;

/**
 * @brief Update particle positions and orientations using Euler-Maruyama scheme
 * 
 * Two-stage integration process:
 * 
 * Stage 1 - Orientation update (rotational diffusion):
 * Implements: de/dt = sqrt(2*De*delta) * (e x xi_e)
 * Uses Ito formulation for stochastic integration
 * Normalizes orientation vectors after update
 * 
 * Stage 2 - Position update (active motion + interactions + thermal diffusion):
 * Implements: dr/dt = vs*e + F_LJ*r + sqrt(2*Dt*delta)*xi_p
 * where F_LJ is the Lennard-Jones repulsive force
 * 
 * Force calculation:
 * - Nested loop over particle pairs (O(N^2) complexity)
 * - Cutoff distance: r (typically 5*L)
 * - Force formula: F = 24*epsilon * (2/R^14 - 1/R^8) (Lennard-Jones)
 * - Force capping: maximum value of 1.0 to prevent numerical instability
 */
void update_position(
  double *x, double *y, double *z,
  double *ex, double *ey, double *ez, double prefactor_e,
  int Particles,
  double delta, double De, double Dt,
  double xi_ex, double xi_ey, double xi_ez, double xi_px,
  double xi_py, double xi_pz, double vs,
  double prefactor_xi_px, double prefactor_xi_py,
  double prefactor_xi_pz,
  double r, double prefactor_interaction,
  default_random_engine &generator,
  normal_distribution<double> &Gaussdistribution,
  uniform_real_distribution<double> &distribution_e) {
    double a = 0.0;  // Force magnitude from single particle interaction
    double norm_e = 0.0, invers_norm_e = 0.0;  // For orientation normalization
    double F = 0.0;  // Total force on particle
    double R = 0.0;  // Inter-particle distance

    // --- Stage 1: Update orientations via rotational diffusion ---
#pragma omp parallel for simd
    for (int k = 0; k < Particles; k++) {
       // Generate random orientation noise (uniform distribution)
       xi_ex = distribution_e(generator);
       xi_ey = distribution_e(generator);
       xi_ez = distribution_e(generator);

       // Ito formulation: de = prefactor_e * (e x xi_e) * dt
       // Cross product: e x xi_e
       ex[k] = prefactor_e * (ey[k] * xi_ez - ez[k] * xi_ey) + ex[k];
       ey[k] = prefactor_e * (ez[k] * xi_ex - ex[k] * xi_ez) + ey[k];
       ez[k] = prefactor_e * (ex[k] * xi_ey - ey[k] * xi_ex) + ez[k];

       // Normalize orientation vector to unit length
       norm_e = sqrt(ex[k] * ex[k] + ey[k] * ey[k] + ez[k] * ez[k]);
       invers_norm_e = 1.0 / norm_e;

       ex[k] = ex[k] * invers_norm_e;
       ey[k] = ey[k] * invers_norm_e;
       ez[k] = ez[k] * invers_norm_e;
    }

  // --- Stage 2: Update positions with active motion and interactions ---
#pragma omp parallel for simd
    for (int k = 0; k < Particles; k++) {
      // Generate Gaussian white noise for translational diffusion
      xi_px = Gaussdistribution(generator);
      xi_py = Gaussdistribution(generator);
      xi_pz = Gaussdistribution(generator);

      // Compute total interaction force from all neighboring particles
      F = 0.0;
      for (int j = 0; j < Particles; j++) {
        if (k != j) {  // Skip self-interaction
          // Calculate Euclidean distance between particles k and j
          R = sqrt((x[j] - x[k]) * (x[j] - x[k])\
            + (y[j] - y[k]) * (y[j] - y[k])\
            + (z[j] - z[k]) * (z[j] - z[k]));
          
          // Apply Lennard-Jones force if within cutoff radius
          if (R < r) {
            // Lennard-Jones force (repulsive part: 2/R^14 - attractive part: 1/R^8)
            // F = prefactor * (2/R^14 - 1/R^8) where prefactor = 24*epsilon
            double R8 = pow(R, 8);
            double R14 = pow(R, 14);
            a = prefactor_interaction * (2.0 / R14 - 1.0 / R8);

            // Cap force to prevent numerical instability at very small distances
            if (a > 1.0) {
              a = 1.0;
            }
            F += a;
          }
        }
      }
      
    // Euler-Maruyama position update:
    // dr = [vs*e + F*r] * dt + sqrt(2*Dt*dt) * xi_p
    x[k] = x[k] + vs * ex[k] * delta \
      + F * x[k] * delta + xi_px * prefactor_xi_px;
    y[k] = y[k] + vs * ey[k] * delta \
      + F * y[k] * delta + xi_py * prefactor_xi_py;
    z[k] = z[k] + vs * ez[k] * delta \
      + F * z[k] * delta + xi_pz * prefactor_xi_pz;
  }
}
