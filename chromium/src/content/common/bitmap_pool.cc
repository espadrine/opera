// Copyright (c) 2013 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/bitmap_pool.h"

// Don't ever keep more than MAX_POOL_SIZE bitmaps in memory.
// Discard if overflow. MAX_POOL_SIZE must always be 2^k.
#define MAX_POOL_SIZE 0x10

namespace content {

BitmapPool* BitmapPool::GetInstance() {
  return Singleton<BitmapPool>::get();
}

BitmapPool::BitmapPool()
  : next_bitmap_id_(0) {}

BitmapPool::~BitmapPool() {}

int32 BitmapPool::Push(const SkBitmap& bitmap) {
  base::AutoLock lock(lock_);

  int32 bitmap_id = next_bitmap_id_;
  next_bitmap_id_ = (next_bitmap_id_ + 1) & INT_MAX;
  int32 bitmap_key = bitmap_id & (MAX_POOL_SIZE - 1);

  std::pair<int32, SkBitmap> pair = bitmaps_[bitmap_key];
  if (!pair.second.isNull())
    bitmaps_.erase(bitmap_key);

  bitmaps_[bitmap_key] = std::make_pair(bitmap_id, bitmap);

  return bitmap_id;
}

SkBitmap BitmapPool::Pop(int32 bitmap_id) {
  base::AutoLock lock(lock_);

  SkBitmap bitmap;
  int32 bitmap_key = bitmap_id & (MAX_POOL_SIZE - 1);
  std::pair<int32, SkBitmap> pair = bitmaps_[bitmap_key];
  if (!pair.second.isNull() && pair.first == bitmap_id) {
    bitmap = pair.second;
    bitmaps_.erase(bitmap_key);
  }
  return bitmap;
}

void BitmapPool::Drop(int32 bitmap_id) {
  base::AutoLock lock(lock_);

  bitmaps_.erase(bitmap_id);
}

}  // namespace content
