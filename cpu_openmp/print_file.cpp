/**
 * @file print_file.cpp
 * @brief Implementation of data output functions
 * @author Jeremy Vachier
 * @date 2023
 */

#include "headers/print_file.h"

using namespace std;

/**
 * @brief Write current state of all particles to CSV or binary file
 *
 * Iterates through all particles and writes their state vectors
 * (position and orientation) to the output file.
 *
 * CSV format: Each row contains: ParticleID, x, y, z, ex, ey, ez, timestep
 * Binary format: Timestep followed by raw double arrays for all particles
 */
void print_file(
  double *x, double *y, double *z, double *ex, double *ey, double *ez,
  int Particles, int time,
  FILE *datafile, int use_binary) {

  if (use_binary) {
    // Binary format: write timestep, then all data as raw doubles
    fwrite(&time, sizeof(int), 1, datafile);
    fwrite(x, sizeof(double), Particles, datafile);
    fwrite(y, sizeof(double), Particles, datafile);
    fwrite(z, sizeof(double), Particles, datafile);
    fwrite(ex, sizeof(double), Particles, datafile);
    fwrite(ey, sizeof(double), Particles, datafile);
    fwrite(ez, sizeof(double), Particles, datafile);
  } else {
    // CSV format: write data for each particle
    for (int k = 0; k < Particles; k++) {
      fprintf(datafile, "Particles%d,%lf,%lf,%lf,%lf,%lf,%lf,%d\n", \
        k, x[k], y[k], z[k], ex[k], ey[k], ez[k], time);
    }
  }
}
