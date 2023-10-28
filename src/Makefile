CC = g++-13
CFLAGS = -Wall -g #-fopenmp

abp_2D_confine: abp_3D_confine.o print_file.o cylindrical_reflective_boundary_conditions.o initialization.o update_position.o check_nooverlap.o
	$(CC) $(CFLAGS) -o abp_3D_confine.out abp_3D_confine.o print_file.o cylindrical_reflective_boundary_conditions.o initialization.o update_position.o check_nooverlap.o

abp_2D_confine.o: abp_3D_confine.cpp
	$(CC) -c abp_3D_confine.cpp

print_file.o: print_file.cpp
	$(CC) -c print_file.cpp

#reflective_boundary_conditions.o: reflective_boundary_conditions.cpp
#	$(CC) -c reflective_boundary_conditions.cpp

cylindrical_reflective_boundary_conditions.o: cylindrical_reflective_boundary_conditions.cpp
	$(CC) -c cylindrical_reflective_boundary_conditions.cpp

initialization.o: initialization.cpp
	$(CC) -c initialization.cpp

update_position.o: update_position.cpp
	$(CC) -c update_position.cpp

check_nooverlap.o: check_nooverlap.cpp
	$(CC) -c check_nooverlap.cpp

clean:
	rm *.o