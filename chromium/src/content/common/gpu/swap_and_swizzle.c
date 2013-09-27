// Copyright (c) 2013 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>

#include "build/build_config.h"

typedef uint32_t PIXEL;

void SwizzleColors(void* row, unsigned count) {
  PIXEL *ptr = (PIXEL*) row;
  const PIXEL mask1 = 0x00ff00ff,  mask2 = 0xff00ff00;
  while (count--) {
    PIXEL col = *ptr;
#if defined(ARCH_CPU_LITTLE_ENDIAN)
    PIXEL x = col & mask1;
    *ptr++ = (col & mask2) | (x << 16) | (x >> 16);
#else
    PIXEL x = col & mask2;
    *ptr++ = (col & mask1) | (x << 16) | (x >> 16);
#endif
  }
}

void SwapRows(void* rowA, void* rowB, unsigned count) {
  PIXEL *ptrA = (PIXEL*) rowA;
  PIXEL *ptrB = (PIXEL*) rowB;
  while (count >= 4) {
    PIXEL tmp1 = ptrA[0];
    PIXEL tmp2 = ptrA[1];
    PIXEL tmp3 = ptrA[2];
    PIXEL tmp4 = ptrA[3];
    *ptrA++ = ptrB[0];
    *ptrA++ = ptrB[1];
    *ptrA++ = ptrB[2];
    *ptrA++ = ptrB[3];
    *ptrB++ = tmp1;
    *ptrB++ = tmp2;
    *ptrB++ = tmp3;
    *ptrB++ = tmp4;
    count -= 4;
  }
  while (count--) {
    PIXEL tmp = *ptrA;
    *ptrA++ = *ptrB;
    *ptrB++ = tmp;
  }
}

void SwapRowsAndSwizzle(void* rowA, void* rowB, unsigned count) {
  PIXEL *ptrA = (PIXEL*) rowA;
  PIXEL *ptrB = (PIXEL*) rowB;
  const PIXEL mask1 = 0x00ff00ff,  mask2 = 0xff00ff00;
  while (count--) {
    PIXEL colA = *ptrA;
    PIXEL colB = *ptrB;
#if defined(ARCH_CPU_LITTLE_ENDIAN)
    PIXEL xA = colA & mask1;
    PIXEL xB = colB & mask1;
    *ptrA++ = (colB & mask2) | (xB << 16) | (xB >> 16);
    *ptrB++ = (colA & mask2) | (xA << 16) | (xA >> 16);
#else
    PIXEL xA = colA & mask2;
    PIXEL xB = colB & mask2;
    *ptrA++ = (colB & mask1) | (xB << 16) | (xB >> 16);
    *ptrB++ = (colA & mask1) | (xA << 16) | (xA >> 16);
#endif
  }
}

