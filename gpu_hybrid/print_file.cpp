/**
 * @file print_file.cpp
 * @brief Implementation of data output functions
 * @author Jeremy Vachier
 * @date 2023
 */

#include "headers/print_file.h"

using namespace std;

/**
 * @brief Write current state of all particles to CSV file
 * 
 * Iterates through all particles and writes their state vectors
 * (position and orientation) to the output file in CSV format.
 * Each row contains: ParticleID, x, y, z, ex, ey, ez, timestep
 */
void print_file(
  double *x, double *y, double *z, double *ex, double *ey, double *ez,
  int Particles, int time,
  FILE *datacsv) {
  // Write data for each particle
  for (int k = 0; k < Particles; k++) {
    fprintf(datacsv, "Particles%d,%lf,%lf,%lf,%lf,%lf,%lf,%d\n", \
      k, x[k], y[k], z[k], ex[k], ey[k], ez[k], time);
  }
}
