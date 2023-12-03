#include "headers/initialization.h"

using namespace std;

void initialization(
	double *x, double *y, double *z, double *ex, double *ey, double *ez, int Particles,
	default_random_engine &generator, uniform_real_distribution<double> &distribution, uniform_real_distribution<double> &distribution_e)
{
	double norm_e = 0.0, invers_norm_e = 0.0;
// Orientation
#pragma omp parallel for simd
	for (int k = 0; k < Particles; k++)
	{
		ex[k] = distribution_e(generator);
		ey[k] = distribution_e(generator);
		ez[k] = distribution_e(generator);

		// Need to normalize the orientaional vector
		norm_e = sqrt(ex[k] * ex[k] + ey[k] * ey[k] + ez[k] * ez[k]);
		invers_norm_e = 1.0 / norm_e;

		ex[k] = ex[k] * invers_norm_e;
		ey[k] = ey[k] * invers_norm_e;
		ez[k] = ez[k] * invers_norm_e;
	}

// Position
#pragma omp parallel for simd
	for (int k = 0; k < Particles; k++)
	{
		x[k] = distribution(generator);
		y[k] = distribution(generator);
		z[k] = distribution(generator);
	}
}