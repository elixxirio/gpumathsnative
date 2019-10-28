// This + linking the so should allow Go to use the bindings I've made more easily.
//
// We are only going to export the "extern C" linked methods, so let's just put those in the header to start...
// Nothing with a template should go in this header.

#ifndef POWM_ODD_EXPORT_H
#define POWM_ODD_EXPORT_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// This type works well when returning large chunks of data to Go
// It's used for the exported bindings for kernel running and data upload
struct return_data {
  // It's difficult for Golang to interpret most of this data, so it should
  // be treated as read-only when the results are used, and unused otherwise.
  // For instance, the upload result gets passed back to the kernel run 
  // method, and it shouldn't be modified or the kernel won't run correctly.
  void *result;
  // Go should check and handle this like a normal Go error - return, handle,
  // or panic.
  const char *error;
};

// Upload data for a powm kernel run for 4K bits
const char* upload_powm_4096(const uint32_t instance_count, void *stream);
// Run powm for 4K bits
const char* run_powm_4096(void *stream);
// Enqueue download from a previous kernel launch
const char* download_powm_4096(void *stream);
// Wait for a results download to finish
struct return_data* getResults_powm(void *stream);

// These methods query the amount of memory necessary for the GPU buffers from the class
size_t getConstantsSize_powm4096();
size_t getInputsSize_powm4096(size_t length);
size_t getOutputsSize_powm4096(size_t length);

struct streamCreateInfo {
  // How many instances can be invoked in a kernel launch?
  size_t capacity;
  // What's the size in bytes of the entire input buffer?
  size_t inputsSize;
  // What's the size in bytes of the entire output buffer?
  size_t outputsSize;
  // What's the size in bytes of the entire constants buffer?
  size_t constantsSize;
};


// Call this when starting the program to allocate resources
// Returns pointer to stream and error
struct return_data* createStream(struct streamCreateInfo createInfo);
// Call this after you're done with the kernel to destroy resources
// Returns error
const char* destroyStream(void *destroyee);

// Get a pointer to the CPU inputs buffer from a stream
// Overwrite this memory with inputs before enqueueing an upload
void* getCpuInputs(void* stream);

// Get a pointer to the CPU outputs buffer from a stream
// Read outputs from this memory after calling getResults to synchronize the event
void* getCpuOutputs(void* stream);

// Get a pointer to the CPU constants buffer from a stream
// Overwrite this memory with constants before enqueueing an upload
void* getCpuConstants(void* stream);

// Call this after execution has completed to write out profile information to the disk
const char* stopProfiling();

// Calling this is optional if you profile from the start of execution.
const char* startProfiling();

// If using the newer profiler, use this instead when kernels have finished 
// running to signal the profiler that execution has finished.
const char* resetDevice();
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // POWM_ODD_EXPORT_H

