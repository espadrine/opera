// Copyright (c) 2012-2013 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sys/types.h>
#include <unistd.h>

#include "base/android/jni_android.h"
#include "base/android/jni_registrar.h"
#include "base/logging.h"
#include "content/browser/android/native_fd_parcelable.h"
#include "jni/NativeFDParcelable_jni.h"

// WARNING
//
// This is dangerous and not guaranteed to work. It depends on that the
// implementation of dalvikvm running on the device happens to have an
// 'int' field in the Java class java.io.FileDescriptor that contains the
// actual Linux file descriptor. If it doesn't, everything will crash and
// burn up in flames.
static struct file_descriptor_offsets_t {
  jfieldID mDescriptor;
} gFileDescriptorOffsets;

namespace content {

void SetFD(JNIEnv* env,
           jclass clazz,
           jobject fileDescriptor,
           jint fd) {
  env->SetIntField(fileDescriptor, gFileDescriptorOffsets.mDescriptor, fd);
}

jint GetFD(JNIEnv* env,
           jclass clazz,
           jobject fileDescriptor) {
  return env->GetIntField(fileDescriptor, gFileDescriptorOffsets.mDescriptor);
}

jint Dup(JNIEnv* env,
         jclass clazz,
         jint fd) {
  return dup(fd);
};

void Close(JNIEnv* env,
           jclass clazz,
           jint fd) {
  close(fd);
}

bool RegisterNativeFDParcelable(JNIEnv* env) {
  jclass clazz = env->FindClass("java/io/FileDescriptor");
  if (!clazz) {
    LOG(ERROR) << "Unable to find class java.io.FileDescriptor";
    return false;
  }

  gFileDescriptorOffsets.mDescriptor =
      env->GetFieldID(clazz, "descriptor", "I");
  if (!gFileDescriptorOffsets.mDescriptor) {
    LOG(ERROR) << "Unable to find descriptor field in java.io.FileDescriptor";
    return false;
  }

  return RegisterNativesImpl(env);
}

}  // namespace content
