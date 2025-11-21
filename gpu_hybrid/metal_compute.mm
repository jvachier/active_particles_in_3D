/**
 * @file metal_compute.mm
 * @brief Implementation of Metal GPU compute wrapper
 * 
 * This file must be compiled as Objective-C++ (.mm extension) to use Metal APIs.
 * Provides C++ interface to Metal compute functionality.
 * 
 * @author Active Particles 3D Simulation
 * @date 2024
 */

#include "headers/metal_compute.h"
#include <iostream>
#include <stdexcept>
#include <cstring>

#if METAL_AVAILABLE

#import <Metal/Metal.h>
#import <Foundation/Foundation.h>

/**
 * @brief Constructor - Initialize Metal device and compile shaders
 */
MetalCompute::MetalCompute() : currentBufferSize(0) {
    // Get default Metal device (GPU)
    id<MTLDevice> mtlDevice = MTLCreateSystemDefaultDevice();
    if (!mtlDevice) {
        throw std::runtime_error("Failed to create Metal device");
    }
    device = (__bridge_retained void*)mtlDevice;
    
    std::cout << "Metal GPU: " << [mtlDevice name].UTF8String << std::endl;
    
    // Create command queue
    id<MTLCommandQueue> mtlQueue = [mtlDevice newCommandQueue];
    if (!mtlQueue) {
        throw std::runtime_error("Failed to create Metal command queue");
    }
    commandQueue = (__bridge_retained void*)mtlQueue;
    
    // Load Metal shader library
    NSError* error = nil;
    NSString* shaderPath = @"particle_interactions.metal";
    NSString* shaderSource = [NSString stringWithContentsOfFile:shaderPath
                                                       encoding:NSUTF8StringEncoding
                                                          error:&error];
    
    if (error || !shaderSource) {
        std::string msg = "Failed to load Metal shader file: particle_interactions.metal";
        if (error) {
            msg += " - ";
            msg += [[error localizedDescription] UTF8String];
        }
        throw std::runtime_error(msg);
    }
    
    // Compile shader library
    id<MTLLibrary> mtlLibrary = [mtlDevice 
        newLibraryWithSource:shaderSource
        options:nil
        error:&error];
    
    if (error || !mtlLibrary) {
        NSString* errorMsg = error ? [error localizedDescription] : @"Unknown error";
        std::string msg = "Failed to compile Metal shaders: ";
        msg += [errorMsg UTF8String];
        throw std::runtime_error(msg);
    }
    library = (__bridge_retained void*)mtlLibrary;
    
    // Get compute function
    id<MTLFunction> kernelFunction = [mtlLibrary 
        newFunctionWithName:@"computeLJForces"];
    
    if (!kernelFunction) {
        throw std::runtime_error("Failed to find computeLJForces kernel in shader");
    }
    
    // Create compute pipeline
    id<MTLComputePipelineState> mtlPipeline = [mtlDevice
        newComputePipelineStateWithFunction:kernelFunction
        error:&error];
    
    if (error || !mtlPipeline) {
        NSString* errorMsg = error ? [error localizedDescription] : @"Unknown error";
        std::string msg = "Failed to create compute pipeline: ";
        msg += [errorMsg UTF8String];
        throw std::runtime_error(msg);
    }
    pipelineState = (__bridge_retained void*)mtlPipeline;
    
    // Initialize buffer pointers
    bufferX = nullptr;
    bufferY = nullptr;
    bufferZ = nullptr;
    bufferFX = nullptr;
    bufferFY = nullptr;
    bufferFZ = nullptr;
    
    std::cout << "Metal compute initialized successfully" << std::endl;
}

/**
 * @brief Destructor - Release Metal resources
 */
