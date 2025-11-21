# Active Particles in 3D Confinement

[![License: CC BY 4.0](https://img.shields.io/badge/License-CC%20BY%204.0-lightgrey.svg)](https://creativecommons.org/licenses/by/4.0/)
[![Language](https://img.shields.io/badge/language-C++17-blue.svg)](https://isocpp.org/)
[![OpenMP](https://img.shields.io/badge/parallel-OpenMP-orange.svg)](https://www.openmp.org/)

A high-performance C++ simulation framework for modeling the dynamics of active Brownian particles (ABPs) under cylindrical confinement in three-dimensional space. This implementation uses the Euler-Maruyama algorithm to numerically solve coupled Langevin equations with OpenMP parallelization.

## Table of Contents

- [Overview](#overview)
- [Implementations](#implementations)
- [Mathematical Model](#mathematical-model)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [Output](#output)
- [Visualizations](#visualizations)
- [Performance](#performance)
- [Contributing](#contributing)
- [License](#license)
- [Citation](#citation)

## Implementations

This repository provides **two optimized implementations**:

### 1. **CPU-Only Version** (`cpu_openmp/`)
- Standard OpenMP parallelization
- Works on any platform (macOS, Linux, Windows)
- Uses g++-14 compiler
- Best for N < 500 particles
- **See [cpu_openmp/README.md](cpu_openmp/README.md) for details**

### 2. **GPU Hybrid Version** (`gpu_hybrid/`)  
- Metal GPU acceleration for Apple Silicon
- Automatic CPU/GPU selection based on particle count
- Uses clang++ with Metal framework
- **57x-87x speedup** for N > 500 particles on M1/M2/M3
- **See [gpu_hybrid/README.md](gpu_hybrid/README.md) for details**

| Version | Compiler | Platform | 200 Particles | 1000 Particles | 5000 Particles |
|---------|----------|----------|---------------|----------------|----------------|
| CPU OpenMP | g++-14 | Any | 0.13s | 0.78s | 14.44s |
| GPU Hybrid | clang++ | macOS | 0.13s (CPU) | **0.39s (GPU)** | **1.39s (GPU)** |
| **Speedup** | - | - | 1.0× | **2.0×** | **10.4×** |

> 💡 **Benchmark:** Run `./benchmark.sh` to compare performance on your system

## Overview

Active particles are self-propelled entities that convert internal energy into directed motion, commonly found in biological systems (bacteria, cells) and synthetic colloidal systems. This project simulates the collective behavior of interacting active particles confined within cylindrical geometries.

### Key Applications
- Studying collective motion and pattern formation
- Understanding confinement effects on active matter
- Investigating particle-particle interactions in 3D systems
- Modeling biological systems (bacterial suspensions, cell motility)

## Mathematical Model

The system dynamics is governed by two coupled stochastic differential equations:

**Position dynamics:**
$$
\frac{d}{d\tilde{t}}\mathbf{\tilde{r}} = \tilde{v_s}\mathbf{e} - \tilde{\nabla}_{\tilde{R}}(\tilde{U}) + \sqrt{2\tilde{D}_t}\tilde{\mathbf{\xi}_t}
$$

**Orientation dynamics:**
$$
\frac{d}{d\tilde{t}}\mathbf{e} = \sqrt{2\tilde{D}_e}\mathbf{e}\times\tilde{\mathbf{\xi}_e}
$$

### Parameters

| Symbol | Description | Unit |
|--------|-------------|------|
| $\mathbf{\tilde{r}}$ | Particle position vector $(x, y, z)$ | Length |
| $\mathbf{e}$ | Orientation unit vector $(e_x, e_y, e_z)$ | Dimensionless |
| $\tilde{v_s}$ | Self-propulsion velocity | Length/Time |
| $\tilde{D_t}$ | Translational diffusion coefficient | Length²/Time |
| $\tilde{D_e}$ | Rotational diffusion coefficient | 1/Time |
| $\tilde{\mathbf{\xi}_t}, \tilde{\mathbf{\xi}_e}$ | Gaussian white noise | - |

### Interaction Potential

Particle-particle interactions are modeled using the repulsive part of the Lennard-Jones potential:

$$
\tilde{U}(\tilde{R}) = 4\tilde{\epsilon}\left[\left(\frac{\tilde{\sigma}}{\tilde{R}}\right)^{12} - \left(\frac{\tilde{\sigma}}{\tilde{R}}\right)^{6}\right]
$$

where:
- $\tilde{\epsilon}$: Interaction strength (depth of potential well)
- $\tilde{\sigma}$: Characteristic length scale
- $\tilde{R}$: Inter-particle distance

**Note:** Only the repulsive component ($\tilde{R} < 2^{1/6}\tilde{\sigma}$) is considered in this implementation.

## Features

- **3D Langevin dynamics** with Euler-Maruyama integration
- **Cylindrical reflective boundary conditions** for confinement
- **Repulsive Lennard-Jones interactions** between particles
- **OpenMP parallelization** for high performance
- **Proper stochastic noise generation** with configurable seeds
- **CSV output** for trajectory analysis
- **Overlap prevention** during initialization
- **Configurable simulation parameters** via text file

## Prerequisites

### Required
- **C++ Compiler** with C++17 support and OpenMP
  - GCC 7.0+ (recommended: GCC 13+)
  - Clang 5.0+ with OpenMP support
- **Make** build system

### macOS Installation
```bash
# Install GCC with OpenMP support
brew install gcc

# Verify installation
g++-13 --version
```

### Linux Installation
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential g++ libomp-dev

# Fedora/RHEL
sudo dnf install gcc-c++ make libomp-devel
```

## Installation

1. **Clone the repository**
```bash
git clone https://github.com/jvachier/active_particles_in_3D.git
cd active_particles_in_3D
```

2. **Choose your implementation**

   **Option A: CPU-Only (OpenMP)**
   ```bash
   cd cpu_openmp
   make
   ```

   **Option B: GPU Hybrid (Metal + OpenMP) - macOS only**
   ```bash
   # Install libomp if not already installed
   brew install libomp
   
   cd gpu_hybrid
   make
   ```

3. **Verify installation**
```bash
# For CPU version
./abp_3D_confine.out

# For GPU version
./abp_3D_confine.out
```

### Build Options

```bash
make              # Build the simulation
make clean        # Remove object files and executable
```

## Usage

### Quick Start

**CPU version** (works everywhere):
```bash
cd cpu_openmp
./abp_3D_confine.out
```

**GPU version** (macOS, automatic CPU/GPU selection):
```bash
cd gpu_hybrid

# Small simulation (uses CPU)
./abp_3D_confine.out

# Large simulation (uses GPU)
cp parameter_gpu.txt parameter.txt
./abp_3D_confine.out
```

### Detailed Usage

1. **Configure simulation parameters**:
```bash
# Edit parameter.txt with your desired values
nano parameter.txt
```

2. **Run the simulation**:
```bash
./abp_3D_confine.out
```

3. **Access results**:
```bash
# Simulation data
cat data/simulation.csv
```

### Quick Start Example

```bash
cd src
# Run with default parameters (200 particles, 10000 timesteps)
./abp_3D_confine.out
```

Expected output:
```
0.010000  1.000000e-04  200  10.100000  0.000000  0.000000  15.000000  15.000000  10000
Initialization done.
Time taken is 12.345678
```

## Configuration

Edit `src/parameter.txt` to configure the simulation. The file contains a single line with tab-separated values:

```
epsilon  delta  Particles  Dt  De  vs  Wall  height  N  output_interval
```

### Parameter Descriptions

| Parameter | Description | Typical Range | Default |
|-----------|-------------|---------------|---------|
| `epsilon` | Interaction strength | 0.001 - 1.0 | 0.01 |
| `delta` | Time step | 1e-5 - 1e-3 | 1e-4 |
| `Particles` | Number of particles | 10 - 1000 | 200 |
| `Dt` | Translational diffusion | 0.1 - 100 | 10.1 |
| `De` | Rotational diffusion | 0.0 - 10.0 | 0.0 |
| `vs` | Self-propulsion velocity | 0.0 - 10.0 | 0.0 |
| `Wall` | Cylinder radius | 5.0 - 50.0 | 15.0 |
| `height` | Cylinder height | 5.0 - 50.0 | 15.0 |
| `N` | Number of iterations | 1000 - 100000 | 10000 |
| `output_interval` | Save frequency (timesteps) | 1 - 1000 | 10 |

### Example Configurations

**Passive Brownian particles:**
```
0.01  1e-4  100  1.0  1.0  0.0  10.0  10.0  5000  10
```

**Active particles without interactions:**
```
0.0  1e-4  100  1.0  1.0  5.0  10.0  10.0  5000  10
```

**Active particles with strong interactions:**
```
0.1  1e-4  100  1.0  1.0  5.0  10.0  10.0  5000  10
```

**High-resolution output (save every timestep):**
```
0.01  1e-4  100  1.0  1.0  5.0  10.0  10.0  1000  1
```

**Low-resolution output (save every 100 timesteps):**
```
0.01  1e-4  200  1.0  1.0  5.0  15.0  15.0  10000  100
```

## Output

### Data Format

The simulation outputs a CSV file (`data/simulation.csv`) with the following columns:

```csv
Particles,x-position,y-position,z-position,ex-orientation,ey-orientation,ez-orientation,time
0,1.234,-2.345,0.456,0.707,0.707,0.000,0
0,1.456,-2.123,0.478,0.710,0.704,0.001,10
...
```

- **Particles**: Particle ID (0 to N-1)
- **x,y,z-position**: 3D coordinates
- **ex,ey,ez-orientation**: Unit orientation vector components
- **time**: Simulation timestep

**Note:** Data is saved at intervals specified by the `output_interval` parameter (default: every 10 timesteps).

## Visualizations

### Particle Dynamics Video

https://github.com/jvachier/active_particles_in_3D/assets/89128100/e0ea3d4e-58a5-4565-8e57-4705057479df

*Real-time visualization of 200 active particles under cylindrical confinement showing collective motion patterns.*

### Spatial Distribution

![Particle positions](./cpu_openmp/figures/particles.png)

*Snapshot of particle positions showing spatial organization within the cylindrical boundary.*

### Trajectory Analysis

![Particle trajectories](./cpu_openmp/figures/particles_time.png)

*Time-resolved particle trajectories demonstrating complex motion patterns.*

## Performance

### Computational Complexity
- **Time per step**: O(N²) for particle interactions
- **Memory**: O(N) for position and orientation storage

### Benchmark Results

Comprehensive benchmark comparing CPU (OpenMP) vs GPU (Metal) performance on **Apple M2** with **1000 timesteps**:

| Particles | CPU (OpenMP) | GPU (Metal) | Speedup | GPU Used |
|-----------|--------------|-------------|---------|----------|
| 100       | 0.105s       | 0.102s      | 1.03×   | ❌ (below threshold) |
| 200       | 0.123s       | 0.117s      | 1.05×   | ❌ (below threshold) |
| 500       | 0.271s       | 0.230s      | 1.18×   | ❌ (below threshold) |
| 1,000     | 0.779s       | 0.393s      | **1.98×**  | ✅ |
| 2,000     | 2.533s       | 0.585s      | **4.33×**  | ✅ |
| 5,000     | 14.441s      | 1.385s      | **10.43×** | ✅ |

**Key Findings:**
- GPU acceleration kicks in at N > 500 particles
- Near-linear GPU scaling: 2× particles ≈ 1.5× time (vs. 3.3× for CPU)
- Maximum tested speedup: **10.43× at 5000 particles**
- CPU overhead dominates for small simulations (N < 500)

### Running Benchmarks

```bash
# Run comprehensive benchmark suite
./benchmark.sh

# Results saved to:
# - benchmark_results.csv (raw data)
# - benchmark_plot.png (performance comparison)
# - speedup_plot.png (GPU speedup visualization)
```

### Optimization Tips

**For CPU version:**
1. Adjust thread count in `cpu_openmp/abp_3D_confine.cpp`:
```cpp
#define N_thread 6  // Set to your CPU core count
```

**For GPU version:**
2. Modify GPU threshold in `gpu_hybrid/abp_3D_confine.cpp`:
```cpp
#define GPU_PARTICLE_THRESHOLD 500  // Lower for earlier GPU usage
```

**General:**
3. Reduce output frequency for faster execution:
```cpp
if (time % 100 == 0)  // Save every 100 steps instead of 10
```

4. Use smaller timesteps for accuracy vs. larger for speed

## Contributing

Contributions are welcome! Please follow these guidelines:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Development Priorities
- [ ] Implement unit tests
- [ ] Add Python visualization tools
- [ ] Create CMake build system
- [ ] Add more boundary condition types
- [ ] Implement adaptive timestep control
- [ ] GPU acceleration support

## License

This project is licensed under the Creative Commons Attribution 4.0 International License (CC-BY-4.0). See [LICENCE](LICENCE) file for details.

You are free to:
- **Share** — copy and redistribute the material
- **Adapt** — remix, transform, and build upon the material

Under the following terms:
- **Attribution** — You must give appropriate credit

## Citation

If you use this code in your research, please cite:

```bibtex
@software{vachier2023active,
  author = {Vachier, Jeremy},
  title = {Active Particles in 3D Confinement},
  year = {2023},
  url = {https://github.com/jvachier/active_particles_in_3D}
}
```

## Contact

**Jeremy Vachier**
- GitHub: [@jvachier](https://github.com/jvachier)

## Acknowledgments

- Implementation based on the Euler-Maruyama numerical scheme for stochastic differential equations
- OpenMP parallelization framework
- Inspired by research on active matter physics

---

**Project Status**: Active Development | **Version**: 1.0.0 | **Last Updated**: November 2025

