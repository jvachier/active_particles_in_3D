#include <iostream>
#include <random>
#include <cstring>
#include <stdio.h>
#include <cmath>
#include <time.h>
//#include <omp.h> //import library to use pragma

void cylindrical_reflective_boundary_conditions(
	double *x, double *y, double *z, int Particles,
	double Wall, double height, int L
);