MetalCompute::~MetalCompute() {
    if (bufferX) CFRelease((__bridge CFTypeRef)bufferX);
    if (bufferY) CFRelease((__bridge CFTypeRef)bufferY);
    if (bufferZ) CFRelease((__bridge CFTypeRef)bufferZ);
    if (bufferFX) CFRelease((__bridge CFTypeRef)bufferFX);
    if (bufferFY) CFRelease((__bridge CFTypeRef)bufferFY);
    if (bufferFZ) CFRelease((__bridge CFTypeRef)bufferFZ);
    
    if (pipelineState) CFRelease((__bridge CFTypeRef)pipelineState);
    if (library) CFRelease((__bridge CFTypeRef)library);
    if (commandQueue) CFRelease((__bridge CFTypeRef)commandQueue);
    if (device) CFRelease((__bridge CFTypeRef)device);
}

/**
 * @brief Check Metal availability
 */
bool MetalCompute::isAvailable() {
    id<MTLDevice> testDevice = MTLCreateSystemDefaultDevice();
    if (testDevice) {
        return true;
    }
    return false;
}

/**
 * @brief Allocate or resize GPU buffers if needed
 */
void MetalCompute::ensureBufferSize(int numParticles) {
    if (numParticles <= currentBufferSize) return;
    
    // Release old buffers
    if (bufferX) { CFRelease((__bridge CFTypeRef)bufferX); bufferX = nullptr; }
    if (bufferY) { CFRelease((__bridge CFTypeRef)bufferY); bufferY = nullptr; }
    if (bufferZ) { CFRelease((__bridge CFTypeRef)bufferZ); bufferZ = nullptr; }
    if (bufferFX) { CFRelease((__bridge CFTypeRef)bufferFX); bufferFX = nullptr; }
    if (bufferFY) { CFRelease((__bridge CFTypeRef)bufferFY); bufferFY = nullptr; }
    if (bufferFZ) { CFRelease((__bridge CFTypeRef)bufferFZ); bufferFZ = nullptr; }
    
    // Allocate new buffers (unified memory for zero-copy on Apple Silicon)
    size_t bufferSize = numParticles * sizeof(float);
    id<MTLDevice> mtlDevice = (__bridge id<MTLDevice>)device;
    
    id<MTLBuffer> bx = [mtlDevice newBufferWithLength:bufferSize options:MTLResourceStorageModeShared];
    id<MTLBuffer> by = [mtlDevice newBufferWithLength:bufferSize options:MTLResourceStorageModeShared];
    id<MTLBuffer> bz = [mtlDevice newBufferWithLength:bufferSize options:MTLResourceStorageModeShared];
    id<MTLBuffer> bfx = [mtlDevice newBufferWithLength:bufferSize options:MTLResourceStorageModeShared];
    id<MTLBuffer> bfy = [mtlDevice newBufferWithLength:bufferSize options:MTLResourceStorageModeShared];
    id<MTLBuffer> bfz = [mtlDevice newBufferWithLength:bufferSize options:MTLResourceStorageModeShared];
    
    if (!bx || !by || !bz || !bfx || !bfy || !bfz) {
        throw std::runtime_error("Failed to allocate Metal buffers");
    }
    
    bufferX = (__bridge_retained void*)bx;
    bufferY = (__bridge_retained void*)by;
    bufferZ = (__bridge_retained void*)bz;
    bufferFX = (__bridge_retained void*)bfx;
    bufferFY = (__bridge_retained void*)bfy;
    bufferFZ = (__bridge_retained void*)bfz;
    
    currentBufferSize = numParticles;
}

/**
 * @brief Compute forces on GPU
 */
