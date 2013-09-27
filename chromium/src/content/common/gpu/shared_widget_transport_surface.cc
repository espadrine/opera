// Copyright (c) 2013 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/gpu/shared_widget_transport_surface.h"

#include <string>
#include <vector>

#include "base/command_line.h"
#include "content/common/bitmap_pool.h"
#include "content/common/gpu/gpu_channel_manager.h"
#include "content/common/gpu/gpu_messages.h"
#include "content/common/gpu/gpu_surface_tracker.h"
#include "content/common/gpu/swap_and_swizzle.h"
#include "content/common/gpu/sync_point_manager.h"
#include "gpu/command_buffer/service/gpu_scheduler.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_surface_egl.h"

namespace content {

SharedWidgetTransportSurface::SharedWidgetTransportSurface(
    GpuChannelManager* manager,
    GpuCommandBufferStub* stub)
    : stub_(stub),
      acquire_pending_(false),
      pbuffer_surface_(new gfx::PbufferGLSurfaceEGL(gfx::Size(1, 1))),
      resize_pending_(false),
      size_(gfx::Size()),
      did_set_swap_interval_(false),
      bitmap_requested_(false)
{
  helper_.reset(new ImageTransportHelper(this,
                                         manager,
                                         stub,
                                         gfx::kNullPluginWindow));
  surface_id_ = stub->surface_id();
}

SharedWidgetTransportSurface::~SharedWidgetTransportSurface() {
  Destroy();
}

bool SharedWidgetTransportSurface::Initialize() {
  if (!pbuffer_surface_->Initialize())
    return false;

  if (!helper_->Initialize())
    return false;

  stub_->SetBitmapRequestHandler(
      base::Bind(&SharedWidgetTransportSurface::BitmapRequestCallback,
                 base::Unretained(this)));

  return true;
}

void SharedWidgetTransportSurface::Destroy() {
  if (!IsOffscreen())
    helper_->SendAcceleratedWidgetRelease(widget_surface_id_);

  if (bitmap_requested_)
    CancelBitmapRequest();

  helper_->Destroy();
}

bool SharedWidgetTransportSurface::IsOffscreen() {
  return !!pbuffer_surface_.get();
}

bool SharedWidgetTransportSurface::OnMakeCurrent(gfx::GLContext* context) {
  if (!did_set_swap_interval_) {
    ImageTransportHelper::SetSwapInterval(context);
    did_set_swap_interval_ = true;
  }
  return true;
}

void* SharedWidgetTransportSurface::GetDisplay() {
  if (IsOffscreen())
    return pbuffer_surface_->GetDisplay();
  else
    return native_view_surface_->GetDisplay();
}

void* SharedWidgetTransportSurface::GetConfig() {
  if (IsOffscreen())
    return pbuffer_surface_->GetConfig();
  else
    return native_view_surface_->GetConfig();
}

void SharedWidgetTransportSurface::OnResize(gfx::Size size, float scale_factor) {
  size_ = size;
  resize_pending_ = false;
}

void SharedWidgetTransportSurface::SetLatencyInfo(
    const ui::LatencyInfo& latency_info) {
  // TODO(jadahl): implement
}

bool SharedWidgetTransportSurface::SwapBuffers() {
  if (bitmap_requested_) {
    bitmap_requested_ = false;
    DoBitmapRequest();
  }

  if (!IsOffscreen()) {
    gfx::Size eglsize = native_view_surface_->GetSize();
    if (eglsize != size_)
      VLOG(0) << "eglSwapBuffers, EGLSurface size "
          << eglsize.width() << "x" << eglsize.height()
          << " differs from SWTS size "
          << size_.width() << "x" << size_.height();

    glFlush();
    if (native_view_surface_->SwapBuffers()) {
      bool inhibit_frame = false;
      gfx::Size next_eglsize = native_view_surface_->GetSize();

      if (stub_->decoder() && stub_->decoder()->GetContextGroup() &&
          stub_->decoder()->GetContextGroup()->feature_info() &&
          stub_->decoder()->GetContextGroup()->feature_info()->workarounds().
          restore_scissor_on_fbo_change) {
        glScissor(0, 0, next_eglsize.width(), next_eglsize.height());
      }

      if (eglsize != next_eglsize) {
        resize_pending_ = true;
      } else if (!resize_pending_ && helper_->enable_frame_fastcase()) {
        helper_->SendFrame();
        inhibit_frame = true;
      }

      GpuHostMsg_AcceleratedWidgetBuffersSwapped_Params params;
      params.next_buffer_size = next_eglsize;
      params.inhibit_frame = inhibit_frame;
      params.was_offscreen = false;
      helper_->SendAcceleratedWidgetBuffersSwapped(params);

      return true;
    } else {
      GpuHostMsg_AcceleratedWidgetBuffersSwapped_Params params;
      params.next_buffer_size = gfx::Size();
      params.inhibit_frame = false;
      params.was_offscreen = false;
      helper_->SendAcceleratedWidgetBuffersSwapped(params);
      LOG(ERROR) << "SwapBuffers() has failed. Ignore.";
      // Sometimes while destroying native window, SwapBuffers() might
      // fail. To avoid recreating whole context we must return true.
      // Keeping a reference to ANativeWindow does not help and
      // there is still an error from BufferQueue:
      // "[SurfaceView] queueBuffer: SurfaceTexture has been abandoned!"
      // Let's also detach from SurfaceView immediately. SWTS is going
      // to be destroyed soon anyway.
      SetNativeWidget(gfx::kNullAcceleratedWidget);
      helper_->SendAcceleratedWidgetRelease(widget_surface_id_);
      return true;
    }
  } else {
    GpuHostMsg_AcceleratedWidgetBuffersSwapped_Params params;
    params.next_buffer_size = gfx::Size();
    params.inhibit_frame = false;
    params.was_offscreen = true;
    helper_->SendAcceleratedWidgetBuffersSwapped(params);
    return true;
  }
}

gfx::Size SharedWidgetTransportSurface::GetSize() {
  // OSMesa expects a non-zero size.
  return gfx::Size(size_.width() == 0 ? 1 : size_.width(),
                   size_.height() == 0 ? 1 : size_.height());
}

void* SharedWidgetTransportSurface::GetHandle() {
  if (IsOffscreen())
    return pbuffer_surface_->GetHandle();
  else
    return native_view_surface_->GetHandle();
}

std::string SharedWidgetTransportSurface::GetExtensions() {
  // TODO(jadahl): Add support for GL_CHROMIUM_post_sub_buffer
  return std::string("");
}

void SharedWidgetTransportSurface::OnBufferPresented(
    const AcceleratedSurfaceMsg_BufferPresented_Params& params) {
  NOTREACHED();
}

void SharedWidgetTransportSurface::OnResizeViewACK() {
  NOTREACHED();
}

void SharedWidgetTransportSurface::ResetOffscreenSurface() {
  if (pbuffer_surface_->GetSize() == gfx::Size(1, 1))
    return;
  pbuffer_surface_->Resize(gfx::Size(1, 1));
}

void SharedWidgetTransportSurface::DoBitmapRequest() {
  if (!helper_->MakeCurrent()) {
    LOG(ERROR) << "Couldn't make context current";
    CancelBitmapRequest();
    return;
  }

  // Get the preferred read-back format.
  GLint format, type;
  glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &format);
  glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &type);
  if (format != GL_BGRA_EXT || type != GL_UNSIGNED_BYTE) {
    format = GL_RGBA;
    type = GL_UNSIGNED_BYTE;
  }

  int width = bitmap_request_size_.width();
  int height = bitmap_request_size_.height();
  int byte_stride = 4 * width;

  SkBitmap bitmap;
  bitmap.setConfig(SkBitmap::kARGB_8888_Config, width, height);
  if (!bitmap.allocPixels()) {
    LOG(ERROR) << "Cannot allocate bitmap buffer";
    CancelBitmapRequest();
    return;
  }

  bitmap.lockPixels();

  uint8_t* pixels = static_cast<uint8_t*>(bitmap.getPixels());
  glReadPixels(0, 0, width, height, format, type, pixels);

  // Flip y axis and, if necessary, swizzle color components (BGR<->RGB).
  uint8_t* row1 = pixels;
  uint8_t* row2 = pixels + (height - 1) * byte_stride;
  if (format == GL_RGBA) {
    for (int y = 0; y < height/2; y++) {
      SwapRows(row1, row2, width);
      row1 += byte_stride;
      row2 -= byte_stride;
    }
  } else {
    for (int y = 0; y < height/2; y++) {
      SwapRowsAndSwizzle(row1, row2, width);
      row1 += byte_stride;
      row2 -= byte_stride;
    }
    if (height & 1) {
      // If there is an odd number of rows, we need to color-swizzle the middle
      // row separately since it was never swapped.
      SwizzleColors(row1, width);
    }
  }

  bitmap.unlockPixels();

  // Put the bitmap in a shared bitmap tracker and release ownership.
  int32 bitmap_id = BitmapPool::GetInstance()->Push(bitmap);

  if (!helper_->SendRequestBitmapResult(bitmap_request_id_, bitmap_id))
    BitmapPool::GetInstance()->Drop(bitmap_id);

  if (IsOffscreen())
    ResetOffscreenSurface();
}

