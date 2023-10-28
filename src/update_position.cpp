#include "update_position.h"

using namespace std;

void update_position(
	double *x, double *y, double *z, double *ex, double *ey, double *ez, double prefactor_e, int Particles,
	double delta, double De, double Dt, double xi_ex, double xi_ey, double xi_ez, double xi_px,
	double xi_py, double xi_pz, double vs, double prefactor_xi_px, double prefactor_xi_py, double prefactor_xi_pz,
	double r, double R, double F, double prefactor_interaction,
	default_random_engine &generator, normal_distribution<double> &Gaussdistribution, uniform_real_distribution<double> &distribution_e)
{
	double a = 0.0; // local variable - here check if no conflict elsewhere
	double norm_e = 0.0, invers_norm_e = 0.0;

// First orientation
//#pragma omp parallel for simd num_threads(N_thread)
for (int k = 0; k < Particles; k++)
	{
		xi_ex = distribution_e(generator);
		xi_ey = distribution_e(generator);
		xi_ez = distribution_e(generator);


// here need to initialize the orientation in initialization
		ex[k] = prefactor_e * ( ey[k]*xi_ez - xi_ez*ez[k])  - ex[k]; 
		ey[k] = prefactor_e * ( ex[k]*xi_ez - xi_ex*ez[k])  - ey[k];
		ez[k] = prefactor_e * ( ex[k]*xi_ey - xi_ex*ey[k])  - ez[k];

		// Need to normalize the orientaional vector
		norm_e = sqrt(ex[k]*ex[k] + ey[k]*ey[k] + ez[k]*ez[k]);
		invers_norm_e = 1.0 / norm_e;

		ex[k] = ex[k] * invers_norm_e;
		ey[k] = ey[k] * invers_norm_e;
		ez[k] = ez[k] * invers_norm_e;
	}


// Second position
//#pragma omp parallel for simd num_threads(N_thread)
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