# Use g++ by default, can be overridden with: make CC=g++-14
CC ?= g++
CXXFLAGS = -O3 -std=c++17 -Wall -g -fopenmp -fopenmp-simd

abp_3D_confine: abp_3D_confine.o print_file.o cylindrical_reflective_boundary_conditions.o initialization.o update_position.o check_nooverlap.o
	$(CC) $(CXXFLAGS) -o abp_3D_confine.out abp_3D_confine.o print_file.o cylindrical_reflective_boundary_conditions.o initialization.o update_position.o check_nooverlap.o

abp_3D_confine.o: abp_3D_confine.cpp
	$(CC) $(CXXFLAGS) -c abp_3D_confine.cpp

print_file.o: print_file.cpp
	$(CC) $(CXXFLAGS) -c print_file.cpp

cylindrical_reflective_boundary_conditions.o: cylindrical_reflective_boundary_conditions.cpp
	$(CC) $(CXXFLAGS) -c cylindrical_reflective_boundary_conditions.cpp

initialization.o: initialization.cpp
	$(CC) $(CXXFLAGS) -c initialization.cpp

update_position.o: update_position.cpp
	$(CC) $(CXXFLAGS) -c update_position.cpp

check_nooverlap.o: check_nooverlap.cpp
	$(CC) $(CXXFLAGS) -c check_nooverlap.cpp

clean:
	rm -f *.o abp_3D_confine.out