void SharedWidgetTransportSurface::CancelBitmapRequest(
    bool reset_offscreen_surface) {
  bitmap_requested_ = false;
  helper_->SendRequestBitmapResult(bitmap_request_id_, -1);
  if (IsOffscreen() && reset_offscreen_surface)
    ResetOffscreenSurface();
}

void SharedWidgetTransportSurface::BitmapRequestCallback(
    int32 request_id, const gfx::Size& size, bool wait_for_frame) {
  if (bitmap_requested_)
    CancelBitmapRequest(false);

  // To get proper content into the bitmap wait until before we call
  // SwapBuffers().
  bitmap_requested_ = true;
  bitmap_request_id_ = request_id;
  bitmap_request_size_ = size;

  // If the bitmap was requested when the surface is offscreen, create a full
  // sized pbuffer surface keeping it until the bitmap has been created.
  if (IsOffscreen() && !size.IsEmpty()) {
    pbuffer_surface_->Resize(size);
  }

  if (!wait_for_frame) {
    // To make sure we will receive a future SwapBuffers() call schedule
    // a composite.
    helper_->SendScheduleComposite();
  }
}

void SharedWidgetTransportSurface::OnAcceleratedWidgetAcquired(
    int32 surface_id) {
  acquire_pending_ = false;

  if (surface_id == 0)
    return;

  widget_surface_id_ = surface_id;

  gfx::AcceleratedWidget widget =
      GpuSurfaceTracker::Get()->GetNativeWidget(surface_id);
  SetNativeWidget(widget);

  helper_->SendScheduleComposite();
}

