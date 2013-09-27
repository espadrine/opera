// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Modified by Opera Software ASA.
// @copied-from: chromium/src/content/browser/android/media_player_manager_android.h
// @last-synchronized: 1c7494e252ae17ae6287b8143c3bd265bb9ccb4e

#ifndef CONTENT_BROWSER_EXTERNAL_MEDIA_PLAYER_MANAGER_EXTERNAL_H_
#define CONTENT_BROWSER_EXTERNAL_MEDIA_PLAYER_MANAGER_EXTERNAL_H_

#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_view_host_observer.h"

namespace content {

class MediaPlayerManagerExternal : public RenderViewHostObserver {
 public:
  // Create a MediaPlayerManagerExternal object for the |render_view_host|.
  static MediaPlayerManagerExternal* Create(RenderViewHost* render_view_host);

  virtual ~MediaPlayerManagerExternal() {}

  // RenderViewHostObserver overrides.
  virtual bool OnMessageReceived(const IPC::Message& message) = 0;

 protected:
  explicit MediaPlayerManagerExternal(RenderViewHost* render_view_host):
      RenderViewHostObserver(render_view_host) {}
};

}  // namespace content

#endif  // CONTENT_BROWSER_EXTERNAL_MEDIA_PLAYER_MANAGER_EXTERNAL_H_
