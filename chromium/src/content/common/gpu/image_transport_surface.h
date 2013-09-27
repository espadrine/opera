// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_GPU_IMAGE_TRANSPORT_SURFACE_H_
#define CONTENT_COMMON_GPU_IMAGE_TRANSPORT_SURFACE_H_

#include <vector>

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/shared_memory.h"
#include "content/common/content_export.h"
#include "ipc/ipc_listener.h"
#include "ipc/ipc_message.h"
#include "ui/base/latency_info.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/gfx/rect.h"
#include "ui/gfx/size.h"
#include "ui/gl/gl_surface.h"

// TODO(jadahl): This puts an incorrect dependency on common -> browser and
// needs to be fixed.
#include "content/public/browser/browser_message_filter.h"

struct AcceleratedSurfaceMsg_BufferPresented_Params;
struct GpuHostMsg_AcceleratedSurfaceBuffersSwapped_Params;
struct GpuHostMsg_AcceleratedSurfacePostSubBuffer_Params;
struct GpuHostMsg_AcceleratedSurfaceRelease_Params;
struct GpuHostMsg_AcceleratedWidgetBuffersSwapped_Params;

namespace gfx {
class GLSurface;
}

namespace gpu {
class GpuScheduler;
class PreemptionFlag;
namespace gles2 {
class GLES2Decoder;
}
}

namespace content {
class GpuChannelManager;
class GpuCommandBufferStub;

// The GPU process is agnostic as to how it displays results. On some platforms
// it renders directly to window. On others it renders offscreen and transports
// the results to the browser process to display. This file provides a simple
// framework for making the offscreen path seem more like the onscreen path.
//
// The ImageTransportSurface class defines an simple interface for events that
// should be responded to. The factory returns an offscreen surface that looks
// a lot like an onscreen surface to the GPU process.
//
// The ImageTransportSurfaceHelper provides some glue to the outside world:
// making sure outside events reach the ImageTransportSurface and
// allowing the ImageTransportSurface to send events to the outside world.

class ImageTransportSurface {
 public:
  ImageTransportSurface();

  virtual void OnBufferPresented(
      const AcceleratedSurfaceMsg_BufferPresented_Params& params) = 0;
  virtual void OnResizeViewACK() = 0;
  virtual void OnResize(gfx::Size size, float scale_factor) = 0;
  virtual void SetLatencyInfo(
      const ui::LatencyInfo& latency_info) = 0;

  virtual void OnAcceleratedWidgetAcquired(int32 surface_id);
  virtual void OnAcceleratedWidgetRelease();

  // Creates a surface with the given attributes.
  static scoped_refptr<gfx::GLSurface> CreateSurface(
      GpuChannelManager* manager,
      GpuCommandBufferStub* stub,
      const gfx::GLSurfaceHandle& handle);

#if defined(OS_MACOSX)
  CONTENT_EXPORT static void SetAllowOSMesaForTesting(bool allow);
#endif

  virtual gfx::Size GetSize() = 0;

 protected:
  virtual ~ImageTransportSurface();

 private:
  // Creates the appropriate native surface depending on the GL implementation.
  // This will be implemented separately by each platform.
  //
  // This will not be called for texture transport surfaces which are
  // cross-platform. The platform implementation should only create the
  // surface and should not initialize it. On failure, a null scoped_refptr
  // should be returned.
  static scoped_refptr<gfx::GLSurface> CreateNativeSurface(
      GpuChannelManager* manager,
      GpuCommandBufferStub* stub,
      const gfx::GLSurfaceHandle& handle);