void SharedWidgetTransportSurface::OnAcceleratedWidgetRelease() {
  if (!IsOffscreen()) {
    SetNativeWidget(gfx::kNullAcceleratedWidget);
    helper_->SendAcceleratedWidgetRelease(widget_surface_id_);
  }
}

void SharedWidgetTransportSurface::Activate() {
  if (IsOffscreen()) {
    if (acquire_pending_)
      return;

    helper_->SendAcceleratedWidgetAcquire();
    acquire_pending_ = true;
  }
}

void SharedWidgetTransportSurface::SetNativeWidget(
    gfx::AcceleratedWidget widget) {
  DCHECK((widget == gfx::kNullAcceleratedWidget &&
          native_view_surface_.get()) ||
         (widget != gfx::kNullAcceleratedWidget &&
          !native_view_surface_.get()));

  // If the current context is the current one, release it while we recreate the
  // surface backend.
  gfx::GLContext* current_context = gfx::GLContext::GetCurrent();
  bool was_current = current_context && current_context->IsCurrent(this);

  // If we are about to release the native widget, clear the frame buffer first.
  if (widget == gfx::kNullAcceleratedWidget &&
      !IsOffscreen() &&
      was_current) {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    native_view_surface_->SwapBuffers();
  }

  if (was_current)
    current_context->ReleaseCurrent(this);

  if (widget != gfx::kNullAcceleratedWidget) {
    native_view_surface_ = new gfx::NativeViewGLSurfaceEGL(widget);
    if (native_view_surface_->Initialize()) {
      pbuffer_surface_->Destroy();
      pbuffer_surface_ = NULL;
    } else {
      native_view_surface_ = NULL;
      LOG(ERROR) << "Couldn't transform SharedWidgetTransportSurface to use "
                 << "the native widget, keep using 1x1 pbuffer.";
    }
  } else {
    native_view_surface_->Destroy();
    native_view_surface_ = NULL;
    gfx::Size size = bitmap_requested_ ? bitmap_request_size_ : gfx::Size(1, 1);
    pbuffer_surface_ = new gfx::PbufferGLSurfaceEGL(size);
    if (!pbuffer_surface_->Initialize()) {
      LOG(FATAL) << "Couldn't reinitialize pbuffer surface.";
      pbuffer_surface_ = NULL;
    }
  }

  if (was_current) {
    current_context->MakeCurrent(this);
    if (!IsOffscreen()) {
      // WAM-1469
      glClearColor(1.0, 1.0, 1.0, 1.0);
      glClear(GL_COLOR_BUFFER_BIT);
      native_view_surface_->SwapBuffers();
    }
  }

  helper_->manager()->gpu_memory_manager()->ScheduleManage(
      GpuMemoryManager::kScheduleManageNow);
}

}  // namespace content
