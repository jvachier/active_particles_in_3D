#!/bin/bash
#
# Benchmark script for comparing CPU (OpenMP) vs GPU (Metal) performance
# Tests various particle counts and measures execution time
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
TIMESTEPS=1000
OUTPUT_INTERVAL=100
PARTICLE_COUNTS=(100 200 500 1000 2000 5000)

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Active Particles 3D: Performance Benchmark${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo "Configuration:"
echo "  Timesteps: $TIMESTEPS"
echo "  Output Interval: $OUTPUT_INTERVAL"
echo "  Particle Counts: ${PARTICLE_COUNTS[@]}"
echo ""

# Check if both executables exist
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
echo "Particles,CPU_Time(s),GPU_Time(s),Speedup" > $RESULTS_FILE

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Running Benchmarks${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Function to extract time from output
extract_time() {
    grep "Time taken:" | sed 's/.*Time taken: \([0-9.]*\).*/\1/'
}

# Run benchmarks for each particle count
for N in "${PARTICLE_COUNTS[@]}"; do
    echo -e "${YELLOW}Testing with $N particles...${NC}"
    
    # Create parameter file
    PARAM_LINE="0.01\t1e-4\t$N\t10.1\t0.0\t0.0\t15.0\t15.0\t$TIMESTEPS\t$OUTPUT_INTERVAL"
    
    # CPU (OpenMP) benchmark
    echo -ne "  CPU (OpenMP):  "
    echo -e "$PARAM_LINE" > cpu_openmp/parameter.txt
    CPU_OUTPUT=$(cd cpu_openmp && ./abp_3D_confine.out 2>&1)
    CPU_TIME=$(echo "$CPU_OUTPUT" | extract_time)
    echo -e "${GREEN}${CPU_TIME}s${NC}"
    
    # GPU (Metal) benchmark
    echo -ne "  GPU (Metal):   "
    echo -e "$PARAM_LINE" > gpu_hybrid/parameter.txt
    GPU_OUTPUT=$(cd gpu_hybrid && ./abp_3D_confine.out 2>&1)
    GPU_TIME=$(echo "$GPU_OUTPUT" | extract_time)
    
    # Check if GPU was actually used
    if echo "$GPU_OUTPUT" | grep -q "Metal GPU acceleration enabled"; then
        echo -e "${GREEN}${GPU_TIME}s (GPU)${NC}"
        GPU_MODE="GPU"
    else
        echo -e "${GREEN}${GPU_TIME}s (CPU)${NC}"
        GPU_MODE="CPU"
    fi
    
    # Calculate speedup
    SPEEDUP=$(python3 -c "print(f'{$CPU_TIME / $GPU_TIME:.2f}')" 2>/dev/null || echo "N/A")
    echo -e "  Speedup:       ${BLUE}${SPEEDUP}x${NC}"
    
    # Save to CSV
    echo "$N,$CPU_TIME,$GPU_TIME,$SPEEDUP" >> $RESULTS_FILE
    
    echo ""
done

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Benchmark Results Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Display results table
printf "%-12s %-15s %-15s %-10s\n" "Particles" "CPU (OpenMP)" "GPU (Metal)" "Speedup"
printf "%-12s %-15s %-15s %-10s\n" "----------" "-------------" "------------" "--------"

tail -n +2 $RESULTS_FILE | while IFS=, read -r particles cpu_time gpu_time speedup; do
    printf "%-12s %-15s %-15s %-10s\n" "$particles" "${cpu_time}s" "${gpu_time}s" "${speedup}x"
done

echo ""
echo -e "${GREEN}Results saved to: $RESULTS_FILE${NC}"

# Generate plot if gnuplot is available
if command -v gnuplot &> /dev/null; then
    echo ""
    echo -e "${YELLOW}Generating performance plot...${NC}"
    
    gnuplot << EOF
set terminal png size 1200,800
set output 'benchmark_plot.png'
set title 'CPU (OpenMP) vs GPU (Metal) Performance'
set xlabel 'Number of Particles'
set ylabel 'Time (seconds)'
set logscale xy
set grid
set key left top
set style data linespoints
set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 1.5
set style line 2 lc rgb '#dd181f' lt 1 lw 2 pt 5 ps 1.5
plot '$RESULTS_FILE' using 1:2 with linespoints ls 1 title 'CPU (OpenMP)', \
     '$RESULTS_FILE' using 1:3 with linespoints ls 2 title 'GPU (Metal)'
EOF
    
    echo -e "${GREEN}✓ Plot saved to: benchmark_plot.png${NC}"
    
    # Generate speedup plot
    gnuplot << EOF
set terminal png size 1200,800
set output 'speedup_plot.png'
set title 'GPU Speedup vs CPU (Higher is Better)'
set xlabel 'Number of Particles'
set ylabel 'Speedup (x times faster)'
set logscale x
set grid
set key left top
set style data linespoints
set style line 1 lc rgb '#00aa00' lt 1 lw 2 pt 7 ps 1.5
set yrange [0:*]
plot '$RESULTS_FILE' using 1:4 with linespoints ls 1 title 'GPU Speedup', \
     1 with lines lt 0 lc rgb '#888888' notitle
EOF
    
    echo -e "${GREEN}✓ Speedup plot saved to: speedup_plot.png${NC}"
else
    echo ""
    echo -e "${YELLOW}Install gnuplot to generate performance plots:${NC}"
    echo "  brew install gnuplot"
fi

echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Benchmark Complete!${NC}"
echo -e "${GREEN}========================================${NC}"
