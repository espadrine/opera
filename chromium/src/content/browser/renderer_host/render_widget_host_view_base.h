// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_RENDERER_HOST_RENDER_WIDGET_HOST_VIEW_BASE_H_
#define CONTENT_BROWSER_RENDERER_HOST_RENDER_WIDGET_HOST_VIEW_BASE_H_

#if defined(OS_MACOSX)
#include <OpenGL/OpenGL.h>
#endif

#if defined(TOOLKIT_GTK)
#include <gdk/gdk.h>
#endif

#include <string>
#include <vector>

#include "base/memory/scoped_ptr.h"
#include "base/callback_forward.h"
#include "content/common/content_export.h"
#include "content/port/browser/render_widget_host_view_port.h"
#include "ui/base/range/range.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/gfx/rect.h"

namespace content {

class RenderWidgetHostImpl;

// Basic implementation shared by concrete RenderWidgetHostView
// subclasses.
//
// Note that nothing should use this class, except concrete subclasses
// that are deriving from it, and code that is written specifically to
// use one of these concrete subclasses (i.e. platform-specific code).
//
// To enable embedders that add ports, everything else in content/
// should use the RenderWidgetHostViewPort interface.
//
// RenderWidgetHostView class hierarchy described in render_widget_host_view.h.
class CONTENT_EXPORT RenderWidgetHostViewBase
    : public RenderWidgetHostViewPort {
 public:
  virtual ~RenderWidgetHostViewBase();

  // RenderWidgetHostViewPort implementation.
  virtual bool OnMessageReceived(const IPC::Message& msg) OVERRIDE;
  virtual void SelectionChanged(const string16& text,
                                size_t offset,
                                const ui::Range& range) OVERRIDE;
  virtual void SetBackground(const SkBitmap& background) OVERRIDE;
  virtual const SkBitmap& GetBackground() OVERRIDE;
  virtual gfx::Size GetPhysicalBackingSize() const OVERRIDE;
  virtual float GetOverdrawBottomHeight() const OVERRIDE;
  virtual bool IsShowingContextMenu() const OVERRIDE;
  virtual void SetShowingContextMenu(bool showing_menu) OVERRIDE;
  virtual string16 GetSelectedText() const OVERRIDE;
  virtual bool IsMouseLocked() OVERRIDE;
  virtual void UnhandledWheelEvent(
      const WebKit::WebMouseWheelEvent& event) OVERRIDE;
  virtual InputEventAckState FilterInputEvent(
      const WebKit::WebInputEvent& input_event) OVERRIDE;
  virtual void GestureEventAck(int gesture_event_type) OVERRIDE;
  virtual void SetPopupType(WebKit::WebPopupType popup_type) OVERRIDE;
  virtual WebKit::WebPopupType GetPopupType() OVERRIDE;
  virtual BrowserAccessibilityManager*
      GetBrowserAccessibilityManager() const OVERRIDE;
  virtual void ProcessAckedTouchEvent(const TouchEventWithLatencyInfo& touch,
                                      InputEventAckState ack_result) OVERRIDE;
  virtual SmoothScrollGesture* CreateSmoothScrollGesture(
      bool scroll_down, int pixels_to_scroll, int mouse_event_x,
      int mouse_event_y) OVERRIDE;
  virtual bool CanSubscribeFrame() const OVERRIDE;
  virtual void BeginFrameSubscription(
      scoped_ptr<RenderWidgetHostViewFrameSubscriber> subscriber) OVERRIDE;
  virtual void EndFrameSubscription() OVERRIDE;
  virtual void OnSwapCompositorFrame(
      scoped_ptr<cc::CompositorFrame> frame) OVERRIDE {}
  virtual void OnOverscrolled(gfx::Vector2dF accumulated_overscroll,
                              gfx::Vector2dF current_fling_velocity) OVERRIDE;

  virtual void FrameScheduled() OVERRIDE {}
  virtual void SyncBufferSize(const gfx::Size& size) OVERRIDE {}
  virtual bool IsResizing() OVERRIDE;

  void SetBrowserAccessibilityManager(BrowserAccessibilityManager* manager);

  // Notification that a resize or move session ended on the native widget.
  void UpdateScreenInfo(gfx::NativeView view);

#if defined(OS_WIN)
  // The callback that DetachPluginsHelper calls for each child window. Call
  // this directly if you want to do custom filtering on plugin windows first.
  static void DetachPluginWindowsCallback(HWND window);
#endif

 protected:
  // Interface class only, do not construct.
  RenderWidgetHostViewBase();

#if defined(OS_WIN)
  // Shared implementation of MovePluginWindows for use by win and aura/wina.
  static void MovePluginWindowsHelper(
      HWND parent,
      const std::vector<webkit::npapi::WebPluginGeometry>& moves);

  static void PaintPluginWindowsHelper(
      HWND parent,
      const gfx::Rect& damaged_screen_rect);

  // Needs to be called before the HWND backing the view goes away to avoid
  // crashes in Windowed plugins.
  static void DetachPluginsHelper(HWND parent);
#endif

  // Whether this view is a popup and what kind of popup it is (select,
  // autofill...).
  WebKit::WebPopupType popup_type_;

  // A custom background to paint behind the web content. This will be tiled
  // horizontally. Can be null, in which case we fall back to painting white.
  SkBitmap background_;

  // While the mouse is locked, the cursor is hidden from the user. Mouse events
  // are still generated. However, the position they report is the last known
  // mouse position just as mouse lock was entered; the movement they report
  // indicates what the change in position of the mouse would be had it not been
  // locked.
  bool mouse_locked_;

  // Whether we are showing a context menu.
  bool showing_context_menu_;

  // A buffer containing the text inside and around the current selection range.
  string16 selection_text_;

  // The offset of the text stored in |selection_text_| relative to the start of
  // the web page.
  size_t selection_text_offset_;

  // The current selection range relative to the start of the web page.
  ui::Range selection_range_;

protected:
  // The scale factor of the display the renderer is currently on.
  float current_device_scale_factor_;

 private:
  // Manager of the tree representation of the WebKit render tree.
  scoped_ptr<BrowserAccessibilityManager> browser_accessibility_manager_;

  gfx::Rect current_display_area_;

  DISALLOW_COPY_AND_ASSIGN(RenderWidgetHostViewBase);
};

}  // namespace content

#endif  // CONTENT_BROWSER_RENDERER_HOST_RENDER_WIDGET_HOST_VIEW_BASE_H_
