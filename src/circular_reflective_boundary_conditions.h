#include <iostream>
#include <random>
#include <cstring>
#include <stdio.h>
#include <cmath>
#include <time.h>
#include <omp.h> //import library to use pragma

void circular_reflective_boundary_conditions(
	double *x, double *y, int Particles,
	double Wall, int L
);