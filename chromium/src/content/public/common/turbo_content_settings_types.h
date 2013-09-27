// Copyright (c) 2013 Yandex LLC. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_PUBLIC_COMMON_TURBO_CONTENT_SETTINGS_TYPES_H_
#define CONTENT_PUBLIC_COMMON_TURBO_CONTENT_SETTINGS_TYPES_H_
#pragma once

// Information about the content, which can be blocked by "Turbo" mode.
enum TurboContentSettingsType {
  TURBO_CONTENT_SETTINGS_TYPE_HTML5_VIDEO,
  TURBO_CONTENT_SETTINGS_TYPE_HTML5_AUDIO,
  TURBO_CONTENT_SETTINGS_TYPE_PLUGINS,
  TURBO_CONTENT_SETTINGS_NUM_TYPES
};

#endif  // CONTENT_PUBLIC_COMMON_TURBO_CONTENT_SETTINGS_TYPES_H_
