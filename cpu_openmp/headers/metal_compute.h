/**
 * @file metal_compute.h
 * @brief Metal API wrapper for GPU-accelerated force calculations
 * 
 * This header defines a C++ wrapper around Metal compute functionality
 * for accelerating particle-particle interaction calculations on macOS.
 * 
 * @author Active Particles 3D Simulation
 * @date 2024
 */

#ifndef METAL_COMPUTE_H
#define METAL_COMPUTE_H

#ifdef __APPLE__
#define METAL_AVAILABLE 1
#else
#define METAL_AVAILABLE 0
#endif

#include <vector>

#if METAL_AVAILABLE
// Forward declare Metal types as opaque pointers for C++
// The .mm implementation file will use proper Objective-C types
typedef void* MTLDevicePtr;
typedef void* MTLCommandQueuePtr;
typedef void* MTLComputePipelineStatePtr;
typedef void* MTLLibraryPtr;
typedef void* MTLBufferPtr;
#endif

/**
 * @class MetalCompute
 * @brief Wrapper class for Metal GPU compute operations
 * 
 * Manages Metal device, command queue, and compute pipeline for
 * GPU-accelerated Lennard-Jones force calculations.
 */
class MetalCompute {
public:
    /**
     * @brief Constructor - initializes Metal device and pipeline
     * @throws std::runtime_error if Metal initialization fails
     */
    MetalCompute();
    
    /**
     * @brief Destructor - releases Metal resources
     */
    ~MetalCompute();
    
    /**
     * @brief Check if Metal is available on this system
     * @return true if Metal compute is available, false otherwise
     */
    static bool isAvailable();
    
    /**
     * @brief Compute Lennard-Jones forces on GPU
     * 
     * @param x X-coordinates of particles
     * @param y Y-coordinates of particles
     * @param z Z-coordinates of particles
     * @param fx Output X-component forces
     * @param fy Output Y-component forces
     * @param fz Output Z-component forces
     * @param prefactor Interaction strength (epsilon * delta)
     * @param numParticles Number of particles
     * 
     * @note Uses unified memory on Apple Silicon for zero-copy transfers
     */
    void computeForces(
        const std::vector<double>& x,
        const std::vector<double>& y,
        const std::vector<double>& z,
        std::vector<double>& fx,
        std::vector<double>& fy,
        std::vector<double>& fz,
        double prefactor,
        int numParticles
    );
    
    /**
     * @brief Get device name for logging
     * @return String describing the Metal device
     */
    const char* getDeviceName() const;

private:
#if METAL_AVAILABLE
    MTLDevicePtr device;                      ///< Metal device (GPU)
    MTLCommandQueuePtr commandQueue;          ///< Command queue for GPU work
    MTLComputePipelineStatePtr pipelineState; ///< Compiled compute pipeline
    MTLLibraryPtr library;                    ///< Metal shader library
    
    // GPU buffers for particle data
    MTLBufferPtr bufferX;
    MTLBufferPtr bufferY;
    MTLBufferPtr bufferZ;
    MTLBufferPtr bufferFX;
    MTLBufferPtr bufferFY;
    MTLBufferPtr bufferFZ;
    
    int currentBufferSize;  ///< Current allocated buffer size
    
    /**
     * @brief Allocate or resize GPU buffers
     * @param numParticles Number of particles to allocate for
     */
    void ensureBufferSize(int numParticles);
#endif
};

#endif // METAL_COMPUTE_H
