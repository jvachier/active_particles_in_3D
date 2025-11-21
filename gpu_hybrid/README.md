# GPU Hybrid Version (Metal + OpenMP)

This folder contains the **hybrid GPU/CPU implementation** with Metal GPU acceleration for macOS.

## Features

- **Metal GPU Acceleration**: Offloads force calculations to Apple Silicon GPU
- **Automatic Selection**: Uses GPU for N ≥ 500 particles, CPU otherwise
- **Hybrid Execution**: Seamless fallback to OpenMP if GPU unavailable
- **Optimized for Apple Silicon**: Leverages unified memory architecture
- **Zero-Copy Transfers**: Efficient data sharing between CPU and GPU
- **Numerical Safeguards**: Prevents NaN overflow from extreme forces at high particle densities
- **Configurable Threading**: OpenMP thread count via parameter.txt

## Requirements

- **macOS** with Apple Silicon (M1/M2/M3) or Intel with Metal support
- **clang++** compiler (comes with Xcode Command Line Tools)
- **libomp**: `brew install libomp`
- **Metal framework**: Built-in on macOS

## Building

```bash
make clean
make
```

The Makefile automatically:
- Detects macOS and enables Metal support
- Links libomp from Homebrew
- Compiles Metal shaders inline
- Falls back to CPU-only on non-macOS platforms

## Running

### Small simulation (CPU - 200 particles):
```bash
./abp_3D_confine.out
```
Output: `Using CPU (OpenMP) - particle count (200) below GPU threshold (500)`

### Large simulation (GPU - 1000 particles):
```bash
cp parameter_gpu.txt parameter.txt
./abp_3D_confine.out
```
Output: `Metal GPU acceleration enabled for 1000 particles`

## Parameters

Parameter file format (11 tab-separated values):
```
epsilon  delta  Particles  Dt  De  vs  Wall  height  N  output_interval  N_thread
```

**Example configurations:**

**Small simulation (CPU mode, 200 particles):**
```
0.01  1e-4  200   10.1  0.0  0.0  15.0  15.0  1000  100  6
```

**Large simulation (GPU mode, 1000 particles):**
```
0.01  1e-4  1000  10.1  0.0  0.0  15.0  15.0  1000  100  6
```

**Very large simulation (GPU mode, 5000 particles):**
```
0.01  1e-4  5000  10.1  0.0  0.0  15.0  15.0  1000  100  6
```

The `N_thread` parameter controls OpenMP parallelization for CPU portions of the code.

## Performance

| Particles | CPU (1 thread) | CPU (6 threads) | GPU (Metal) | Speedup vs 1 CPU | Speedup vs OpenMP |
|-----------|----------------|-----------------|-------------|------------------|-------------------|
| 100       | 0.026s         | 0.103s          | 0.096s (CPU)| 0.3×             | 0.9×              |
| 200       | ~0.5s          | ~0.2s           | 0.2s (CPU)  | ~2.5×            | ~1.0×             |
| 500       | ~4s            | ~1.5s           | 0.35s (GPU) | **11.4×**        | **4.3×**          |
| 1000      | ~16s           | ~6s             | 0.46s (GPU) | **34.8×**        | **13.0×**         |
| 2000      | ~65s           | ~24s            | 0.88s (GPU) | **73.9×**        | **27.3×**         |
| 5000      | 34.4s          | 14.3s           | 1.7s (GPU)  | **20.2×**        | **8.4×**          |

*Benchmarks on Apple M2, 1000 timesteps*

### Notes
- GPU threshold: N ≥ 500 particles
- Below 500 particles, CPU overhead not worth GPU transfer
- Speedup increases with particle count due to O(N²) force calculation

## GPU Threshold

The hybrid version automatically selects execution mode:
- **N < 500**: Uses CPU (OpenMP) - overhead not worth GPU transfer
- **N ≥ 500**: Uses GPU (Metal) - significant speedup from parallelization

You can modify `GPU_PARTICLE_THRESHOLD` in `abp_3D_confine.cpp` around line 64.

## Implementation Details

### Metal Shader (`particle_interactions.metal`)
- Computes Lennard-Jones forces on GPU
- Each GPU thread processes one particle
- O(N²) force calculation parallelized across thousands of GPU cores
- **Numerical safeguards**:
  - Skips force calculation if particles too close (R² < 0.25, distance < 0.5)
  - Caps maximum force magnitude at 1×10¹⁰ to prevent overflow
  - Prevents NaN propagation at high particle densities

### Force Calculation Split
- **Orientation update**: CPU (small, random, not worth GPU transfer)
- **Force computation**: GPU (O(N²), highly parallelizable)
- **Position update**: CPU (simple, uses pre-computed forces)

### Memory Management
- Unified memory on Apple Silicon eliminates explicit transfers
- Force arrays allocated once, reused each timestep
- Automatic buffer resizing when particle count changes

### Bug Fixes (Nov 2024)
- Fixed missing z-coordinate update in overlap detection
- Added NaN prevention in Metal shader for extreme forces
- Race condition eliminated in `check_nooverlap.cpp`

## Troubleshooting

### Metal initialization fails
```
Failed to initialize Metal GPU: Failed to load Metal shader file
```
**Solution**: Ensure `particle_interactions.metal` is in the current directory when running.

### libomp not found during build
```
ld: library 'omp' not found
```
**Solution**: Install libomp: `brew install libomp`

### Linking warnings
```
ld: warning: building for macOS-16.0, but linking with dylib ... which was built for newer version 26.0
```
**Note**: This is harmless - libomp was built for a newer macOS version but works fine.

## Files

- `abp_3D_confine.cpp` - Main simulation with Metal detection
- `metal_compute.mm` - Objective-C++ wrapper for Metal API
- `metal_compute.h` - C++ header for Metal wrapper
- `particle_interactions.metal` - Metal compute shader
- `compute_forces.cpp` - CPU fallback force calculation
- `update_position_vectorized.cpp` - Vectorized integration
- `parameter.txt` - Default 200 particle config
- `parameter_gpu.txt` - 1000 particle config for GPU

## Output

Results saved to `data/simulation.csv` with particle positions and orientations.
