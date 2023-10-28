/*
 * Author: Jeremy Vachier
 * Purpose: ABP 3D confine in a square using an Euler-Mayurama algorithm
 * Language: C++
 * Date: 2023
 * Compilation line to use pragma: g++ name.cpp -fopenmp -o name.o (on mac run g++-12 ; 12 latest version obtain using brew list gcc)
 * Compilation line to use pragma, simd (vectorization) and tuple: g++ -O3 -std=c++17 name.cpp -fopenmp -o name.o
 */

#include <iostream>
#include <random>
#include <cstring>
#include <stdio.h>
#include <cmath>
#include <time.h>
#include <omp.h> //import library to use pragma
#include <tuple> //to output multiple components of a function

#include "print_file.h"
#include "reflective_boundary_conditions.h"
#include "circular_reflective_boundary_conditions.h"
#include "initialization.h"
#include "update_position.h"
#include "check_nooverlap.h"

#define PI 3.141592653589793
#define N_thread 6

using namespace std;

int main(int argc, char *argv[])
{
	// File
	FILE *datacsv;
	FILE *parameter;
	parameter = fopen("parameter.txt", "r");
	datacsv = fopen("./data/simulation.csv", "w");

	// check if the file parameter is exist
	if (parameter == NULL)
	{
		printf("no such file.");
		return 0;
	}

	// read the parameters from the file
	double epsilon, delta, Dt, De, vs;
	double F, R, Wall;
	int Particles;
	char name[100];
	char key1[] = "circular";
	char key2[] = "squared";

	bool flag = false;

	printf("Select confinement geometry, either squared or circular:");
	scanf("%s", &name);

	while (flag == false)
	{
		if ((strcmp(name, key1) == 0) or (strcmp(name, key2) == 0))
		{
			flag = true;
		}
		else
		{
			printf("You have not selected the correct, please select again\n");
			printf("Select confinement geometry, either squared or circular:");
			scanf("%s", &name);
			flag = false;
		}
	}
	fscanf(parameter, "%lf\t%lf\t%d\t%lf\t%lf\t%lf\t%lf\n", &epsilon, &delta, &Particles, &Dt, &De, &vs, &Wall);
	printf("%lf\t%lf\t%d\t%lf\t%lf\t%lf\t%lf\n", epsilon, delta, Particles, Dt, De, vs, Wall);

	double *x = (double *)malloc(Particles * sizeof(double)); // x-position
	double *y = (double *)malloc(Particles * sizeof(double)); // y-position

	// parameters
	const int N = 1E6; // number of iterations
	const int L = 1.0; // particle size

	// initialization of the random generator
	random_device rdev;
	default_random_engine generator(rdev()); // random seed -> rdev
	// default_random_engine generator(1); //same seed

	// Distributions Gaussian
	normal_distribution<double> Gaussdistribution(0.0, 1.0);
	// Distribution Uniform for initialization
	uniform_real_distribution<double> distribution(-Wall, Wall);
	// uniform_real_distribution<double> distribution_e(0.0,360.0*PI / 180.0); // directly in radian
	uniform_real_distribution<double> distribution_e(0.0, 360.0);

	double xi_px; // noise for x-position
	double xi_py; // noise for y-position
	double xi_e;  // noise ortientation
	double x_x;	  // used to initialize
	double y_y;	  // used to initialize
	int i, j, k;

	double phi = 0.0;
	double prefactor_e = sqrt(2.0 * delta * De);
	double prefactor_xi_px = sqrt(2.0 * delta * Dt);
	double prefactor_xi_py = sqrt(2.0 * delta * Dt);
	double prefactor_interaction = epsilon * 48.0;
	double r = 5.0 * L;

	clock_t tStart = clock(); // check time for one trajectory

	fprintf(datacsv, "Particles,x-position,y-position,time,%s\n", name);

	// initialization position and activity
	initialization(
		x, y, Particles,
		generator, distribution);

	check_nooverlap(
		x, y, Particles,
		R, L,
		generator, distribution);
	printf("Initialization done.\n");

	// Time evoultion
	int time;
	for (time = 0; time < N; time++)
	{
		update_position(
			x, y, phi, prefactor_e, Particles,
			delta, De, Dt, xi_e, xi_px,
			xi_py, vs, prefactor_xi_px, prefactor_xi_py,
			r, R, F, prefactor_interaction,
			generator, Gaussdistribution, distribution_e);
		if (strcmp(name, key1) == 0)
		{
			circular_reflective_boundary_conditions(
				x, y, Particles,
				Wall, L);
		}
		if (strcmp(name, key2) == 0)
		{
			reflective_boundary_conditions(
				x, y, Particles,
				Wall, L);
		}

		if (time % 100 == 0 && time >= 0)
		{
			print_file(
				x, y,
				Particles, time,
				datacsv);
		}
	}

	printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC); // time for one trajectory

	free(x);
	free(y);

	fclose(datacsv);
	return 0;
}
