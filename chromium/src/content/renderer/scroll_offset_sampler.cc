// Copyright (c) 2012 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/renderer/scroll_offset_sampler.h"

#include "base/message_loop/message_loop.h"
#include "base/message_loop/message_loop_proxy.h"
#include "content/common/view_messages.h"
#include "content/renderer/render_view_impl.h"
#include "ui/gfx/point_conversions.h"

#include "third_party/WebKit/public/platform/WebSize.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include "third_party/WebKit/public/web/WebInputEvent.h"
#include "third_party/WebKit/public/web/WebView.h"

namespace content {

namespace {

static const int SAMPLE_INTERVAL_IN_MS = 100;

}  // namespace

ScrollOffsetSampler::ScrollOffsetSampler(RenderViewImpl* renderViewImpl)
  : RenderViewObserver(renderViewImpl),
    render_view_impl_(renderViewImpl),
    sampling_(false),
    consistent_direction_(true) {
  DCHECK(renderViewImpl);
}

bool ScrollOffsetSampler::OnMessageReceived(const IPC::Message& message) {
  if (!sampling_ && message.type() == ViewMsg_SampleScrollSpeed::ID) {
    if (!GetScrollOffset(&initial_point_))
      return false;

    final_point_ = initial_point_;
    sampling_ = true;
    consistent_direction_ = true;

    base::MessageLoop::current()->PostDelayedTask(
        FROM_HERE,
        base::Bind(&ScrollOffsetSampler::OnSampleTimeout,
                   base::Unretained(this)),
        base::TimeDelta::FromMilliseconds(SAMPLE_INTERVAL_IN_MS));
  }
  return false;
}

void ScrollOffsetSampler::DidChangeScrollOffset(WebKit::WebFrame* frame) {
  gfx::Point current_point = final_point_;
  GetScrollOffset(&final_point_);

  bool total_is_downwards = final_point_.y() >= initial_point_.y();
  bool sample_is_downwards = final_point_.y() >= current_point.y();

  consistent_direction_ = consistent_direction_ &&
      total_is_downwards == sample_is_downwards;
}

void ScrollOffsetSampler::OnSampleTimeout() {
  WebKit::WebFrame* frame = NULL;

  sampling_ = false;

  if (!render_view_impl_->webview() ||
      !(frame = render_view_impl_->webview()->mainFrame()))
    return;

  if (!consistent_direction_)
    return;

  WebKit::WebSize vSize = frame->contentsSize();

  gfx::Point delta(final_point_.x() - initial_point_.x(),
                   final_point_.y() - initial_point_.y());
  gfx::Point viewSize(vSize.width, vSize.height);

  const float page_scale = render_view_impl_->webview()->pageScaleFactor();

  render_view_impl_->Send(
      new ViewHostMsg_SampledScrollSpeed(
          render_view_impl_->GetRoutingID(),
          gfx::ToRoundedPoint(gfx::ScalePoint(delta, page_scale)),
          gfx::ToRoundedPoint(gfx::ScalePoint(initial_point_, page_scale)),
          gfx::ToRoundedPoint(gfx::ScalePoint(viewSize, page_scale)),
          SAMPLE_INTERVAL_IN_MS));
}

bool ScrollOffsetSampler::GetScrollOffset(gfx::Point* point) {
  DCHECK(point);
  WebKit::WebFrame* frame = NULL;

  if (!render_view_impl_->webview() ||
      !(frame = render_view_impl_->webview()->mainFrame()))
    return false;

  point->SetPoint(frame->scrollOffset().width,
                  frame->scrollOffset().height);
  return true;
}

}  // namespace content
