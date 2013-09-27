// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_ALLOCATOR_ALLOCATOR_THUNKS_EXTENSION_H
#define BASE_ALLOCATOR_ALLOCATOR_THUNKS_EXTENSION_H

#include <stddef.h> // for size_t

#ifdef _MSC_VER
typedef __int64 int64;
#else
#include <stdint.h>  // for int64_t
typedef int64_t int64;
#endif

namespace base {
namespace allocator {
namespace thunks {

// WARNING: You probably don't want to use this file unless you are routing a
// new allocator extension from a specific allocator implementation to base.
// See allocator_extension.h to see the interface that base exports.

typedef bool (*GetAllocatorWasteSizeFunction)(size_t* size);
void SetGetAllocatorWasteSizeFunction(
    GetAllocatorWasteSizeFunction get_allocator_waste_size_function);
GetAllocatorWasteSizeFunction GetGetAllocatorWasteSizeFunction();

typedef void (*GetStatsFunction)(char* buffer, int buffer_length);
void SetGetStatsFunction(GetStatsFunction get_stats_function);
GetStatsFunction GetGetStatsFunction();

typedef bool (*MemoryCollarFunction)(int64 diff);
typedef void (*SetMemoryCollarFunctionFunction)(MemoryCollarFunction f);
void SetSetMemoryCollarFunctionFunction(
    SetMemoryCollarFunctionFunction set_memory_collar_function_function);
SetMemoryCollarFunctionFunction GetSetMemoryCollarFunctionFunction();

typedef void (*ReleaseFreeMemoryFunction)();
void SetReleaseFreeMemoryFunction(
    ReleaseFreeMemoryFunction release_free_memory_function);
ReleaseFreeMemoryFunction GetReleaseFreeMemoryFunction();

}  // namespace thunks
}  // namespace allocator
}  // namespace base

#endif
