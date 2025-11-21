# CPU OpenMP Version

This folder contains the **CPU-only implementation** using OpenMP for multi-threading parallelization.

## Features

- **OpenMP Parallelization**: Multi-threaded execution with configurable thread count
- **SIMD Vectorization**: Compiler optimizations for enhanced performance
- **Standard C++17**: Works on any platform with g++ or clang++
- **Memory Safe**: Uses `std::vector` for automatic memory management
- **Configurable Threading**: Thread count specified in parameter.txt (11th parameter)

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

Edit `parameter.txt` to change simulation parameters (11 tab-separated values):
```
epsilon  delta  Particles  Dt  De  vs  Wall  height  N  output_interval  N_thread
0.01     1e-4   200        10.1 0.0 0.0 15.0  15.0    1000 100             6
```

The `N_thread` parameter controls the number of OpenMP threads used for parallel execution.

## Performance

| Particles | 1 Thread | 6 Threads | Speedup |
|-----------|----------|-----------|---------|
| 100       | 0.026s   | 0.103s    | 0.25×   |
| 200       | ~0.5s    | ~0.2s     | ~2.5×   |
| 1000      | ~16s     | ~6s       | ~2.7×   |
| 5000      | 34.4s    | 14.3s     | 2.4×    |

*Benchmarks on Apple M2, 1000 timesteps*

**Scaling**: O(N²) due to pairwise particle interactions

**Note**: For N < 100, single-threaded execution is faster due to OpenMP overhead.

## Output

Results are saved to `data/simulation.csv` with particle positions and orientations at each output interval.
