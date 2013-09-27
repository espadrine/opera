// Copyright (c) 2012 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_SCROLL_OFFSET_SAMPLER_H_
#define CONTENT_RENDERER_SCROLL_OFFSET_SAMPLER_H_

#include "content/public/renderer/render_view_observer.h"
#include "ui/gfx/point.h"

namespace content {

class RenderViewImpl;

// A helper to keep track of the movements of the scroll view as a (likely)
// result of a fling gesture.
//
// If need to extend this arises, have a look at
// ui/base/gestures/velocity_calculator.h
class ScrollOffsetSampler : public RenderViewObserver {
 public:
  explicit ScrollOffsetSampler(RenderViewImpl* renderViewImpl);

  // RenderViewObserver
  virtual bool OnMessageReceived(const IPC::Message& message) OVERRIDE;
  virtual void DidChangeScrollOffset(WebKit::WebFrame* frame) OVERRIDE;

 protected:
  void OnSampleTimeout();

  // Try to fetch the current scroll offset. Returns true on success
  bool GetScrollOffset(gfx::Point* point);

 private:
  RenderViewImpl* render_view_impl_;

  gfx::Point initial_point_;
  gfx::Point final_point_;

  // Whether sampling of the scroll offset is taking place
  bool sampling_;
  // Flag indicating whether all intermediate offset changes were in the same
  // direction as the total offset change
  bool consistent_direction_;

  DISALLOW_COPY_AND_ASSIGN(ScrollOffsetSampler);
};

}  // namespace content

#endif  // CONTENT_RENDERER_SCROLL_OFFSET_SAMPLER_H_
