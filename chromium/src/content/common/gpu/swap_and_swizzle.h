// Copyright (c) 2013 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_GPU_SWAP_AND_SWIZZLE_H_
#define CONTENT_COMMON_GPU_SWAP_AND_SWIZZLE_H_

#ifdef __cplusplus
extern "C" {
#endif

/// Swizzle the color components in all pixles in a row.
///
/// @param row Points to the first pixel in the row.
/// @param count Number of pixels in the row.
void SwizzleColors(void* row, unsigned count);

/// Swap two rows with each other.
///
/// @param rowA Points to the first pixel in the first row.
/// @param rowB Points to the first pixel in the second row.
/// @param count Number of pixels in the row.
void SwapRows(void* rowA, void* rowB, unsigned count);

/// Swap two rows with each other, and swizzle the color components of all the
/// pixels in the process.
///
/// @param rowA Points to the first pixel in the first row.
/// @param rowB Points to the first pixel in the second row.
/// @param count Number of pixels in the row.
void SwapRowsAndSwizzle(void* rowA, void* rowB, unsigned count);

#ifdef __cplusplus
}
#endif

#endif // CONTENT_COMMON_GPU_SWAP_AND_SWIZZLE_H_
