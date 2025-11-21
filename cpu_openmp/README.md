# CPU OpenMP Version

This folder contains the **CPU-only implementation** using OpenMP for multi-threading parallelization.

## Features

- **OpenMP Parallelization**: Multi-threaded execution using 6 threads
- **SIMD Vectorization**: Compiler optimizations for enhanced performance
- **Standard C++17**: Works on any platform with g++ or clang++
- **Memory Safe**: Uses `std::vector` for automatic memory management

## Requirements

- g++-14 (or any modern C++ compiler)
- OpenMP support
- macOS, Linux, or Windows (with appropriate compiler)

## Building

```bash
make clean
make
```

## Running

```bash
./abp_3D_confine.out
```

## Parameters

Edit `parameter.txt` to change simulation parameters:
```
epsilon  delta  Particles  Dt  De  vs  Wall  height  N  output_interval
0.01     1e-4   200        10.1 0.0 0.0 15.0  15.0    1000 10
```

## Performance

- **200 particles**: ~0.13 seconds for 1000 timesteps
- **1000 particles**: ~3-4 seconds for 1000 timesteps (estimated)
- **Scaling**: O(N²) due to pairwise particle interactions

## Output

Results are saved to `data/simulation.csv` with particle positions and orientations at each output interval.
