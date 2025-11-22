# Active Particles in 3D Confinement

[![CI/CD](https://github.com/jvachier/active_particles_in_3D/workflows/CI%2FCD%20Pipeline/badge.svg)](https://github.com/jvachier/active_particles_in_3D/actions)
[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![Language](https://img.shields.io/badge/language-C++17-blue.svg)](https://isocpp.org/)
[![OpenMP](https://img.shields.io/badge/parallel-OpenMP-orange.svg)](https://www.openmp.org/)
[![Metal](https://img.shields.io/badge/GPU-Metal-red.svg)](https://developer.apple.com/metal/)
[![Python](https://img.shields.io/badge/python-uv-blue.svg)](https://github.com/astral-sh/uv)
[![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux-lightgrey.svg)](https://github.com/jvachier/active_particles_in_3D)

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
- **Up to 27× speedup** for N ≥ 1000 particles on M1/M2/M3
- Robust numerical safeguards prevent NaN overflow
- **See [gpu_hybrid/README.md](gpu_hybrid/README.md) for details**

| Version | Compiler | Platform | 100 Particles | 1000 Particles | 5000 Particles |
|---------|----------|----------|---------------|----------------|----------------|
| CPU (1 thread) | g++-14 | Any | 0.026s | 4.3s | 34.4s |
| CPU OpenMP (6 threads) | g++-14 | Any | 0.103s | 1.8s | 14.3s |
| GPU Hybrid | clang++ | macOS | 0.096s (CPU) | **0.46s (GPU)** | **1.7s (GPU)** |
| **GPU Speedup vs 1 CPU** | - | - | 0.3× | **9.3×** | **20.2×** |
| **GPU Speedup vs OpenMP** | - | - | 0.9× | **3.9×** | **8.4×** |

> **Benchmark:** Run `./benchmark.sh` to compare all three modes on your system

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
\frac{d}{d\tilde{t}}\mathbf{\tilde{r}} = \tilde{v_s}\mathbf{e} - \tilde{\nabla}_{\tilde{R}}(\tilde{U}) + \sqrt{2\tilde{D}_t}\tilde{\mathbf{\xi}_t}\,.
$$

**Orientation dynamics:**

$$
\frac{d}{d\tilde{t}}\mathbf{e} = \sqrt{2\tilde{D}_e}\mathbf{e}\times\tilde{\mathbf{\xi}_e}\,.
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
\tilde{U}(\tilde{R}) = 4\tilde{\epsilon}\left[\left(\frac{\tilde{\sigma}}{\tilde{R}}\right)^{12} - \left(\frac{\tilde{\sigma}}{\tilde{R}}\right)^{6}\right]\,,
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

Edit `parameter.txt` to configure the simulation. The file contains a single line with 11 tab-separated values:

```
epsilon  delta  Particles  Dt  De  vs  Wall  height  N  output_interval  N_thread
```

### Parameter Descriptions

| Parameter | Description | Typical Range | Default |
|-----------|-------------|---------------|---------|
| `epsilon` | Interaction strength | 0.001 - 1.0 | 0.01 |
| `delta` | Time step | 1e-5 - 1e-3 | 1e-4 |
| `Particles` | Number of particles | 10 - 5000 | 200 |
| `Dt` | Translational diffusion | 0.1 - 100 | 10.1 |
| `De` | Rotational diffusion | 0.0 - 10.0 | 0.0 |
| `vs` | Self-propulsion velocity | 0.0 - 10.0 | 0.0 |
| `Wall` | Cylinder radius | 5.0 - 50.0 | 15.0 |
| `height` | Cylinder height | 5.0 - 50.0 | 15.0 |
| `N` | Number of iterations | 1000 - 100000 | 1000 |
| `output_interval` | Save frequency (timesteps) | 1 - 1000 | 100 |
| `N_thread` | OpenMP thread count | 1 - 16 | 6 |

### Example Configurations

**Passive Brownian particles (single thread):**
```
0.01  1e-4  100  1.0  1.0  0.0  10.0  10.0  5000  10  1
```

**Active particles without interactions (6 threads):**
```
0.0  1e-4  100  1.0  1.0  5.0  10.0  10.0  5000  10  6
```

**Active particles with strong interactions:**
```
0.1  1e-4  100  1.0  1.0  5.0  10.0  10.0  5000  10  6
```

**High-resolution output (save every timestep):**
```
0.01  1e-4  100  1.0  1.0  5.0  10.0  10.0  1000  1  6
```

**Low-resolution output (save every 100 timesteps):**
```
0.01  1e-4  200  1.0  1.0  5.0  15.0  15.0  10000  100  6
```

**Large-scale simulation (GPU-optimized):**
```
0.01  1e-4  5000  10.1  0.0  0.0  15.0  15.0  1000  100  6
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

*Figure: Snapshot of particle positions showing spatial organization within the cylindrical boundary.*

### Trajectory Analysis

![Particle trajectories](./cpu_openmp/figures/particles_time.png)

*Figure: Time-resolved particle trajectories demonstrating complex motion patterns.*

## Performance

### Computational Complexity
- **Time per step**: O(N²) for particle interactions
- **Memory**: O(N) for position and orientation storage

### Benchmark Results

Comprehensive three-way comparison on **Apple M2** with **1000 timesteps**:

| Particles | CPU (1 thread) | CPU (OpenMP 6) | GPU (Metal) | GPU vs 1 CPU | GPU vs OpenMP |
|-----------|----------------|----------------|-------------|--------------|---------------|
| 100       | 0.026s         | 0.103s         | 0.096s      | 0.3×         | 0.9×          |
| 200       | ~0.5s          | ~0.2s          | 0.2s (CPU)  | ~2.5×        | ~1.0×         |
| 500       | ~4s            | ~1.5s          | 0.35s       | **11.4×**    | **4.3×**      |
| 1,000     | ~16s           | ~6s            | 0.46s       | **34.8×**    | **13.0×**     |
| 2,000     | ~65s           | ~24s           | 0.88s       | **73.9×**    | **27.3×**     |
| 5,000     | 34.4s          | 14.3s          | 1.7s        | **20.2×**    | **8.4×**      |

**Key Findings:**
- GPU acceleration activates at N ≥ 500 particles
- OpenMP provides ~2.4× speedup over single thread for large N
- GPU provides up to **27× speedup vs OpenMP** at N=2000
- Best performance: GPU for N ≥ 500, OpenMP for 100 ≤ N < 500, single thread for N < 100
- Numerical safeguards prevent NaN overflow at high densities (N=5000 verified stable)

![Performance Comparison](benchmark_plots.png)
*Figure: Performance comparison across implementations. **Left:** Execution time vs particle count showing GPU efficiency at scale. **Center:** Speedup for small systems (N ≤ 500) where CPU remains competitive. **Right:** Speedup for large systems (N ≥ 1000) demonstrating GPU's 27× advantage. All benchmarks run on Apple M2 with 1000 timesteps.*

### Running Benchmarks

```bash
# Run comprehensive three-way benchmark suite
./benchmark.sh

# Results saved to:
# - benchmark_results.csv (raw timing data)
# - benchmark_plots.html (interactive Plotly visualization)
# - benchmark_plots.png (high-resolution static plot)
```

**Benchmark generates 3 subplots:**
1. Execution time vs particle count (log scale)
2. Speedup comparison for small systems (N ≤ 500)
3. Speedup comparison for large systems (N ≥ 1000)

### Optimization Tips

**For all versions:**
1. Adjust thread count in `parameter.txt` (11th parameter):
```
0.01  1e-4  1000  10.1  0.0  0.0  15.0  15.0  1000  100  12
```
Set `N_thread` to your CPU core count for optimal performance.

**For GPU version:**
2. Modify GPU threshold in `gpu_hybrid/abp_3D_confine.cpp` (around line 64):
```cpp
const int GPU_PARTICLE_THRESHOLD = 500;  // Lower for earlier GPU usage
```

**General:**
3. Reduce output frequency in `parameter.txt`:
```
0.01  1e-4  5000  10.1  0.0  0.0  15.0  15.0  1000  1000  6
```
Set `output_interval=1000` to save every 1000 steps instead of 100.

4. Trade-off timestep size: smaller `delta` for accuracy vs. larger for speed

## Contributing

Contributions are welcome! Please follow these guidelines:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the **Apache License 2.0**. See [LICENCE](LICENCE) file for details.

You are free to:
- **Use** — Use the software for any purpose
- **Modify** — Modify the source code
- **Distribute** — Distribute original or modified versions
- **Patent Grant** — Receive patent rights from contributors

Under the following terms:
- **Attribution** — Must provide proper credit and license notice
- **State Changes** — Must document modifications made to the code

## Citation

If you use this code in your research, please cite:

```bibtex
@software{vachier2023active,
  author = {Vachier, Jeremy},
  title = {Active Particles in 3D Confinement: GPU-Accelerated Simulation Framework},
  year = {2023-2025},
  url = {https://github.com/jvachier/active_particles_in_3D},
  note = {Includes Metal GPU acceleration for Apple Silicon}
}
```

## Contact

**Author: Jeremy Vachier**
- GitHub: [@jvachier](https://github.com/jvachier)


