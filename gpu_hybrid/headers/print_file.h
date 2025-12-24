/**
 * @file print_file.h
 * @brief Header file for data output functions
 * @author Jeremy Vachier
 * @date 2023
 */

#ifndef PRINT_FILE_H
#define PRINT_FILE_H

#include <time.h>
#include <stdio.h>
#include <iostream>
#include <random>
#include <cstring>
#include <cmath>

/**
 * @brief Write particle data to CSV or binary file
 *
 * @details
 * Outputs the current state of all particles (positions and orientations).
 *
 * CSV format: Each particle's data is written as a single row with format:
 * Particles{ID},{x},{y},{z},{ex},{ey},{ez},{time}
 *
 * Binary format: Data is written as raw doubles in sequence for all particles:
 * [timestep][x0,y0,z0,ex0,ey0,ez0][x1,y1,z1,ex1,ey1,ez1]...
 *
 * This function is typically called at regular intervals during simulation to
 * create a time-series dataset for analysis and visualization.
 *
 * @param[in] x Array of x-coordinates for all particles
 * @param[in] y Array of y-coordinates for all particles
 * @param[in] z Array of z-coordinates for all particles
 * @param[in] ex Array of x-components of orientation unit vectors
 * @param[in] ey Array of y-components of orientation unit vectors
 * @param[in] ez Array of z-components of orientation unit vectors
 * @param[in] Particles Total number of particles
 * @param[in] time Current simulation timestep
 * @param[in,out] datafile File pointer to output file
 * @param[in] use_binary 1 for binary format, 0 for CSV format
 *
 * @note File must be opened before calling this function
 * @note For binary format, file must be opened with "wb" mode
 * @note For CSV format, file must be opened with "w" mode
 */
void print_file(
  double *x, double *y, double *z, double *ex, double *ey, double *ez,
  int Particles, int time,
  FILE *datafile, int use_binary);

#endif  // PRINT_FILE_H
