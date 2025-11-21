# Unit Tests Makefile
# Compile and run tests for the Active Particles 3D simulation

CXX = g++-14
CXXFLAGS = -std=c++17 -Wall -g
OPENMP_FLAGS = -fopenmp -fopenmp-simd
SRC_DIR = ../src
TEST_DIR = .

# Test executables
TEST_BOUNDARY = test_boundary_conditions.out
TEST_INIT = test_initialization.out

# Color output
GREEN = \033[0;32m
RED = \033[0;31m
NC = \033[0m # No Color

.PHONY: all clean test test-boundary test-init help

# Default target: compile all tests
all: $(TEST_BOUNDARY) $(TEST_INIT)
	@echo "$(GREEN)All tests compiled successfully!$(NC)"

# Compile boundary conditions test
$(TEST_BOUNDARY): test_boundary_conditions.cpp $(SRC_DIR)/cylindrical_reflective_boundary_conditions.cpp
	@echo "Compiling boundary conditions test..."
	$(CXX) $(CXXFLAGS) $(OPENMP_FLAGS) -I$(SRC_DIR) \
		test_boundary_conditions.cpp \
		$(SRC_DIR)/cylindrical_reflective_boundary_conditions.cpp \
		-o $(TEST_BOUNDARY)

# Compile initialization test
$(TEST_INIT): test_initialization.cpp $(SRC_DIR)/initialization.cpp $(SRC_DIR)/check_nooverlap.cpp
	@echo "Compiling initialization test..."
	$(CXX) $(CXXFLAGS) $(OPENMP_FLAGS) -I$(SRC_DIR) \
		test_initialization.cpp \
		$(SRC_DIR)/initialization.cpp \
		$(SRC_DIR)/check_nooverlap.cpp \
		-o $(TEST_INIT)

# Run all tests
test: all
	@echo ""
	@echo "======================================"
	@echo "Running All Tests"
	@echo "======================================"
	@echo ""
	@./$(TEST_BOUNDARY) && echo "$(GREEN)Boundary tests passed$(NC)" || echo "$(RED)Boundary tests failed$(NC)"
	@echo ""
	@./$(TEST_INIT) && echo "$(GREEN)Initialization tests passed$(NC)" || echo "$(RED)Initialization tests failed$(NC)"
	@echo ""

# Run individual test suites
test-boundary: $(TEST_BOUNDARY)
	@echo "Running boundary condition tests..."
	@./$(TEST_BOUNDARY)

test-init: $(TEST_INIT)
	@echo "Running initialization tests..."
	@./$(TEST_INIT)

# Clean compiled tests
clean:
	@echo "Cleaning test executables..."
	@rm -f $(TEST_BOUNDARY) $(TEST_INIT)
	@rm -f *.o
	@echo "$(GREEN)Clean complete!$(NC)"

# Help message
help:
	@echo "Available targets:"
	@echo "  all          - Compile all tests (default)"
	@echo "  test         - Compile and run all tests"
	@echo "  test-boundary- Run boundary condition tests only"
	@echo "  test-init    - Run initialization tests only"
	@echo "  clean        - Remove compiled test executables"
	@echo "  help         - Show this help message"