  DISALLOW_COPY_AND_ASSIGN(ImageTransportSurface);
};

class ImageTransportHelper
    : public IPC::Listener,
      public base::SupportsWeakPtr<ImageTransportHelper> {
 public:
  // Takes weak pointers to objects that outlive the helper.
  ImageTransportHelper(ImageTransportSurface* surface,
                       GpuChannelManager* manager,
                       GpuCommandBufferStub* stub,
                       gfx::PluginWindowHandle handle);
  virtual ~ImageTransportHelper();

  void InitializeOnUIThread(int surface_id);
  bool Initialize();
  void Destroy();

  // IPC::Listener implementation:
  virtual bool OnMessageReceived(const IPC::Message& message) OVERRIDE;

  // Helper send functions. Caller fills in the surface specific params
  // like size and surface id. The helper fills in the rest.
  void SendAcceleratedSurfaceBuffersSwapped(
      GpuHostMsg_AcceleratedSurfaceBuffersSwapped_Params params);
  void SendAcceleratedSurfacePostSubBuffer(
      GpuHostMsg_AcceleratedSurfacePostSubBuffer_Params params);
  void SendAcceleratedSurfaceRelease(
      GpuHostMsg_AcceleratedSurfaceRelease_Params params);
  void SendAcceleratedWidgetAcquire();
  void SendAcceleratedWidgetRelease(int32 surface_id);
  void SendFrame();
  void SendAcceleratedWidgetBuffersSwapped(
      GpuHostMsg_AcceleratedWidgetBuffersSwapped_Params params);
  void SendResizeView(const gfx::Size& size);
  void SendUpdateVSyncParameters(
      base::TimeTicks timebase, base::TimeDelta interval);
  bool SendRequestBitmapResult(int32 request_id, int32 bitmap_id);
  void SendScheduleComposite();

  void SendLatencyInfo(const ui::LatencyInfo& latency_info);

  // Whether or not we should execute more commands.
  void SetScheduled(bool is_scheduled);

  void DeferToFence(base::Closure task);

  void SetPreemptByFlag(
      scoped_refptr<gpu::PreemptionFlag> preemption_flag);

  // Make the surface's context current.
  bool MakeCurrent();

  // Set the default swap interval on the surface.
  static void SetSwapInterval(gfx::GLContext* context);

  void Suspend();

  GpuChannelManager* manager() const { return manager_; }
  GpuCommandBufferStub* stub() const { return stub_.get(); }

  bool enable_frame_fastcase() { return enable_frame_fastcase_; }

 private:
  gpu::GpuScheduler* Scheduler();
  gpu::gles2::GLES2Decoder* Decoder();

  class ImageTransportFilter : public content::BrowserMessageFilter {
   public:
    explicit ImageTransportFilter(ImageTransportHelper* helper)
        : helper_(helper) {}

    virtual bool OnMessageReceived(const IPC::Message& message,
                                   bool* message_was_ok) OVERRIDE;

   private:
    virtual ~ImageTransportFilter() {}

    void OnStartFrameLoop();
    void OnStopFrameLoop();

    ImageTransportHelper* helper_;
  };

  static void UnrefFilterOnUIThread(
      int32 surface_id,
      scoped_refptr<ImageTransportFilter> filter);

  // IPC::Message handlers.
  void OnBufferPresented(
      const AcceleratedSurfaceMsg_BufferPresented_Params& params);
  void OnResizeViewACK();
  void OnAcceleratedWidgetAcquired(int32 surface_id);
  void OnAcceleratedWidgetRelease();

  // Backbuffer resize callback.
  void Resize(gfx::Size size, float scale_factor);

  void SetLatencyInfo(const ui::LatencyInfo& latency_info);

  // Weak pointers that point to objects that outlive this helper.
  ImageTransportSurface* surface_;
  GpuChannelManager* manager_;

  base::WeakPtr<GpuCommandBufferStub> stub_;
  int32 route_id_;
  gfx::PluginWindowHandle handle_;

  scoped_refptr<ImageTransportFilter> filter_;
  bool enable_frame_fastcase_;
  int32 widget_host_routing_id_;

  DISALLOW_COPY_AND_ASSIGN(ImageTransportHelper);
};

// An implementation of ImageTransportSurface that implements GLSurface through
// GLSurfaceAdapter, thereby forwarding GLSurface methods through to it.
class PassThroughImageTransportSurface
    : public gfx::GLSurfaceAdapter,
      public ImageTransportSurface {
 public:
  PassThroughImageTransportSurface(GpuChannelManager* manager,
                                   GpuCommandBufferStub* stub,
                                   gfx::GLSurface* surface,
                                   bool transport);

  // GLSurface implementation.
  virtual bool Initialize() OVERRIDE;
  virtual void Destroy() OVERRIDE;
  virtual bool DeferDraws() OVERRIDE;
  virtual bool SwapBuffers() OVERRIDE;
  virtual bool PostSubBuffer(int x, int y, int width, int height) OVERRIDE;
  virtual bool OnMakeCurrent(gfx::GLContext* context) OVERRIDE;

  // ImageTransportSurface implementation.
  virtual void OnBufferPresented(
      const AcceleratedSurfaceMsg_BufferPresented_Params& params) OVERRIDE;
  virtual void OnResizeViewACK() OVERRIDE;
  virtual void OnResize(gfx::Size size, float scale_factor) OVERRIDE;
  virtual gfx::Size GetSize() OVERRIDE;
  virtual void SetLatencyInfo(
      const ui::LatencyInfo& latency_info) OVERRIDE;

 protected:
  virtual ~PassThroughImageTransportSurface();

  // If updated vsync parameters can be determined, send this information to
  // the browser.
  virtual void SendVSyncUpdateIfAvailable();

 private:
  scoped_ptr<ImageTransportHelper> helper_;
  gfx::Size new_size_;
  bool transport_;
  bool did_set_swap_interval_;
  bool did_unschedule_;
  bool is_swap_buffers_pending_;
  ui::LatencyInfo latency_info_;

  DISALLOW_COPY_AND_ASSIGN(PassThroughImageTransportSurface);
};

}  // namespace content

#endif  // CONTENT_COMMON_GPU_IMAGE_TRANSPORT_SURFACE_H_
