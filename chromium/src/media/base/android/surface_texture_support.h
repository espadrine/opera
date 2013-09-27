// Copyright (c) 2012 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_ANDROID_SURFACE_TEXTURE_SUPPORT_H_
#define MEDIA_BASE_ANDROID_SURFACE_TEXTURE_SUPPORT_H_

#include "media/base/media_export.h"

namespace media {

  // Are android.graphics.SurfaceTexture and
  // android.media.MediaPlayer.setSurface supported?
  MEDIA_EXPORT bool IsSurfaceTextureSupported();

}  // namespace media

#endif  // MEDIA_BASE_ANDROID_SURFACE_TEXTURE_SUPPORT_H_
