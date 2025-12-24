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

using namespace std;

/**
 * @brief Main entry point for the simulation
 * @param argc Number of command line arguments (currently unused)
 * @param argv Array of command line arguments (currently unused)
 * @return int Exit status (0 for success, non-zero for error)
 */
int main(int argc, char *argv[]) {
  // Open input parameter file and output data file
  FILE *datafile;
  FILE *parameter;
  parameter = fopen("../parameter.txt", "r");
  datafile = nullptr;  // Will be opened after reading parameters

  // Verify parameter file exists
  if (parameter == NULL) {
    printf("Error: ../parameter.txt file not found.\n");
    printf("Make sure parameter.txt exists in the project root directory.\n");
    return 0;
  }

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
  int output_interval; // Save data every N timesteps
  int N_thread;       // Number of OpenMP threads
  int use_binary;     // Output format: 1 for binary, 0 for CSV

  // Read parameters from tab-separated file
  fscanf(parameter, "%lf\t%lf\t%d\t%lf\t%lf\t%lf\t%lf\t%lf\t%d\t%d\t%d\t%d\n", \
    &epsilon, &delta, &Particles, &Dt, &De, &vs, &Wall, &height, &N, &output_interval, &N_thread, &use_binary);
  fclose(parameter);
  
  // Set number of OpenMP threads for parallelization
  omp_set_num_threads(N_thread);
  
  // Echo parameters to console for verification
  printf("Simulation parameters:\n");
  printf("epsilon=%lf delta=%lf Particles=%d Dt=%lf De=%lf vs=%lf Wall=%lf height=%lf N=%d output_interval=%d N_thread=%d use_binary=%d\n", \
    epsilon, delta, Particles, Dt, De, vs, Wall, height, N, output_interval, N_thread, use_binary);

  // Validate parameters
  if (epsilon < 0.0) {
    fprintf(stderr, "Error: epsilon must be non-negative (got %lf)\n", epsilon);
    return EXIT_FAILURE;
  }
  if (delta <= 0.0) {
    fprintf(stderr, "Error: delta (timestep) must be positive (got %lf)\n", delta);
    return EXIT_FAILURE;
  }
  if (Particles <= 0) {
    fprintf(stderr, "Error: Number of particles must be positive (got %d)\n", Particles);
    return EXIT_FAILURE;
  }
  if (Particles > 10000) {
    fprintf(stderr, "Warning: Large number of particles (%d) may cause slow performance\n", Particles);
  }
  if (Dt < 0.0) {
    fprintf(stderr, "Error: Translational diffusion Dt must be non-negative (got %lf)\n", Dt);
    return EXIT_FAILURE;
  }
  if (De < 0.0) {
    fprintf(stderr, "Error: Rotational diffusion De must be non-negative (got %lf)\n", De);
    return EXIT_FAILURE;
  }
  if (vs < 0.0) {
    fprintf(stderr, "Error: Self-propulsion velocity vs must be non-negative (got %lf)\n", vs);
    return EXIT_FAILURE;
  }
  if (Wall <= 0.0) {
    fprintf(stderr, "Error: Cylinder radius (Wall) must be positive (got %lf)\n", Wall);
    return EXIT_FAILURE;
  }
  if (height <= 0.0) {
    fprintf(stderr, "Error: Cylinder height must be positive (got %lf)\n", height);
    return EXIT_FAILURE;
  }
  if (N <= 0) {
    fprintf(stderr, "Error: Number of iterations must be positive (got %d)\n", N);
    return EXIT_FAILURE;
  }
  if (output_interval <= 0) {
    fprintf(stderr, "Error: Output interval must be positive (got %d)\n", output_interval);
    return EXIT_FAILURE;
  }
  if (output_interval > N) {
    fprintf(stderr, "Warning: Output interval (%d) is larger than total iterations (%d)\n", output_interval, N);
  }
  if (use_binary != 0 && use_binary != 1) {
    fprintf(stderr, "Error: use_binary must be 0 (CSV) or 1 (binary) (got %d)\n", use_binary);
    return EXIT_FAILURE;
  }

  // Open output file based on format selection
  if (use_binary) {
    datafile = fopen("../data/simulation.bin", "wb");
  } else {
    datafile = fopen("../data/simulation.csv", "w");
  }

  // Check if output file was created successfully
  if (datafile == NULL) {
    fprintf(stderr, "Error: Cannot create output file ../data/simulation.%s\n", use_binary ? "bin" : "csv");
    fprintf(stderr, "Make sure the data/ directory exists in the project root.\n");
    return EXIT_FAILURE;
  }

  printf("Parameters validated successfully.\n");
  printf("Output format: %s\n", use_binary ? "Binary" : "CSV");

  // Allocate memory for particle positions and orientations using std::vector
  // Modern C++ approach - automatic memory management, no manual free() needed
  vector<double> x(Particles);
  vector<double> y(Particles);
  vector<double> z(Particles);
  
  vector<double> ex(Particles);
  vector<double> ey(Particles);
  vector<double> ez(Particles);

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

  // Write CSV header (only for CSV format)
  if (!use_binary) {
    fprintf(datafile, "Particles,x-position,y-position,z-position, "\
      "ex-orientation,ey-orientation,ez-orientation,time\n");
  } else {
    // Write binary header: number of particles and total timesteps
    int num_frames = (N + output_interval - 1) / output_interval;
    fwrite(&Particles, sizeof(int), 1, datafile);
    fwrite(&num_frames, sizeof(int), 1, datafile);
  }

  // Initialize particle positions and orientations randomly within the cylinder
  initialization(
    x.data(), y.data(), z.data(), ex.data(), ey.data(), ez.data(), Particles,
    generator, distribution, distribution_e);

  // Verify no particles overlap initially (enforce minimum separation)
  check_nooverlap(
    x.data(), y.data(), z.data(), Particles, L,
    generator, distribution);
  printf("Initialization complete. Starting simulation...\n");

  // Main simulation loop: integrate equations of motion
  for (int time = 0; time < N; time++) {
    // Update particle positions and orientations using Euler-Maruyama scheme
    update_position(
      x.data(), y.data(), z.data(), ex.data(), ey.data(), ez.data(),
      prefactor_e, Particles,
      delta, De, Dt, xi_ex, xi_ey, xi_ez, xi_px,
      xi_py, xi_pz, vs, prefactor_xi_px, prefactor_xi_py, prefactor_xi_pz,
      r, prefactor_interaction,
      generator, Gaussdistribution, distribution_e);

    // Apply reflective boundary conditions at cylindrical walls
    cylindrical_reflective_boundary_conditions(
      x.data(), y.data(), z.data(), Particles,
      Wall, height, L);

    // Save particle states at specified intervals to reduce file size
    if (time % output_interval == 0) {
      print_file(
        x.data(), y.data(), z.data(), ex.data(), ey.data(), ez.data(),
        Particles, time,
        datafile, use_binary);
      }
    }

  // Calculate and display total execution time
  ftime = omp_get_wtime();
  exec_time = ftime - itime;
  printf("Simulation complete. Time taken: %.3f seconds\n", exec_time);

  // Vectors automatically deallocate memory when going out of scope
  // No need for manual memory management!

  // Close output file
  fclose(datafile);

  printf("Results saved to ../data/simulation.%s\n", use_binary ? "bin" : "csv");
  return 0;
}
