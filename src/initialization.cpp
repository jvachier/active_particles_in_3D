#include "initialization.h"

using namespace std;

void initialization(
	double *x, double *y, double *z, double *ex, double *ey, double *ez, int Particles,
	default_random_engine &generator, uniform_real_distribution<double> &distribution, uniform_real_distribution<double> &distribution_e)
{
	double norm_e = 0.0;
// Orientation
//#pragma omp parallel for simd num_threads(N_thread)
	for (int k = 0; k < Particles; k++)
	{
		ex[k] = distribution(generator);
		ey[k] = distribution(generator);
		ez[k] = distribution(generator);

		// Need to normalize the orientaional vector
		norm_e = sqrt(ex[k]*ex[k] + ey[k]*ey[k] + ez[k]*ez[k]);

		ex[k] = norm_e * ex[k];
		ey[k] = norm_e * ey[k];
		ez[k] = norm_e * ez[k];
	}

// Position
//#pragma omp parallel for simd num_threads(N_thread)
	for (int k = 0; k < Particles; k++)
	{
		x[k] = distribution(generator);
		y[k] = distribution(generator);
		z[k] = distribution(generator);
	}
}