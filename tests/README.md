# Unit Tests

This directory contains unit tests for the Active Particles 3D simulation.

## Test Files

- `test_boundary_conditions.cpp` - Tests for cylindrical reflective boundary conditions
- `test_initialization.cpp` - Tests for particle initialization and overlap detection
- `Makefile` - Build system for compiling and running tests

**Note:** Tests compile against the `cpu_openmp/` implementation since it's cross-platform compatible.

## Building Tests

```bash
cd tests
make
```

This will compile all test executables using the CPU OpenMP implementation.

## Running Tests

### Run all tests:
```bash
make test
```

### Run specific test suites:
```bash
make test-boundary     # Boundary condition tests only
make test-init         # Initialization tests only
```

### Run individual executables:
```bash
./test_boundary_conditions.out
./test_initialization.out
```

## Test Coverage

### Boundary Conditions Tests
- Radial boundary enforcement (particle projection onto cylinder surface)
- Axial boundary enforcement (z-direction reflections)
- Combined radial and axial boundary handling
- Edge cases (particle at origin, small cylinders, extreme positions)

### Initialization Tests
- Orientation vector normalization (all vectors have unit length)
- Position initialization within specified bounds
- Overlap detection and resolution
- Orientation vector properties (finite values, variation)
- Edge cases (single particle, small particle counts)

## Test Results

Tests output:
- `[PASS]` for successful tests
- `[FAIL]` for failed tests
- Summary with total passed/failed count

Exit codes:
- `0` - All tests passed
- `1` - One or more tests failed

## Adding New Tests

1. Create a new test file `test_yourfeature.cpp`
2. Include necessary headers from `../cpu_openmp/headers/`
3. Write test functions using the `assert_test()` helper
4. Add compilation rule to `Makefile`
5. Update this README
6. Add test to CI/CD pipeline in `.github/workflows/ci.yml`

## Dependencies

- C++17 compiler (g++-14 or compatible)
- OpenMP support
- Source files from `../cpu_openmp/`

## CI/CD Integration

Tests are automatically run in the CI/CD pipeline on:
- Every push to `main` and feature branches
- Pull requests to `main`
- Manual workflow dispatch

See `.github/workflows/ci.yml` for pipeline configuration.

## Example Test Function

```cpp
void test_my_feature() {
    cout << "\n=== Testing My Feature ===" << endl;
    
    // Setup
    double value = my_function(input);
    
    // Assert
    assert_test(value == expected, "My feature works correctly");
}
```

## Cleaning

Remove compiled test files:
```bash
make clean
```
