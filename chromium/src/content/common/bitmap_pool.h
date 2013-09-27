// Copyright (c) 2013 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_BITMAP_POOL_H_
#define CONTENT_COMMON_BITMAP_POOL_H_

#include <map>
#include <utility>

#include "base/memory/singleton.h"
#include "base/synchronization/lock.h"
#include "third_party/skia/include/core/SkBitmap.h"

namespace content {

class BitmapPool {
 public:
  static BitmapPool* GetInstance();

  int32 Push(const SkBitmap& bitmap);
  SkBitmap Pop(int32 bitmap_id);
  void Drop(int32 bitmap_id);

 private:
  BitmapPool();
  ~BitmapPool();

  int32 next_bitmap_id_;
  std::map<int32, std::pair<int32, SkBitmap> > bitmaps_;

  base::Lock lock_;

  friend struct DefaultSingletonTraits<BitmapPool>;

  DISALLOW_COPY_AND_ASSIGN(BitmapPool);
};

}  // namespace content

#endif  // CONTENT_COMMON_BITMAP_POOL_H_
