#include "circular_reflective_boundary_conditions.h"

using namespace std;

void circular_reflective_boundary_conditions(
	double *x, double *y, int Particles,
	double Wall, int L)
{
	double distance_squared = 0.0, Wall_squared = Wall * Wall; 
#pragma omp parallel for simd num_threads(N_thread)
	for (int k = 0; k < Particles; k++)
	{
		distance_squared = x[k] * x[k] + y[k] * y[k];
		if (distance_squared > Wall_squared)
		{
			x[k] = (sqrt(Wall_squared) / sqrt(distance_squared)) * x[k];
			y[k] = (sqrt(Wall_squared) / sqrt(distance_squared)) * y[k];
		}
	}
}