/**
 * @file abp_3D_confine.cpp
 * @brief Main simulation file for Active Brownian Particles in 3D cylindrical confinement
 * @author Jeremy Vachier
 * @date 2023
 * @version 1.0
 * 
 * @details
 * This program simulates the dynamics of active Brownian particles (ABPs) confined
 * within a cylindrical geometry in three-dimensional space. The simulation uses the
 * Euler-Maruyama numerical scheme to solve coupled stochastic differential equations
 * (Langevin equations) for particle position and orientation.
 * 
 * Physics:
 * - Position evolution: dr/dt = v_s*e - grad(U) + sqrt(2*D_t)*xi_t
 * - Orientation evolution: de/dt = sqrt(2*D_e)*e x xi_e
 * - Interaction potential: Lennard-Jones (repulsive part only)
 * - Boundary conditions: Cylindrical reflective walls
 * 
 * Parallelization:
 * - OpenMP is used for parallel computation of particle dynamics
 * - SIMD vectorization for enhanced performance
 * 
 * Compilation:
 * - Standard: g++-13 -O3 -std=c++17 -fopenmp abp_3D_confine.cpp -o abp_3D_confine.out
 * - With SIMD: g++-13 -O3 -std=c++17 -fopenmp -fopenmp-simd abp_3D_confine.cpp -o abp_3D_confine.out
 * - macOS: Use g++-13 (install via: brew install gcc)
 * 
 * Input:
 * - parameter.txt: Simulation parameters (epsilon, delta, N_particles, etc.)
 * 
 * Output:
 * - data/simulation.csv: Time-series data of particle positions and orientations
 * 
 * @see README.md for detailed parameter descriptions and usage examples
 */
#include <omp.h>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <random>
#include <cstring>
#include <cmath>
#include <tuple>

#include "headers/print_file.h"
#include "headers/cylindrical_reflective_boundary_conditions.h"
#include "headers/initialization.h"
#include "headers/update_position.h"
#include "headers/check_nooverlap.h"

#define PI 3.141592653589793
#define N_thread 6

using namespace std;

/**
 * @brief Main entry point for the simulation
 * @param argc Number of command line arguments (currently unused)
 * @param argv Array of command line arguments (currently unused)
 * @return int Exit status (0 for success, non-zero for error)
 */
