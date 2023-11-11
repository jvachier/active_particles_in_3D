#include "cylindrical_reflective_boundary_conditions.h"

using namespace std;

void cylindrical_reflective_boundary_conditions(
	double *x, double *y, double *z, int Particles,
	double Wall, double height, int L)
{
	double distance_squared = 0.0, Wall_squared = Wall * Wall; 
	double height_L = height - L/2.0;
	double D_AW_z = 0.0;
#pragma omp parallel for simd 
	for (int k = 0; k < Particles; k++)
	{
		// x-y coordidnate circle
		distance_squared = x[k] * x[k] + y[k] * y[k];
		if (distance_squared > Wall_squared)
		{
			x[k] = (sqrt(Wall_squared) / sqrt(distance_squared)) * x[k];
			y[k] = (sqrt(Wall_squared) / sqrt(distance_squared)) * y[k];
		}

		// z coordinate 
		D_AW_z = 0.0;
		if (abs(z[k]) > height_L)
		{
			D_AW_z = abs(z[k] + height);

			if (D_AW_z > 4.0 * L)
			{
				if (z[k] > height_L)
				{
					z[k] = height - 2.0 * L;
				}
				else if (z[k] < -height_L)
				{
					z[k] = 2.0 * L - height;
				}
			}
			else
			{
				if (z[k] > height_L)
				{
					z[k] -= 2.0 * D_AW_z;
				}
				else if (z[k] < -height_L)
				{
					z[k] += 2.0 * D_AW_z;
				}
			}
		}
	}

}