#include <time.h>
#include <stdio.h>
#include <omp.h>  // import library to use pragma
#include <iostream>
#include <random>
#include <cstring>
#include <cmath>


void initialization(
  double *x, double *y, double *z,
  double *ex, double *ey, double *ez,
  int Particles,
  std::default_random_engine &generator,
  std::uniform_real_distribution<double> &distribution,
  std::uniform_real_distribution<double> &distribution_e);