int main(int argc, char *argv[]) {
  // Open input parameter file and output data file
  FILE *datacsv;
  FILE *parameter;
  parameter = fopen("parameter.txt", "r");
  datacsv = fopen("./data/simulation.csv", "w");

  // Verify parameter file exists
  if (parameter == NULL) {
    printf("Error: parameter.txt file not found.\n");
    return 0;
  }

  // Set number of OpenMP threads for parallelization
  omp_set_num_threads(N_thread);

  // Simulation parameters read from file
  double epsilon;     // Interaction strength (Lennard-Jones potential depth)
  double delta;       // Time step size
  double Dt;          // Translational diffusion coefficient
  double De;          // Rotational diffusion coefficient
  double vs;          // Self-propulsion velocity
  double Wall;        // Cylinder radius
  double height;      // Cylinder height
  int Particles;      // Number of particles in simulation
  int N;              // Total number of time iterations

  // Read parameters from tab-separated file
  fscanf(parameter, "%lf\t%lf\t%d\t%lf\t%lf\t%lf\t%lf\t%lf\t%d\n", \
    &epsilon, &delta, &Particles, &Dt, &De, &vs, &Wall, &height, &N);
  
  // Echo parameters to console for verification
  printf("Simulation parameters:\n");
  printf("epsilon=%lf delta=%lf Particles=%d Dt=%lf De=%lf vs=%lf Wall=%lf height=%lf N=%d\n", \
    epsilon, delta, Particles, Dt, De, vs, Wall, height, N);

  // Allocate memory for particle positions (x, y, z coordinates)
  double *x = reinterpret_cast<double*> \
    (malloc(Particles * sizeof(double)));
  double *y = reinterpret_cast<double*> \
    (malloc(Particles * sizeof(double)));
  double *z = reinterpret_cast<double*> \
    (malloc(Particles * sizeof(double)));

  // Allocate memory for particle orientations (unit vector components)
  double *ex = reinterpret_cast<double*> \
    (malloc(Particles * sizeof(double)));
  double *ey = reinterpret_cast<double*> \
    (malloc(Particles * sizeof(double)));
  double *ez = reinterpret_cast<double*> \
    (malloc(Particles * sizeof(double)));

  // Physical constants
  const int L = 1.0;  // Particle diameter (characteristic length scale)

  // Initialize random number generator with hardware entropy
  random_device rdev;
  default_random_engine generator(rdev());

  // Define probability distributions for stochastic noise
  normal_distribution<double> Gaussdistribution(0.0, 1.0);  // For translational noise
  uniform_real_distribution<double> distribution(-Wall, Wall);  // For initial positions
  uniform_real_distribution<double> distribution_e(0.0, 1.0);  // For orientation noise

  // Stochastic noise variables (Gaussian white noise)
  double xi_px = 0.0;  // Translational noise in x-direction
  double xi_py = 0.0;  // Translational noise in y-direction
  double xi_pz = 0.0;  // Translational noise in z-direction
  double xi_ex = 0.0;  // Rotational noise for ex component
  double xi_ey = 0.0;  // Rotational noise for ey component
  double xi_ez = 0.0;  // Rotational noise for ez component

  // Pre-computed prefactors for Euler-Maruyama integration
  double prefactor_e = sqrt(2.0 * delta * De);           // Orientation diffusion prefactor
  double prefactor_xi_px = sqrt(2.0 * delta * Dt);       // x-position noise prefactor
  double prefactor_xi_py = sqrt(2.0 * delta * Dt);       // y-position noise prefactor
  double prefactor_xi_pz = sqrt(2.0 * delta * Dt);       // z-position noise prefactor
  double prefactor_interaction = epsilon * 48.0;         // Lennard-Jones force prefactor
  double r = 5.0 * L;  // Interaction cutoff radius (5 particle diameters)

  // Start timing the simulation using OpenMP
  double itime, ftime, exec_time;
  itime = omp_get_wtime();

  // Write CSV header
  fprintf(datacsv, "Particles,x-position,y-position,z-position, "\
    "ex-orientation,ey-orientation,ez-orientation,time\n");

  // Initialize particle positions and orientations randomly within the cylinder
  initialization(
    x, y, z, ex, ey, ez, Particles,
    generator, distribution, distribution_e);

  // Verify no particles overlap initially (enforce minimum separation)
  check_nooverlap(
    x, y, z, Particles, L,
    generator, distribution);
  printf("Initialization complete. Starting simulation...\n");

  // Main simulation loop: integrate equations of motion
  for (int time = 0; time < N; time++) {
    // Update particle positions and orientations using Euler-Maruyama scheme
    update_position(
      x, y, z, ex, ey, ez, prefactor_e, Particles,
      delta, De, Dt, xi_ex, xi_ey, xi_ez, xi_px,
      xi_py, xi_pz, vs, prefactor_xi_px, prefactor_xi_py, prefactor_xi_pz,
      r, prefactor_interaction,
      generator, Gaussdistribution, distribution_e);

    // Apply reflective boundary conditions at cylindrical walls
    cylindrical_reflective_boundary_conditions(
      x, y, z, Particles,
      Wall, height, L);

    // Save particle states every 10 timesteps to reduce file size
    if (time % 10 == 0 && time >= 0) {
      print_file(
        x, y, z, ex, ey, ez,
        Particles, time,
        datacsv);
      }
    }

  // Calculate and display total execution time
  ftime = omp_get_wtime();
  exec_time = ftime - itime;
  printf("Simulation complete. Time taken: %.3f seconds\n", exec_time);

  // Free dynamically allocated memory
  free(x);
  free(y);
  free(z);
  free(ex);
  free(ey);
  free(ez);

  // Close output file
  fclose(datacsv);
  fclose(parameter);
  
  printf("Results saved to ./data/simulation.csv\n");
  return 0;
}
