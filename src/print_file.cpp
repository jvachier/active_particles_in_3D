#include "print_file.h"

using namespace std;

void print_file(
	double *x, double *y, double *z, double *ex, double *ey, double *ez,
	int Particles, int time,
	FILE *datacsv)
{
	for (int k = 0; k < Particles; k++)
	{
		fprintf(datacsv, "Particles%d,%lf,%lf,%d\n", k, x[k], y[k], z[k], ex[k], ey[k], ez[k], time);
	}
}