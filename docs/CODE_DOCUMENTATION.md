# Code Documentation

## Overview

This document provides comprehensive technical documentation for the Active Brownian Particles 3D simulation codebase. The simulation implements a stochastic particle dynamics system confined within cylindrical boundaries.

## Architecture

### File Structure

```
src/
├── abp_3D_confine.cpp          # Main simulation entry point
├── initialization.cpp           # Particle initialization
├── update_position.cpp          # Time-stepping integration
├── cylindrical_reflective_boundary_conditions.cpp  # Boundary enforcement
├── check_nooverlap.cpp         # Overlap detection
├── print_file.cpp              # Data output
├── Makefile                    # Build configuration
└── headers/
    ├── initialization.h
    ├── update_position.h
    ├── cylindrical_reflective_boundary_conditions.h
    ├── check_nooverlap.h
    └── print_file.h
```

## Core Components

### 1. Main Simulation Loop (`abp_3D_confine.cpp`)

**Purpose**: Orchestrates the entire simulation workflow.

**Key Responsibilities**:
- Parameter loading from `parameter.txt`
- Memory allocation for particle arrays
- Random number generator initialization
- Main time-stepping loop
- Output file management
- Performance timing

**Data Structures**:
```cpp
double *x, *y, *z;        // Particle positions (3D coordinates)
double *ex, *ey, *ez;     // Particle orientations (unit vectors)
```

**Main Loop Flow**:
1. Initialize particles randomly
2. Check for overlaps
3. For each timestep:
   - Update positions and orientations
   - Apply boundary conditions
   - Write data (every 10 steps)
4. Report execution time

### 2. Initialization Module (`initialization.cpp`)

**Purpose**: Generate initial particle configurations.

**Algorithm**:
```
For each particle:
  1. Generate random orientation components (ex, ey, ez)
  2. Normalize to unit vector: e = e / |e|
  3. Generate random position (x, y, z)
```

**Parallelization**: Uses `#pragma omp parallel for simd`

**Mathematical Details**:
- Orientation uniformly distributed on unit sphere
- Positions uniformly distributed in cylindrical volume
- No correlation between position and orientation

### 3. Position Update Module (`update_position.cpp`)

**Purpose**: Integrate equations of motion for one timestep.

**Two-Stage Algorithm**:

#### Stage 1: Orientation Update
Implements rotational diffusion:
```
de/dt = sqrt(2*De*delta) * (e × xi_e)
```

**Implementation**:
- Uses Ito stochastic calculus
- Cross product with noise vector
- Normalization after update

#### Stage 2: Position Update
Implements active motion with interactions:
```
dr/dt = vs*e + F_interaction*r + sqrt(2*Dt*delta)*xi_p
```

**Force Calculation**:
- O(N²) pairwise interaction loop
- Lennard-Jones repulsive potential
- Cutoff radius: 5L
- Force capping: max value = 1.0

**Numerical Scheme**: Euler-Maruyama (first-order)

### 4. Boundary Conditions Module (`cylindrical_reflective_boundary_conditions.cpp`)

**Purpose**: Confine particles within cylindrical geometry.

#### Radial Confinement (x-y plane)
```cpp
if (r > Wall) {
    scale = Wall / r;
    x = scale * x;
    y = scale * y;
}
```
Projects particles onto cylinder surface while preserving angle.

#### Axial Confinement (z direction)
Two reflection modes:

**Small penetration** (D < 4L):
```cpp
z_new = z - 2 * penetration_distance
```

**Large penetration** (D ≥ 4L):
```cpp
z_new = boundary - 2*L
```

### 5. Overlap Detection Module (`check_nooverlap.cpp`)

**Purpose**: Prevent initial particle overlaps.

**Algorithm**:
```
For each particle pair (i, j):
  If distance(i, j) < 1.5*L:
    Reposition particle j randomly
    Retry up to 3 times
    If still overlapping: exit with error
```

**Minimum Separation**: 1.5 particle diameters

**Failure Mode**: Exits if density too high (>3 repositioning attempts)

### 6. Data Output Module (`print_file.cpp`)

**Purpose**: Write particle states to CSV.

**Output Format**:
```csv
Particles{ID},{x},{y},{z},{ex},{ey},{ez},{timestep}
```

**Frequency**: Every 10 timesteps (configurable in main loop)

## Mathematical Background

### Stochastic Differential Equations

The system evolves according to coupled Langevin equations:

**Position**:
```
dr = vs*e*dt - ∇U*dt + sqrt(2*Dt*dt)*dW_t
```

**Orientation**:
```
de = sqrt(2*De*dt) * (e × dW_e)
```

where:
- `dW_t, dW_e`: Wiener processes (Gaussian white noise)
- `U`: Interaction potential (Lennard-Jones)

### Numerical Integration

**Method**: Euler-Maruyama scheme

**Discretization**:
```
r(t+δt) = r(t) + [vs*e + F]*δt + sqrt(2*Dt*δt)*ξ
e(t+δt) = normalize(e(t) + sqrt(2*De*δt)*(e × ξ_e))
```

where `ξ ~ N(0,1)` is standard Gaussian noise.

**Stability**: Requires small timestep (typically `δt ~ 1e-4`)

### Interaction Potential

**Lennard-Jones** (repulsive part only):
```
U(r) = 4ε[(σ/r)^12 - (σ/r)^6]
F(r) = -∇U(r) = 48ε/r^14
```

