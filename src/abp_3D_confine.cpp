/*
 * Author: Jeremy Vachier
 * Purpose: ABP 3D confine in a square using an Euler-Mayurama algorithm
 * Language: C++
 * Date: 2023
 * Compilation line to use pragma: g++ name.cpp -fopenmp -o name.o (on mac run g++-13 ; 13 latest version obtain using brew list gcc)
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

#include "headers/print_file.h"
#include "headers/cylindrical_reflective_boundary_conditions.h"
#include "headers/initialization.h"
#include "headers/update_position.h"
#include "headers/check_nooverlap.h"

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

	omp_set_num_threads(N_thread);

	// read the parameters from the file
	double epsilon, delta, Dt, De, vs;
	double Wall, height;
	int Particles;
	int N; // number of iterations

	fscanf(parameter, "%lf\t%lf\t%d\t%lf\t%lf\t%lf\t%lf\t%lf\t%d\n", &epsilon, &delta, &Particles, &Dt, &De, &vs, &Wall, &height, &N);
	printf("%lf\t%lf\t%d\t%lf\t%lf\t%lf\t%lf\t%lf\t%d\n", epsilon, delta, Particles, Dt, De, vs, Wall, height, N);

    // Position
	double *x = (double *)malloc(Particles * sizeof(double)); // x-position
	double *y = (double *)malloc(Particles * sizeof(double)); // y-position
    double *z = (double *)malloc(Particles * sizeof(double)); // z-position  

    // Orientation
    double *ex = (double *)malloc(Particles * sizeof(double)); // ex-orientation
	double *ey = (double *)malloc(Particles * sizeof(double)); // ey-orientation
    double *ez = (double *)malloc(Particles * sizeof(double)); // ez-orientation 

	// parameters
	const int L = 1.0; // particle size

	// initialization of the random generator
	random_device rdev;
	default_random_engine generator(rdev()); // random seed -> rdev
	// default_random_engine generator(1); //same seed

	// Distributions Gaussian
	normal_distribution<double> Gaussdistribution(0.0, 1.0);
	// Distribution Uniform for initialization
	uniform_real_distribution<double> distribution(-Wall, Wall);
    // Uniform distribution for the orientation - later on maybe take it from the unit sphere but normalized in update position
	uniform_real_distribution<double> distribution_e(0.0, 1.0);

	double xi_px = 0.0; // noise for x-position
	double xi_py = 0.0; // noise for y-position 
    double xi_pz = 0.0; // noise for z-position
	double xi_ex = 0.0;  // noise ex ortientation
    double xi_ey = 0.0;  // noise ey ortientation
    double xi_ez = 0.0;  // noise ez ortientation

	// double phi = 0.0;
	double prefactor_e = sqrt(2.0 * delta * De);
	double prefactor_xi_px = sqrt(2.0 * delta * Dt);
	double prefactor_xi_py = sqrt(2.0 * delta * Dt);
    double prefactor_xi_pz = sqrt(2.0 * delta * Dt);
	double prefactor_interaction = epsilon * 48.0;
	double r = 5.0 * L;

	/* does not work when using openmp
	clock_t tStart = clock(); // check time for one trajectory 
	*/

	// Open MP to get execution time
	double itime, ftime, exec_time;
    itime = omp_get_wtime(); 
    
    fprintf(datacsv, "Particles,x-position,y-position,z-position,ex-orientation,ey-orientation,ez-orientation,time\n");

	// initialization position and activity
	initialization(
		x, y, z, ex, ey, ez, Particles,
		generator, distribution, distribution_e);

	check_nooverlap(
		x, y, z, Particles, L,
		generator, distribution);
	printf("Initialization done.\n");

	// Time evoultion
	for (int time = 0; time < N; time++)
	{
		update_position(
			x, y, z, ex, ey, ez, prefactor_e, Particles,
			delta, De, Dt, xi_ex, xi_ey, xi_ez, xi_px,
			xi_py, xi_pz, vs, prefactor_xi_px, prefactor_xi_py, prefactor_xi_pz, 
			r, prefactor_interaction,
			generator, Gaussdistribution, distribution_e);
		
		cylindrical_reflective_boundary_conditions(
			x, y, z, Particles,
			Wall, height, L);

		if (time % 10 == 0 && time >= 0)
		{
			print_file(
				x, y, z, ex, ey, ez,
				Particles, time,
				datacsv);
		}
	}

	/* Does not work when using openmp
	printf("Time taken: %.2fs\n", ((double)(clock() - tStart) / CLOCKS_PER_SEC)); 
	// printf("Time taken: %.2fs\n", ((double)(clock() - tStart) / CLOCKS_PER_SEC/N_thread));
	*/
	
	ftime = omp_get_wtime();
    exec_time = ftime - itime;
    printf("Time taken is %f", exec_time);

	free(x);
	free(y);
	free(z);
	free(ex);
	free(ey);
	free(ez);

	fclose(datacsv);
	return 0;
}
