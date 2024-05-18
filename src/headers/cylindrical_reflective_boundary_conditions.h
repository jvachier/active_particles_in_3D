#include <time.h>
#include <stdio.h>
#include <omp.h>  // import library to use pragma
#include <iostream>
#include <random>
#include <cstring>
#include <cmath>

void cylindrical_reflective_boundary_conditions(
  double *x, double *y, double *z, int Particles,
  double Wall, double height, int L
);
