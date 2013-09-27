// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Modified by Opera Software ASA
// @copied-from chromium/src/content/shell/shell_content_main.cc
// @final-synchronized 92feb11b27e220860e342d9936574616eee0a47e

#include "desktop/mac/opera_content_main.h"

#include "content/public/app/content_main.h"

#include "desktop/common/opera_main_delegate.h"
#include "desktop/mac/crash_handler/crash_handler.h"

int ContentMain(int argc,
                const char** argv) {
  opera::InitCrashHandler(argc, argv);
  OperaMainDelegate delegate;
  return content::ContentMain(argc, argv, &delegate);
}
