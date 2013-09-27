/** Copyright (C) 1995-2013 Opera Software AS.  All rights reserved.
 *
 * This file is part of the Opera TVSDK.
 * It includes Opera proprietary information and distribution is prohibited without
 * Opera Software's prior, explicit and written consent.
 */

#ifndef CONTENT_BROWSER_EXTERNAL_MEDIA_PLAYER_FILTER_EXTERNAL_H_
#define CONTENT_BROWSER_EXTERNAL_MEDIA_PLAYER_FILTER_EXTERNAL_H_

#include "content/public/browser/browser_message_filter.h"
#include "content/public/browser/render_view_host_observer.h"

namespace content {

class MediaPlayerFilterExternal : public BrowserMessageFilter {
 public:
  // Create a MediaPlayerFilterExternal object for the |render_view_host|.
  static MediaPlayerFilterExternal* Create();
  MediaPlayerFilterExternal() {}
  virtual ~MediaPlayerFilterExternal() {}

  // RenderViewHostObserver overrides.
  virtual bool OnMessageReceived(const IPC::Message& message,
                                 bool* message_was_ok) = 0;
};

}  // namespace content

#endif  // CONTENT_BROWSER_EXTERNAL_MEDIA_PLAYER_FILTER_EXTERNAL_H_
