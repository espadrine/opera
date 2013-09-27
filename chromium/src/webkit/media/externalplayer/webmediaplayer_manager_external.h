// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Modified by Opera Software ASA.
// @copied-from: chromium/src/webkit/media/android/webmediaplayer_manager_android.h
// @last-synchronized: 1c7494e252ae17ae6287b8143c3bd265bb9ccb4e

#ifndef WEBKIT_MEDIA_EXTERNALPLAYER_WEBMEDIAPLAYER_MANAGER_EXTERNAL_H_
#define WEBKIT_MEDIA_EXTERNALPLAYER_WEBMEDIAPLAYER_MANAGER_EXTERNAL_H_

namespace WebKit {
class WebFrame;
class WebMediaPlayerClient;
class WebMediaPlayer;
}

namespace content {
class RenderView;
}

namespace webkit_media {

// Interface class for managing all the external WebMediaPlayer objects in the
// same RenderView.
class WebMediaPlayerManagerExternal {
 public:
  static WebMediaPlayerManagerExternal* Create(
      content::RenderView* render_view);

  virtual ~WebMediaPlayerManagerExternal() { }

  // Release all the media resources managed by this object unless
  // an audio play is in progress.
  virtual void ReleaseMediaResources() = 0;

  // Create new media player
  virtual WebKit::WebMediaPlayer* CreateMediaPlayer(
      WebKit::WebFrame* frame,
      WebKit::WebMediaPlayerClient* client) = 0;
};

}  // namespace webkit_media

#endif  // WEBKIT_MEDIA_EXTERNALPLAYER_WEBMEDIAPLAYER_MANAGER_EXTERNAL_H_
