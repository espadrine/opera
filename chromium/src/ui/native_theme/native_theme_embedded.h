// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Modified by Opera Software ASA

#ifndef UI_NATIVE_THEME_NATIVE_THEME_EMBEDDED_H_
#define UI_NATIVE_THEME_NATIVE_THEME_EMBEDDED_H_

#include "ui/native_theme/native_theme_base.h"

namespace ui {

// Embedded implementation of native theme support.
class NativeThemeEmbedded : public NativeThemeBase {
 public:
  static NativeThemeEmbedded* instance();

  virtual SkColor GetSystemColor(ColorId color_id) const OVERRIDE;

 private:
  NativeThemeEmbedded();
  virtual ~NativeThemeEmbedded();

  DISALLOW_COPY_AND_ASSIGN(NativeThemeEmbedded);
};

}  // namespace ui

#endif  // UI_NATIVE_THEME_NATIVE_THEME_EMBEDDED_H_
