#include "reflective_boundary_conditions.h"

using namespace std;

void reflective_boundary_conditions(
	double *x, double *y, int Particles,
	double Wall, int L)
{
	double D_AW_x = 0.0;
	double D_AW_y = 0.0;
	double Wall_L = Wall - L/2.0;
#pragma omp parallel for simd num_threads(N_thread)
	for (int k = 0; k < Particles; k++)
	{
		D_AW_x = 0.0;
		D_AW_y = 0.0;
		if (abs(x[k]) > Wall_L)
		{
			D_AW_x = abs(x[k] + Wall);

			if (D_AW_x > 4.0 * L)
			{
				if (x[k] > Wall_L)
				{
					x[k] = Wall - 2.0 * L;
				}
				else if (x[k] < -Wall_L)
				{
					x[k] = 2.0 * L - Wall;
				}
			}
			else
			{
				if (x[k] > Wall_L)
				{
					x[k] -= 2.0 * D_AW_x;
				}
				else if (x[k] < -Wall_L)
				{
					x[k] += 2.0 * D_AW_x;
				}
			}
		}
		if (abs(y[k]) > Wall_L)
		{
			D_AW_y = abs(y[k] + Wall);
			if (D_AW_y > 4.0 * L)
			{
				if (y[k] > Wall_L)
				{
					y[k] = Wall - 2.0 * L;
				}
				else if (y[k] < -Wall_L)
				{
					y[k] = 2.0 * L - Wall;
				}
			}
			else
			{
				if (y[k] > Wall_L)
				{
					y[k] -= 2.0 * D_AW_y;
				}
				else if (y[k] < -Wall_L)
				{
					y[k] += 2.0 * D_AW_y;
				}
			}
		}
	}
}
