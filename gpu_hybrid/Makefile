# Detect macOS for Metal support
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	# Use clang++ on macOS for Metal compatibility
	CC = clang++ -O3 -std=c++17 -stdlib=libc++
	CFLAGS = -Wall -g -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include
	LDFLAGS = -L/opt/homebrew/opt/libomp/lib -lomp
	METAL_FLAGS = -framework Metal -framework Foundation
	METAL_OBJS = metal_compute.o
	OBJC_COMPILER = clang++ -x objective-c++ -O3 -std=c++17 -stdlib=libc++
else
	# Use g++ on other platforms (no Metal support)
	CC = g++-14 -O3 -std=c++17
	CFLAGS = -Wall -g -fopenmp -fopenmp-simd
	LDFLAGS =
	METAL_FLAGS =
	METAL_OBJS =
	OBJC_COMPILER =
endif

abp_3D_confine: abp_3D_confine.o print_file.o cylindrical_reflective_boundary_conditions.o initialization.o update_position.o update_position_vectorized.o compute_forces.o check_nooverlap.o $(METAL_OBJS)
	$(CC) $(CFLAGS) -o abp_3D_confine.out abp_3D_confine.o print_file.o cylindrical_reflective_boundary_conditions.o initialization.o update_position.o update_position_vectorized.o compute_forces.o check_nooverlap.o $(METAL_OBJS) $(LDFLAGS) $(METAL_FLAGS)

abp_3D_confine.o: abp_3D_confine.cpp
	$(CC) $(CFLAGS) -c abp_3D_confine.cpp

print_file.o: print_file.cpp
	$(CC) -c print_file.cpp

cylindrical_reflective_boundary_conditions.o: cylindrical_reflective_boundary_conditions.cpp
	$(CC) $(CFLAGS) -c cylindrical_reflective_boundary_conditions.cpp

initialization.o: initialization.cpp
	$(CC) $(CFLAGS) -c initialization.cpp

update_position.o: update_position.cpp
	$(CC) $(CFLAGS) -c update_position.cpp

check_nooverlap.o: check_nooverlap.cpp
	$(CC) $(CFLAGS) -c check_nooverlap.cpp

update_position_vectorized.o: update_position_vectorized.cpp
	$(CC) $(CFLAGS) -c update_position_vectorized.cpp

compute_forces.o: compute_forces.cpp
	$(CC) $(CFLAGS) -c compute_forces.cpp

# Metal support (macOS only, requires Objective-C++)
metal_compute.o: metal_compute.mm
ifeq ($(UNAME_S),Darwin)
	$(OBJC_COMPILER) -I/opt/homebrew/opt/libomp/include -c metal_compute.mm -o metal_compute.o
endif

clean:
	rm -f *.o abp_3D_confine.out