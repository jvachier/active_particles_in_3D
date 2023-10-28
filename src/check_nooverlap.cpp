#include "check_nooverlap.h"

using namespace std;

void check_nooverlap(
	double *x, double *y, double *z, int Particles,
	double R, int L,
	default_random_engine &generator, uniform_real_distribution<double> &distribution)
{
	int count = 0;
#pragma omp parallel for num_threads(N_thread)
	for (int k = 0; k < Particles; k++)
	{
		for (int j = 0; j < Particles; j++)
		{
			if (k != j)
			{
				R = sqrt((x[j] - x[k]) * (x[j] - x[k]) + (y[j] - y[k]) * (y[j] - y[k]) + (z[j] - z[k]) * (z[j] - z[k]));
				count = 0;
				while (R < 1.5 * L)
				{
					x[j] = distribution(generator);
					y[j] = distribution(generator);
					R = sqrt((x[j] - x[k]) * (x[j] - x[k]) + (y[j] - y[k]) * (y[j] - y[k]) + (z[j] - z[k]) * (z[j] - z[k]));
					count += 1;
					if (count > 3)
					{
						printf("Number of particle too high\n");
						exit(0);
					}
				}
			}
		}
	}
}