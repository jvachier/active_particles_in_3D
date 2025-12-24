# Detect operating system for compiler selection
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	# Use clang++ on macOS (compatible with Xcode updates)
	# Note: clang doesn't support -fopenmp-simd, warnings about vectorization are normal
	CC = clang++ -O3 -std=c++17 -stdlib=libc++
	CFLAGS = -Wall -g -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include -Wno-pass-failed
	LDFLAGS = -L/opt/homebrew/opt/libomp/lib -lomp
else
	# Use g++ on Linux and other platforms (has full SIMD support)
	CC = g++ -O3 -std=c++17
	CFLAGS = -Wall -g -fopenmp -fopenmp-simd
	LDFLAGS =
endif

abp_3D_confine: abp_3D_confine.o print_file.o cylindrical_reflective_boundary_conditions.o initialization.o update_position.o check_nooverlap.o
	$(CC) $(CFLAGS) -o abp_3D_confine.out abp_3D_confine.o print_file.o cylindrical_reflective_boundary_conditions.o initialization.o update_position.o check_nooverlap.o $(LDFLAGS)

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

clean:
	rm -f *.o abp_3D_confine.out
