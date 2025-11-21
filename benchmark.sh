#!/bin/bash
#
# Enhanced Benchmark: Single CPU vs OpenMP (6 threads) vs GPU (Metal)
# Tests various particle counts and generates visualization
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
TIMESTEPS=1000
OUTPUT_INTERVAL=100
PARTICLE_COUNTS=(100 200 500 1000 2000 5000)

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  Active Particles 3D Benchmark${NC}"
echo -e "${BLUE}  1 CPU vs OpenMP (6) vs GPU Metal${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo "Configuration:"
echo "  Timesteps: $TIMESTEPS"
echo "  Output Interval: $OUTPUT_INTERVAL"
echo "  Particle Counts: ${PARTICLE_COUNTS[@]}"
echo ""

# Build both versions if needed
if [ ! -f "cpu_openmp/abp_3D_confine.out" ]; then
    echo -e "${YELLOW}Building CPU (OpenMP) version...${NC}"
    cd cpu_openmp
    make clean && make
    cd ..
fi

if [ ! -f "gpu_hybrid/abp_3D_confine.out" ]; then
    echo -e "${YELLOW}Building GPU (Metal/OpenMP) hybrid version...${NC}"
    cd gpu_hybrid
    make clean && make
    cd ..
fi

echo -e "${GREEN}✓ Both executables ready${NC}"
echo ""

# Create results file
RESULTS_FILE="benchmark_results.csv"
echo "Particles,Single_CPU_Time,OpenMP_6_Time,GPU_Time,OpenMP_vs_Single,GPU_vs_OpenMP,GPU_vs_Single" > $RESULTS_FILE

# Function to extract time from simulation output
extract_time() {
    local output="$1"
    local time=$(echo "$output" | grep "Time taken:" | sed 's/.*Time taken: \([0-9.]*\) seconds.*/\1/')
    if [ -z "$time" ]; then
        echo "ERROR"
    else
        echo "$time"
    fi
}

# Function to create parameter file for benchmarking
create_param_file() {
    local particles=$1
    local n_thread=$2
    local filename=$3
    
    # epsilon delta particles Dt De vs Wall height N output_interval N_thread
    echo -e "0.01\t1e-4\t${particles}\t10.1\t0.0\t0.0\t15.0\t15.0\t${TIMESTEPS}\t${OUTPUT_INTERVAL}\t${n_thread}" > "$filename"
}

echo -e "${CYAN}Starting benchmark...${NC}"
echo ""

# Run benchmarks for each particle count
for N in "${PARTICLE_COUNTS[@]}"; do
    echo -e "${MAGENTA}Testing $N particles:${NC}"
    
    # ========================================
    # Test 1: Single CPU (1 thread, OpenMP version)
    # ========================================
    echo -ne "  ${YELLOW}[1/3]${NC} Single CPU (1 thread)... "
    create_param_file $N 1 "cpu_openmp/parameter.txt"
    cd cpu_openmp
    CPU1_OUTPUT=$(./abp_3D_confine.out 2>&1)
    cd ..
    CPU1_TIME=$(extract_time "$CPU1_OUTPUT")
    echo -e "${GREEN}${CPU1_TIME}s${NC}"
    
    # ========================================
    # Test 2: OpenMP with 6 threads
    # ========================================
    echo -ne "  ${YELLOW}[2/3]${NC} OpenMP (6 threads)...    "
    create_param_file $N 6 "cpu_openmp/parameter.txt"
    cd cpu_openmp
    OMP_OUTPUT=$(./abp_3D_confine.out 2>&1)
    cd ..
    OMP_TIME=$(extract_time "$OMP_OUTPUT")
    echo -e "${GREEN}${OMP_TIME}s${NC}"
    
    # ========================================
    # Test 3: GPU Metal (with OpenMP fallback for small N)
    # ========================================
    echo -ne "  ${YELLOW}[3/3]${NC} GPU Metal...            "
    create_param_file $N 6 "gpu_hybrid/parameter.txt"
    cd gpu_hybrid
    GPU_OUTPUT=$(./abp_3D_confine.out 2>&1)
    cd ..
    GPU_TIME=$(extract_time "$GPU_OUTPUT")
    
    # Check if GPU was actually used
    if echo "$GPU_OUTPUT" | grep -q "Metal GPU acceleration enabled"; then
        echo -e "${GREEN}${GPU_TIME}s ${CYAN}(GPU)${NC}"
    else
        echo -e "${GREEN}${GPU_TIME}s ${YELLOW}(CPU fallback)${NC}"
    fi
    
    # Calculate speedups
    SPEEDUP_OMP=$(echo "scale=2; $CPU1_TIME / $OMP_TIME" | bc -l)
    SPEEDUP_GPU_OMP=$(echo "scale=2; $OMP_TIME / $GPU_TIME" | bc -l)
    SPEEDUP_GPU_CPU1=$(echo "scale=2; $CPU1_TIME / $GPU_TIME" | bc -l)
    
    echo -e "  ${BLUE}→${NC} OpenMP vs Single: ${GREEN}${SPEEDUP_OMP}×${NC}"
    echo -e "  ${BLUE}→${NC} GPU vs OpenMP:    ${GREEN}${SPEEDUP_GPU_OMP}×${NC}"
    echo -e "  ${BLUE}→${NC} GPU vs Single:    ${GREEN}${SPEEDUP_GPU_CPU1}×${NC}"
    echo ""
    
    # Save to CSV
    echo "$N,$CPU1_TIME,$OMP_TIME,$GPU_TIME,$SPEEDUP_OMP,$SPEEDUP_GPU_OMP,$SPEEDUP_GPU_CPU1" >> $RESULTS_FILE
done

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Benchmark Complete!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "Results saved to: $RESULTS_FILE"
echo ""

# Restore default parameter files (200 particles, 6 threads)
create_param_file 200 6 "cpu_openmp/parameter.txt"
create_param_file 200 6 "gpu_hybrid/parameter.txt"

# Generate visualization if uv is available
if command -v uv &> /dev/null; then
    echo -e "${CYAN}Generating visualizations...${NC}"
    if uv run visualize_benchmark.py; then
        echo -e "${GREEN}✓ Interactive plots saved to benchmark_plots.html${NC}"
        echo -e "${GREEN}✓ Open in browser: open benchmark_plots.html${NC}"
    else
        echo -e "${YELLOW}⚠ Visualization failed. Install dependencies: uv add plotly pandas${NC}"
    fi
else
    echo -e "${YELLOW}⚠ uv not found. Skipping visualization.${NC}"
    echo -e "  To generate plots: uv run visualize_benchmark.py${NC}"
fi

echo ""
echo -e "${BLUE}Summary Table:${NC}"
echo ""
column -t -s',' $RESULTS_FILE
