#include "update_position.h"

using namespace std;


kernel void add_arrays(device double* ex,
                       device double* ey,
                       device double* ez,
					   device double xi_ex, xi_ey, xi_ez, prefactor_e,
					   device default_random_engine &generator, device uniform_real_distribution<double> &distribution_e
                       uint index [[thread_position_in_grid]])
{
	double norm_e = 0.0, invers_norm_e = 0.0;

	xi_ex = distribution_e(generator);
	xi_ey = distribution_e(generator);
	xi_ez = distribution_e(generator);

// Ito formulation
	ex[index] = prefactor_e * ( ey[index]*xi_ez - xi_ez*ez[index])  - ex[index]; 
	ey[index] = prefactor_e * ( ex[index]*xi_ez - xi_ex*ez[index])  - ey[index];
	ez[index] = prefactor_e * ( ex[index]*xi_ey - xi_ex*ey[index])  - ez[index];

	// Need to normalize the orientaional vector
	norm_e = sqrt(ex[index]*ex[index] + ey[index]*ey[index] + ez[index]*ez[index]);
	invers_norm_e = 1.0 / norm_e;

	ex[index] = ex[index] * invers_norm_e;
	ey[index] = ey[index] * invers_norm_e;
	ez[index] = ez[index] * invers_norm_e;

}

void update_position(
	double *x, double *y, double *z, double *ex, double *ey, double *ez, double prefactor_e, int Particles,
	double delta, double De, double Dt, double xi_ex, double xi_ey, double xi_ez, double xi_px,
	double xi_py, double xi_pz, double vs, double prefactor_xi_px, double prefactor_xi_py, double prefactor_xi_pz,
	double r, double prefactor_interaction,
	default_random_engine &generator, normal_distribution<double> &Gaussdistribution, uniform_real_distribution<double> &distribution_e)
{
	double a = 0.0; // local variable - here check if no conflict elsewhere
	double F = 0.0, R = 0.0;

// First orientation

// Second position
#pragma omp parallel for simd 
	for (int k = 0; k < Particles; k++)
	{

		xi_px = Gaussdistribution(generator);
		xi_py = Gaussdistribution(generator);
		xi_pz = Gaussdistribution(generator);

		F = 0.0;
		for (int j = 0; j < Particles; j++)
		{
			if (k != j) // see how to improved the nested if conditions
			{
				R = sqrt((x[j] - x[k]) * (x[j] - x[k]) + (y[j] - y[k]) * (y[j] - y[k]) + (z[j] - z[k]) * (z[j] - z[k]));
				if (R < r)
				{
					a = prefactor_interaction / pow(R, 14);
					if (a > 1.0)
					{
						a = 1.0; // this value needs to be checked
					}
					F += a;
				}
			}
		}
		x[k] = x[k] + vs * ex[k] * delta + F * x[k] * delta + xi_px * prefactor_xi_px;
		y[k] = y[k] + vs * ey[k] * delta + F * y[k] * delta + xi_py * prefactor_xi_py;
		z[k] = z[k] + vs * ez[k] * delta + F * z[k] * delta + xi_pz * prefactor_xi_pz; 
	}
}