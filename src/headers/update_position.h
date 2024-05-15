#include <iostream>
#include <random>
#include <cstring>
#include <stdio.h>
#include <cmath>
#include <time.h>
#include <omp.h>  // import library to use pragma

void update_position(
  double *x, double *y, double *z,
  double *ex, double *ey, double *ez,
  double prefactor_e, int Particles,
  double delta, double De, double Dt,
  double xi_ex, double xi_ey, double xi_ez, double xi_px,
  double xi_py, double xi_pz, double vs,
  double prefactor_xi_px, double prefactor_xi_py, double prefactor_xi_pz,
  double r, double prefactor_interaction,
  std::default_random_engine generator,
  std::normal_distribution<double> Gaussdistribution,
  std::uniform_real_distribution<double> distribution_e);