void MetalCompute::computeForces(
    const std::vector<double>& x,
    const std::vector<double>& y,
    const std::vector<double>& z,
    std::vector<double>& fx,
    std::vector<double>& fy,
    std::vector<double>& fz,
    double prefactor,
    int numParticles)
{
    // Ensure buffers are allocated
    ensureBufferSize(numParticles);
    
    // Get buffer pointers
    id<MTLBuffer> bx = (__bridge id<MTLBuffer>)bufferX;
    id<MTLBuffer> by = (__bridge id<MTLBuffer>)bufferY;
    id<MTLBuffer> bz = (__bridge id<MTLBuffer>)bufferZ;
    id<MTLBuffer> bfx = (__bridge id<MTLBuffer>)bufferFX;
    id<MTLBuffer> bfy = (__bridge id<MTLBuffer>)bufferFY;
    id<MTLBuffer> bfz = (__bridge id<MTLBuffer>)bufferFZ;
    
    // Copy input data to GPU (convert double to float for GPU)
    float* xData = (float*)[bx contents];
    float* yData = (float*)[by contents];
    float* zData = (float*)[bz contents];
    
    for (int i = 0; i < numParticles; i++) {
        xData[i] = (float)x[i];
        yData[i] = (float)y[i];
        zData[i] = (float)z[i];
    }
    
    // Create command buffer and encoder
    id<MTLCommandQueue> mtlQueue = (__bridge id<MTLCommandQueue>)commandQueue;
    id<MTLComputePipelineState> mtlPipeline = (__bridge id<MTLComputePipelineState>)pipelineState;
    id<MTLCommandBuffer> commandBuffer = [mtlQueue commandBuffer];
    id<MTLComputeCommandEncoder> encoder = [commandBuffer computeCommandEncoder];
    
    // Set pipeline and buffers
    [encoder setComputePipelineState:mtlPipeline];
    [encoder setBuffer:bx offset:0 atIndex:0];
    [encoder setBuffer:by offset:0 atIndex:1];
    [encoder setBuffer:bz offset:0 atIndex:2];
    [encoder setBuffer:bfx offset:0 atIndex:3];
    [encoder setBuffer:bfy offset:0 atIndex:4];
    [encoder setBuffer:bfz offset:0 atIndex:5];
    
    // Set parameters
    float prefactor_f = (float)prefactor;
    uint32_t numParticles_u = (uint32_t)numParticles;
    [encoder setBytes:&prefactor_f length:sizeof(float) atIndex:6];
    [encoder setBytes:&numParticles_u length:sizeof(uint32_t) atIndex:7];
    
    // Calculate threadgroup size
    NSUInteger threadGroupSize = [mtlPipeline maxTotalThreadsPerThreadgroup];
    if (threadGroupSize > 256) threadGroupSize = 256; // Reasonable limit
    
    MTLSize threadsPerThreadgroup = MTLSizeMake(threadGroupSize, 1, 1);
    MTLSize numThreadgroups = MTLSizeMake(
        (numParticles + threadGroupSize - 1) / threadGroupSize, 1, 1);
    
    // Dispatch compute kernel
    [encoder dispatchThreadgroups:numThreadgroups threadsPerThreadgroup:threadsPerThreadgroup];
    [encoder endEncoding];
    
    // Execute and wait
    [commandBuffer commit];
    [commandBuffer waitUntilCompleted];
    
    // Copy results back (convert float to double)
    float* fxData = (float*)[bfx contents];
    float* fyData = (float*)[bfy contents];
    float* fzData = (float*)[bfz contents];
    
    for (int i = 0; i < numParticles; i++) {
        fx[i] = (double)fxData[i];
        fy[i] = (double)fyData[i];
        fz[i] = (double)fzData[i];
    }
}

/**
 * @brief Get device name
 */
const char* MetalCompute::getDeviceName() const {
    static std::string deviceName;
    id<MTLDevice> mtlDevice = (__bridge id<MTLDevice>)device;
    deviceName = [mtlDevice name].UTF8String;
    return deviceName.c_str();
}

#else // !METAL_AVAILABLE

// Stub implementations for non-Apple platforms
MetalCompute::MetalCompute() {
    throw std::runtime_error("Metal is not available on this platform");
}

MetalCompute::~MetalCompute() {}

bool MetalCompute::isAvailable() {
    return false;
}

void MetalCompute::computeForces(
    const std::vector<double>&, const std::vector<double>&, const std::vector<double>&,
    std::vector<double>&, std::vector<double>&, std::vector<double>&,
    double, int)
{
    throw std::runtime_error("Metal is not available on this platform");
}

const char* MetalCompute::getDeviceName() const {
    return "N/A";
}

#endif // METAL_AVAILABLE
