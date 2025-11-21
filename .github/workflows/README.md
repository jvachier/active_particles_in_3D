# CI/CD Pipeline

This directory contains GitHub Actions workflows for automated testing and validation.

## Workflows

### `ci.yml` - Continuous Integration Pipeline

Automated testing pipeline that runs on every push and pull request.

#### Jobs

1. **Unit Tests** (`unit-tests`)
   - Platform: Ubuntu Latest
   - Compiles and runs all unit tests
   - Tests boundary conditions and initialization modules
   - Uploads test results as artifacts

2. **CPU OpenMP Build** (`cpu-openmp`)
   - Platform: Ubuntu Latest
   - Builds CPU OpenMP version with g++-13
   - Runs smoke test with 10 particles
   - Validates no NaN values in output
   - Uploads executable and simulation results

3. **GPU Hybrid Build** (`gpu-hybrid-macos`)
   - Platform: macOS Latest (required for Metal support)
   - Builds GPU hybrid version with clang++
   - Tests CPU fallback mode with 100 particles
   - Validates Metal shader compilation
   - Checks for NaN-free output
   - Uploads executable and simulation results

4. **Benchmark Validation** (`benchmark-validation`)
   - Platform: macOS Latest
   - Validates benchmark script functionality
   - Runs minimal 3-way comparison (100 particles only)
   - Verifies Python visualization dependencies (uv, plotly, pandas)
   - Checks CSV output format

5. **Documentation Check** (`documentation-check`)
   - Platform: Ubuntu Latest
   - Verifies all README files exist
   - Validates LICENSE file (Apache 2.0)
   - Checks parameter.txt format (11 tab-separated values)
   - Detects broken relative links

6. **CI Success** (`ci-success`)
   - Summary job requiring all previous jobs to pass
   - Provides consolidated success message

## Triggers

The pipeline runs on:
- **Push** to `main` or `jv/improvement2` branches
- **Pull requests** to `main`
- **Manual dispatch** via GitHub Actions UI

## Artifacts

Each job produces artifacts that are retained for 90 days:
- `test-results` - Unit test executables
- `cpu-openmp-build` - CPU version executable and output
- `gpu-hybrid-macos-build` - GPU version executable and output
- `benchmark-test-results` - Benchmark CSV data

## Local Testing

Run the same checks locally before pushing:

```bash
# Unit tests
cd tests
make test

# CPU build
cd cpu_openmp
make clean && make
echo -e "0.01\t1e-4\t10\t10.1\t0.0\t0.0\t15.0\t15.0\t10\t5\t2" > parameter.txt
./abp_3D_confine.out

# GPU build (macOS only)
cd gpu_hybrid
make clean && make
echo -e "0.01\t1e-4\t100\t10.1\t0.0\t0.0\t15.0\t15.0\t10\t5\t2" > parameter.txt
./abp_3D_confine.out

# Check for NaN
grep -q "nan" data/simulation.csv && echo "FAIL: NaN detected" || echo "PASS: No NaN"
```

## Requirements

### Ubuntu Runners
- build-essential
- g++-13
- libomp-dev

### macOS Runners
- Xcode Command Line Tools
- Homebrew (libomp)
- Metal framework (built-in)

## Status Badge

Add to README.md:
```markdown
[![CI/CD](https://github.com/jvachier/active_particles_in_3D/workflows/CI%2FCD%20Pipeline/badge.svg)](https://github.com/jvachier/active_particles_in_3D/actions)
```

## Troubleshooting

### Tests fail on Ubuntu but pass locally (macOS)
- Check g++ version (requires g++-13+)
- Verify OpenMP installation: `dpkg -l | grep libomp`

### GPU tests fail on macOS
- Ensure libomp installed: `brew list libomp`
- Check Metal shader exists: `test -f gpu_hybrid/particle_interactions.metal`

### Benchmark validation timeout
- Reduce particle count in `benchmark_test.sh`
- Current: 100 particles, 10 timesteps (takes ~1s)

### Documentation check fails
- Verify parameter.txt has 11 fields: `awk -F'\t' '{print NF}' parameter.txt`
- Check relative links in README files

## Contributing

When adding new features:
1. Update unit tests if modifying core modules
2. Ensure all tests pass locally before pushing
3. Update documentation (README files)
4. If adding new dependencies, update CI workflow

## Maintenance

Pipeline configuration is in YAML format. Key sections:
- `on:` - Trigger conditions
- `jobs:` - Individual test stages
- `steps:` - Commands within each job
- `needs:` - Job dependencies

To modify:
1. Edit `.github/workflows/ci.yml`
2. Test locally if possible
3. Push to feature branch first
4. Monitor Actions tab for results
5. Merge to main after verification

---

**Last Updated**: November 2025  
**Maintainer**: Jeremy Vachier
