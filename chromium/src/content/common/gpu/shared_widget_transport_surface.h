// Copyright (c) 2013 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_GPU_SHARED_WIDGET_TRANSPORT_SURFACE_H_
#define CONTENT_COMMON_GPU_SHARED_WIDGET_TRANSPORT_SURFACE_H_

#include <map>

#include "base/basictypes.h"
#include "base/memory/weak_ptr.h"
#include "base/memory/linked_ptr.h"
#include "content/common/gpu/gpu_command_buffer_stub.h"
#include "content/common/gpu/image_transport_surface.h"
#include "ui/gl/gl_surface.h"
#include "ui/gl/gl_surface_egl.h"

namespace content {
class GpuChannelManager;

// Dynamically uses a shared widget to render to. When no widget is available
// a 1x1 pbuffer is used. When created and when made visible a widget access
// request is sent to the GPU host process.
class SharedWidgetTransportSurface : public ImageTransportSurface,
                                     public gfx::GLSurface {
 public:
  SharedWidgetTransportSurface(GpuChannelManager* manager,
                               GpuCommandBufferStub* stub);

  // gfx::GLSurface implementation.
  virtual bool Initialize() OVERRIDE;
  virtual void Destroy() OVERRIDE;
  virtual bool IsOffscreen() OVERRIDE;
  virtual void SetLatencyInfo(const ui::LatencyInfo& latency_info) OVERRIDE;
  virtual bool SwapBuffers() OVERRIDE;
  virtual gfx::Size GetSize() OVERRIDE;
  virtual void* GetHandle() OVERRIDE;
  virtual std::string GetExtensions() OVERRIDE;
  virtual bool OnMakeCurrent(gfx::GLContext* context) OVERRIDE;
  virtual void* GetDisplay() OVERRIDE;
  virtual void* GetConfig() OVERRIDE;
  virtual void Activate() OVERRIDE;

 protected:
  // ImageTransportSurface implementation.
  virtual void OnBufferPresented(
      const AcceleratedSurfaceMsg_BufferPresented_Params& params) OVERRIDE;
  virtual void OnResizeViewACK() OVERRIDE;
  virtual void OnResize(gfx::Size size, float scale_factor) OVERRIDE;

  virtual void OnAcceleratedWidgetAcquired(int32 surface_id) OVERRIDE;
  virtual void OnAcceleratedWidgetRelease() OVERRIDE;

 private:
  virtual ~SharedWidgetTransportSurface();

  void ResetOffscreenSurface();

  void CancelBitmapRequest(bool reset_offscreen_surface = true);
  void DoBitmapRequest();
  void BitmapRequestCallback(
      int32 request_id, const gfx::Size& size, bool wait_for_frame);

  void SetNativeWidget(gfx::AcceleratedWidget widget);

  GpuCommandBufferStub* stub_;

  scoped_ptr<ImageTransportHelper> helper_;
  int surface_id_;

  bool acquire_pending_;

  scoped_refptr<gfx::PbufferGLSurfaceEGL> pbuffer_surface_;
  scoped_refptr<gfx::NativeViewGLSurfaceEGL> native_view_surface_;

  // The native widget surface id.
  int widget_surface_id_;

  bool resize_pending_;

  // The onscreen surface used to make the context current.
  gfx::Size size_;
  bool did_set_swap_interval_;

  bool bitmap_requested_;
  int32 bitmap_request_id_;
  gfx::Size bitmap_request_size_;

  DISALLOW_COPY_AND_ASSIGN(SharedWidgetTransportSurface);
};

}  // namespace content

#endif  // CONTENT_COMMON_GPU_SHARED_WIDGET_TRANSPORT_SURFACE_H_
