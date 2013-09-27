// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_ALLOCATOR_ALLOCATOR_EXTENSION_H
#define BASE_ALLOCATOR_ALLOCATOR_EXTENSION_H

#include <stddef.h> // for size_t

#include "base/allocator/allocator_extension_thunks.h"
#include "base/base_export.h"
#include "build/build_config.h"

namespace base {
namespace allocator {

// Request the allocator to report value of its waste memory size.
// Waste size corresponds to memory that has been allocated from the OS but
// not passed up to the application. It e.g. includes memory retained by free
// lists, internal data, chunks padding, etc.
//
// |size| pointer to the returned value, must be not NULL.
// Returns true if the value has been returned, false otherwise.
BASE_EXPORT bool GetAllocatorWasteSize(size_t* size);

// Request that the allocator print a human-readable description of the current
// state of the allocator into a null-terminated string in the memory segment
// buffer[0,buffer_length-1].
//
// |buffer| must point to a valid piece of memory
// |buffer_length| must be > 0.
BASE_EXPORT void GetStats(char* buffer, int buffer_length);

// The MemoryCollarFunction is used to monitor and possibly deny
// allocations of system memory.
//
// It is to be called by the allocator when first set, prior to
// allocating and after releasing system memory. It is only for system
// memory and should not be called for allocations that can be
// fullfilled on the existing arena without allocating or committing
// released system memory.
//
// When a new MemoryCollarFunction is set, the allocator must call it
// with the current system memory usage of the allocator. If this
// first call returns false, the allocator must not allocate more
// system memory and should try to release memory back to the system.
//
// If false is returned prior to an allocation (i.e. diff > 0) the
// allocator must not allocate more system memory and should rather
// fail the allocation. In the event that an allocation is allowed,
// but the system call to allocate more memory fails, the monitor
// should be called with a negative diff of the same amount to correct
// the count.
//
// If false is returned when releasing memory (i.e. diff < 0) it means
// that the allocator is using too much memory and should try to
// release more memory.
//
// N.B. The allocator may call the function while the allocator is
// locked. The function must not do anything that requires the
// allocator.
typedef bool (*MemoryCollarFunction)(int64 diff);
BASE_EXPORT void SetMemoryCollarFunction(MemoryCollarFunction f);

// Request that the allocator release any free memory it knows about to the
// system.
BASE_EXPORT void ReleaseFreeMemory();


// These settings allow specifying a callback used to implement the allocator
// extension functions.  These are optional, but if set they must only be set
// once.  These will typically called in an allocator-specific initialization
// routine.
//
// No threading promises are made.  The caller is responsible for making sure
// these pointers are set before any other threads attempt to call the above
// functions.
BASE_EXPORT void SetGetAllocatorWasteSizeFunction(
    thunks::GetAllocatorWasteSizeFunction get_allocator_waste_size_function);

BASE_EXPORT void SetGetStatsFunction(
    thunks::GetStatsFunction get_stats_function);

BASE_EXPORT void SetSetMemoryCollarFunctionFunction(
    thunks::SetMemoryCollarFunctionFunction
        set_memory_collar_function_function);

BASE_EXPORT void SetReleaseFreeMemoryFunction(
    thunks::ReleaseFreeMemoryFunction release_free_memory_function);

}  // namespace allocator
}  // namespace base

#endif
