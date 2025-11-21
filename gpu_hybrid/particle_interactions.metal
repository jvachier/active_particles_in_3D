#include <metal_stdlib>
using namespace metal;

/**
 * @file particle_interactions.metal
 * @brief Metal compute shader for GPU-accelerated Lennard-Jones force calculations
 * 
 * This shader implements the O(N²) particle-particle interaction calculations
 * using GPU parallelization. Each thread computes forces for one particle from
 * all other particles.
 * 
 * @author Active Particles 3D Simulation
 * @date 2025
 */

/**
 * @brief Compute Lennard-Jones forces for particle interactions
 * 
 * Calculates repulsive forces between particles using the Lennard-Jones potential.
 * Each thread processes one particle and computes its interaction with all others.
 * 
 * Force calculation: F = prefactor * (r_j - r_i) / R^14
 * where R is the distance between particles i and j
 * 
 * @param x X-coordinates of all particles
 * @param y Y-coordinates of all particles
 * @param z Z-coordinates of all particles
 * @param fx Output X-component forces on particles
 * @param fy Output Y-component forces on particles
 * @param fz Output Z-component forces on particles
 * @param prefactor Interaction strength parameter (epsilon * delta)
 * @param numParticles Total number of particles
 * @param threadId Thread index (maps to particle index)
 */
kernel void computeLJForces(
    device float* x [[buffer(0)]],
    device float* y [[buffer(1)]],
    device float* z [[buffer(2)]],
    device float* fx [[buffer(3)]],
    device float* fy [[buffer(4)]],
    device float* fz [[buffer(5)]],
    constant float& prefactor [[buffer(6)]],
    constant uint& numParticles [[buffer(7)]],
    uint threadId [[thread_position_in_grid]])
{
    // Return if thread ID exceeds particle count
    if (threadId >= numParticles) return;
    
    // Local force accumulator for this particle
    float fx_local = 0.0f;
    float fy_local = 0.0f;
    float fz_local = 0.0f;
    
    // Current particle position
    float xi = x[threadId];
    float yi = y[threadId];
    float zi = z[threadId];
    
    // Loop over all other particles
    for (uint j = 0; j < numParticles; j++) {
        if (j == threadId) continue; // Skip self-interaction
        
        // Distance vector components
        float dx = x[j] - xi;
        float dy = y[j] - yi;
        float dz = z[j] - zi;
        
        // Distance squared and to various powers
        float R2 = dx*dx + dy*dy + dz*dz;
        
        // Skip if particles are too close (avoid division by zero and overflow)
        // Minimum distance threshold: 0.5 (about half the particle diameter)
        if (R2 < 0.25f) continue;
        
        float R4 = R2 * R2;
        float R8 = R4 * R4;
        float R14 = R8 * R4 * R2;
        
        // Lennard-Jones force (repulsive only)
        // F_ij = prefactor * (r_j - r_i) / R^14
        float force_magnitude = prefactor / R14;
        
        // Cap force magnitude to prevent overflow
        force_magnitude = min(force_magnitude, 1e10f);
        
        fx_local += force_magnitude * dx;
        fy_local += force_magnitude * dy;
        fz_local += force_magnitude * dz;
    }
    
    // Write accumulated forces to global memory
    fx[threadId] = fx_local;
    fy[threadId] = fy_local;
    fz[threadId] = fz_local;
}


