// Copyright (c) 2012 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/android/surface_texture_support.h"

#include "base/android/jni_android.h"
#include "base/logging.h"

using base::android::AttachCurrentThread;
using base::android::ClearException;
using base::android::GetClass;
using base::android::JavaRef;
using base::android::ScopedJavaLocalRef;

namespace media {

// This method was taken from chromium/src/base/android/jni_android.[c,h]
// but since it was removed from there, its kept here until possible
// upstreaming to avoid potential conflicts in future upgrades.
static bool HasMethod(JNIEnv* env, const JavaRef<jclass>& clazz,
                      const char* method_name,
                      const char* jni_signature) {
  jmethodID method_id =
      env->GetMethodID(clazz.obj(), method_name, jni_signature);
  if (!method_id) {
    ClearException(env);
    return false;
  }
  bool error = ClearException(env);
  DCHECK(!error);
  return true;
}

bool IsSurfaceTextureSupported() {
  JNIEnv* env = AttachCurrentThread();
  CHECK(env);

  static enum {
    UNKNOWN,
    YES,
    NO
  } surface_texture_supported = UNKNOWN;

  if (surface_texture_supported == UNKNOWN) {
    ScopedJavaLocalRef<jclass> cls(GetClass(env, "android/media/MediaPlayer"));
    surface_texture_supported =
        HasMethod(env, cls,
                  "setSurface",
                  "(Landroid/view/Surface;)V") ? YES : NO;
  }

  return surface_texture_supported == YES;
}

}  // namespace media