**Cutoff**: `r_cut = 5σ` to reduce computational cost

## Performance Optimization

### Parallelization Strategy

**OpenMP Directives**:
```cpp
#pragma omp parallel for simd
```

**Benefits**:
- Thread-level parallelism across particles
- SIMD vectorization within threads
- Scales well up to ~8 threads

**Thread Count**: Set via `N_thread` macro (default: 6)

### Computational Complexity

| Operation | Complexity | Bottleneck |
|-----------|-----------|-----------|
| Initialization | O(N) | Low |
| Position update | O(N²) | **High** |
| Boundary conditions | O(N) | Low |
| File I/O | O(N) | Medium |

**Dominant Cost**: O(N²) interaction loop in `update_position()`

### Optimization Opportunities

1. **Neighbor Lists**: Reduce interaction complexity from O(N²) to O(N)
2. **Cell Lists**: Spatial partitioning for efficient neighbor search
3. **GPU Acceleration**: Offload particle updates to CUDA/OpenCL
4. **Reduced Output**: Write data less frequently
5. **Binary Output**: Replace CSV with binary format

## Parameter Sensitivity

### Critical Parameters

| Parameter | Typical Range | Impact | Stability Constraint |
|-----------|--------------|--------|---------------------|
| `delta` | 1e-5 - 1e-3 | Accuracy vs. speed | δt < 1/(max force) |
| `epsilon` | 0.001 - 1.0 | Interaction strength | - |
| `Dt` | 0.1 - 100 | Thermal motion | - |
| `De` | 0.0 - 10.0 | Orientation randomization | - |
| `vs` | 0.0 - 10.0 | Active propulsion | - |
| `Particles` | 10 - 1000 | System size | N < volume/(1.5L)³ |

### Recommended Configurations

**Passive Brownian particles**:
```
epsilon=0.01, delta=1e-4, Dt=1.0, De=1.0, vs=0.0
```

**Active particles (weak interactions)**:
```
epsilon=0.01, delta=1e-4, Dt=1.0, De=1.0, vs=5.0
```

**Dense systems (strong interactions)**:
```
epsilon=0.1, delta=5e-5, Dt=0.1, De=0.1, vs=2.0
```

## Error Handling

### Current Limitations

1. **No parameter validation**: Invalid inputs not checked
2. **Silent failures**: malloc() failures unchecked
3. **File I/O errors**: fopen() failures minimally handled
4. **Numerical instabilities**: No adaptive timestep

### Recommended Improvements

```cpp
// Validate parameters
if (Particles <= 0 || N <= 0 || delta <= 0) {
    fprintf(stderr, "Error: Invalid parameters\n");
    exit(1);
}

// Check memory allocation
if (x == NULL || y == NULL || z == NULL) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    exit(1);
}

// Monitor force magnitude
if (F > threshold) {
    fprintf(stderr, "Warning: Large force detected (F=%f)\n", F);
}
```

## Testing and Validation

### Unit Tests (Recommended)

1. **Initialization**:
   - Verify orientation normalization: |e| = 1
   - Check position bounds: |x| < Wall, |z| < height

2. **Integration**:
   - Energy conservation (for De=0, vs=0, epsilon=0)
   - Correct noise statistics: <ξ> = 0, <ξ²> = 1

3. **Boundary Conditions**:
   - Particles never leave domain
   - Momentum conservation in reflections

### Validation Metrics

- **Mean Squared Displacement**: Should scale as t for passive particles
- **Orientation Correlation**: Should decay as exp(-De*t)
- **Radial Distribution**: Check for excluded volume effects

## Code Quality Improvements

### Suggested Refactoring

1. **Use std::vector instead of raw pointers**:
```cpp
std::vector<double> x(Particles);
std::vector<double> y(Particles);
std::vector<double> z(Particles);
```

2. **Create Particle struct**:
```cpp
struct Particle {
    double x, y, z;      // position
    double ex, ey, ez;   // orientation
};
```

3. **Separate concerns**:
```cpp
class Simulation {
    void initialize();
    void step();
    void applyBoundaries();
    void writeOutput();
};
```

4. **Add error handling**:
```cpp
class ParameterError : public std::exception { };
class InitializationError : public std::exception { };
```

## Future Extensions

### Potential Features

1. **Multiple species**: Different particle types with distinct properties
2. **External fields**: Gravity, flow, electric fields
3. **Attractive interactions**: Full Lennard-Jones potential
4. **Soft boundaries**: Harmonic wall potentials
5. **Non-spherical particles**: Rod-like or ellipsoidal geometry
6. **Hydrodynamic interactions**: Far-field coupling
7. **Adaptive timestep**: Automatic stability control
8. **Checkpointing**: Resume from saved state
9. **Real-time visualization**: OpenGL/VTK rendering
10. **Parameter sweeps**: Automated batch runs

## References

### Key Algorithms

- **Euler-Maruyama**: P. E. Kloeden & E. Platen, "Numerical Solution of Stochastic Differential Equations" (1992)
- **Active Brownian Particles**: C. Bechinger et al., Rev. Mod. Phys. 88, 045006 (2016)

### Related Work

- Lennard-Jones potential: Jones (1924)
- Langevin dynamics: Langevin (1908)
- Reflective boundaries: Allen & Tildesley, "Computer Simulation of Liquids" (1987)

---

**Last Updated**: November 2025  
**Version**: 1.0  
**Maintainer**: Jeremy Vachier
