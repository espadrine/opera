// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_RENDER_THREAD_IMPL_H_
#define CONTENT_RENDERER_RENDER_THREAD_IMPL_H_

#include <set>
#include <string>
#include <vector>

#include "base/observer_list.h"
#include "base/strings/string16.h"
#include "base/timer.h"
#include "build/build_config.h"
#include "content/child/child_process.h"
#include "content/child/child_thread.h"
#include "content/common/content_export.h"
#include "content/common/gpu/client/gpu_channel_host.h"
#include "content/common/gpu/gpu_process_launch_causes.h"
#include "content/public/renderer/render_thread.h"
#include "ipc/ipc_channel_proxy.h"
#include "ui/gfx/native_widget_types.h"

class GrContext;
class SkBitmap;
struct ViewMsg_New_Params;

namespace WebKit {
class WebGraphicsContext3D;
class WebMediaStreamCenter;
class WebMediaStreamCenterClient;
}

namespace base {
class MessageLoopProxy;
class Thread;

#if defined(OS_WIN)
namespace win {
class ScopedCOMInitializer;
}
#endif
}

namespace cc {
class ContextProvider;
}

namespace IPC {
class ForwardingMessageFilter;
}

namespace media {
class AudioHardwareConfig;
}

namespace v8 {
class Extension;
}

namespace webkit {
namespace gpu {
class GrContextForWebGraphicsContext3D;
}
}

namespace content {

class AppCacheDispatcher;
class AudioInputMessageFilter;
class AudioMessageFilter;
class AudioRendererMixerManager;
class ContextProviderCommandBuffer;
class DBMessageFilter;
class DevToolsAgentFilter;
class DomStorageDispatcher;
class GpuChannelHost;
class IndexedDBDispatcher;
class InputEventFilter;
class InputHandlerManager;
class MediaStreamCenter;
class MediaStreamDependencyFactory;
class MIDIMessageFilter;
class P2PSocketDispatcher;
class PeerConnectionTracker;
class RendererWebKitPlatformSupportImpl;
class RenderProcessObserver;
class VideoCaptureImplManager;
class WebDatabaseObserverImpl;
class WebGraphicsContext3DCommandBufferImpl;

// The RenderThreadImpl class represents a background thread where RenderView
// instances live.  The RenderThread supports an API that is used by its
// consumer to talk indirectly to the RenderViews and supporting objects.
// Likewise, it provides an API for the RenderViews to talk back to the main
// process (i.e., their corresponding WebContentsImpl).
//
// Most of the communication occurs in the form of IPC messages.  They are
// routed to the RenderThread according to the routing IDs of the messages.
// The routing IDs correspond to RenderView instances.
class CONTENT_EXPORT RenderThreadImpl : public RenderThread,
                                        public ChildThread,
                                        public GpuChannelHostFactory {
 public:
  static RenderThreadImpl* current();

  RenderThreadImpl();
  // Constructor that's used when running in single process mode.
  explicit RenderThreadImpl(const std::string& channel_name);
  virtual ~RenderThreadImpl();
  virtual void Shutdown() OVERRIDE;

  // When initializing WebKit, ensure that any schemes needed for the content
  // module are registered properly.  Static to allow sharing with tests.
  static void RegisterSchemes();

  // RenderThread implementation:
  virtual bool Send(IPC::Message* msg) OVERRIDE;
  virtual base::MessageLoop* GetMessageLoop() OVERRIDE;
  virtual IPC::SyncChannel* GetChannel() OVERRIDE;
  virtual std::string GetLocale() OVERRIDE;
  virtual IPC::SyncMessageFilter* GetSyncMessageFilter() OVERRIDE;
  virtual scoped_refptr<base::MessageLoopProxy> GetIOMessageLoopProxy()
      OVERRIDE;
  virtual void AddRoute(int32 routing_id, IPC::Listener* listener) OVERRIDE;
  virtual void RemoveRoute(int32 routing_id) OVERRIDE;
  virtual int GenerateRoutingID() OVERRIDE;
  virtual void AddFilter(IPC::ChannelProxy::MessageFilter* filter) OVERRIDE;
  virtual void RemoveFilter(IPC::ChannelProxy::MessageFilter* filter) OVERRIDE;
  virtual void SetOutgoingMessageFilter(
      IPC::ChannelProxy::OutgoingMessageFilter* filter) OVERRIDE;
  virtual void AddObserver(RenderProcessObserver* observer) OVERRIDE;
  virtual void RemoveObserver(RenderProcessObserver* observer) OVERRIDE;
  virtual void SetResourceDispatcherDelegate(
      ResourceDispatcherDelegate* delegate) OVERRIDE;
  virtual void WidgetHidden() OVERRIDE;
  virtual void WidgetRestored() OVERRIDE;
  virtual void EnsureWebKitInitialized() OVERRIDE;
  virtual void RecordUserMetrics(const std::string& action) OVERRIDE;
  virtual scoped_ptr<base::SharedMemory> HostAllocateSharedMemoryBuffer(
      size_t buffer_size) OVERRIDE;
  virtual void RegisterExtension(v8::Extension* extension) OVERRIDE;
  virtual void ScheduleIdleHandler(int64 initial_delay_ms) OVERRIDE;
  virtual void IdleHandler() OVERRIDE;
  virtual int64 GetIdleNotificationDelayInMs() const OVERRIDE;
  virtual void SetIdleNotificationDelayInMs(
      int64 idle_notification_delay_in_ms) OVERRIDE;
  virtual void ToggleWebKitSharedTimer(bool suspend) OVERRIDE;
  virtual void UpdateHistograms(int sequence_number) OVERRIDE;
  virtual int PostTaskToAllWebWorkers(const base::Closure& closure) OVERRIDE;
  virtual bool ResolveProxy(const GURL& url, std::string* proxy_list) OVERRIDE;
#if defined(OS_WIN)
  virtual void PreCacheFont(const LOGFONT& log_font) OVERRIDE;
  virtual void ReleaseCachedFonts() OVERRIDE;
#endif

  // ChildThread:
  virtual bool IsWebFrameValid(WebKit::WebFrame* frame) OVERRIDE;

  // GpuChannelHostFactory implementation:
  virtual bool IsMainThread() OVERRIDE;
  virtual base::MessageLoop* GetMainLoop() OVERRIDE;
  virtual scoped_refptr<base::MessageLoopProxy> GetIOLoopProxy() OVERRIDE;
  virtual base::WaitableEvent* GetShutDownEvent() OVERRIDE;
  virtual scoped_ptr<base::SharedMemory> AllocateSharedMemory(
      size_t size) OVERRIDE;
  virtual int32 CreateViewCommandBuffer(
      int32 surface_id,
      const GPUCreateCommandBufferConfig& init_params) OVERRIDE;
  virtual void CreateImage(
      gfx::PluginWindowHandle window,
      int32 image_id,
      const CreateImageCallback& callback) OVERRIDE;
  virtual void DeleteImage(int32 image_id, int32 sync_point) OVERRIDE;

  // Synchronously establish a channel to the GPU plugin if not previously
  // established or if it has been lost (for example if the GPU plugin crashed).
  // If there is a pending asynchronous request, it will be completed by the
  // time this routine returns.
  virtual GpuChannelHost* EstablishGpuChannelSync(CauseForGpuLaunch) OVERRIDE;


  // These methods modify how the next message is sent.  Normally, when sending
  // a synchronous message that runs a nested message loop, we need to suspend
  // callbacks into WebKit.  This involves disabling timers and deferring
  // resource loads.  However, there are exceptions when we need to customize
  // the behavior.
  void DoNotSuspendWebKitSharedTimer();
  void DoNotNotifyWebKitOfModalLoop();

  // True if we are running layout tests. This currently disables forwarding
  // various status messages to the console, skips network error pages, and
  // short circuits size update and focus events.
  bool layout_test_mode() const {
    return layout_test_mode_;
  }
  void set_layout_test_mode(bool layout_test_mode) {
    layout_test_mode_ = layout_test_mode;
  }

  IPC::ForwardingMessageFilter* compositor_filter() const {
    return compositor_filter_.get();
  }

  IPC::ForwardingMessageFilter* compositor_output_surface_filter() const {
    return compositor_output_surface_filter_.get();
  }

  InputHandlerManager* input_handler_manager() const {
    return input_handler_manager_.get();
  }

  // Will be NULL if threaded compositing has not been enabled.
  scoped_refptr<base::MessageLoopProxy> compositor_message_loop_proxy() const {
    return compositor_message_loop_proxy_;
  }

  AppCacheDispatcher* appcache_dispatcher() const {
    return appcache_dispatcher_.get();
  }

  DomStorageDispatcher* dom_storage_dispatcher() const {
    return dom_storage_dispatcher_.get();
  }

  AudioInputMessageFilter* audio_input_message_filter() {
    return audio_input_message_filter_.get();
  }

  AudioMessageFilter* audio_message_filter() {
    return audio_message_filter_.get();
  }

  MIDIMessageFilter* midi_message_filter() {
    return midi_message_filter_.get();
  }

  // Creates the embedder implementation of WebMediaStreamCenter.
  // The resulting object is owned by WebKit and deleted by WebKit at tear-down.
  WebKit::WebMediaStreamCenter* CreateMediaStreamCenter(
      WebKit::WebMediaStreamCenterClient* client);

  // Returns a factory used for creating RTC PeerConnection objects.
  MediaStreamDependencyFactory* GetMediaStreamDependencyFactory();

  PeerConnectionTracker* peer_connection_tracker() {
    return peer_connection_tracker_.get();
  }

  // Current P2PSocketDispatcher. Set to NULL if P2P API is disabled.
  P2PSocketDispatcher* p2p_socket_dispatcher() {
    return p2p_socket_dispatcher_.get();
  }

  VideoCaptureImplManager* video_capture_impl_manager() const {
    return vc_manager_.get();
  }

  // Get the GPU channel. Returns NULL if the channel is not established or
  // has been lost.
  GpuChannelHost* GetGpuChannel();

  // Returns a MessageLoopProxy instance corresponding to the message loop
  // of the thread on which file operations should be run. Must be called
  // on the renderer's main thread.
  scoped_refptr<base::MessageLoopProxy> GetFileThreadMessageLoopProxy();

  // Returns a MessageLoopProxy instance corresponding to the message loop
  // of the thread on which media operations should be run. Must be called
  // on the renderer's main thread.
  scoped_refptr<base::MessageLoopProxy> GetMediaThreadMessageLoopProxy();

  // Causes the idle handler to skip sending idle notifications
  // on the two next scheduled calls, so idle notifications are
  // not sent for at least one notification delay.
  void PostponeIdleNotification();

  // Returns a graphics context shared among all
  // RendererGpuVideoDecoderFactories, or NULL on error.  Context remains owned
  // by this class and must be null-tested before each use to detect context
  // loss.  The returned context is only valid on the compositor thread when
  // threaded compositing is enabled.
  WebGraphicsContext3DCommandBufferImpl* GetGpuVDAContext3D();

  // Handle loss of the shared GpuVDAContext3D context above.
  static void OnGpuVDAContextLoss();

  scoped_refptr<cc::ContextProvider>
      OffscreenContextProviderForMainThread();
  scoped_refptr<cc::ContextProvider>
      OffscreenContextProviderForCompositorThread();

  // AudioRendererMixerManager instance which manages renderer side mixer
  // instances shared based on configured audio parameters.  Lazily created on
  // first call.
  AudioRendererMixerManager* GetAudioRendererMixerManager();

  // AudioHardwareConfig contains audio hardware configuration for
  // renderer side clients.  Creation requires a synchronous IPC call so it is
  // lazily created on the first call.
  media::AudioHardwareConfig* GetAudioHardwareConfig();

#if defined(OS_WIN)
  void PreCacheFontCharacters(const LOGFONT& log_font, const string16& str);
#endif

  // For producing custom V8 histograms. Custom histograms are produced if all
  // RenderViews share the same host, and the host is in the pre-specified set
  // of hosts we want to produce custom diagrams for. The name for a custom
  // diagram is the name of the corresponding generic diagram plus a
  // host-specific suffix.
  class CONTENT_EXPORT HistogramCustomizer {
   public:
    HistogramCustomizer();
    ~HistogramCustomizer();

    // Called when a top frame of a RenderView navigates. This function updates
    // RenderThreadImpl's information about whether all RenderViews are
    // displaying a page from the same host. |host| is the host where a
    // RenderView navigated, and |view_count| is the number of RenderViews in
    // this process.
    void RenderViewNavigatedToHost(const std::string& host, size_t view_count);

    // Used for customizing some histograms if all RenderViews share the same
    // host. Returns the current custom histogram name to use for
    // |histogram_name|, or |histogram_name| if it shouldn't be customized.
    std::string ConvertToCustomHistogramName(const char* histogram_name) const;

   private:
    friend class RenderThreadImplUnittest;

    // Used for updating the information on which is the common host which all
    // RenderView's share (if any). If there is no common host, this function is
    // called with an empty string.
    void SetCommonHost(const std::string& host);

    // The current common host of the RenderViews; empty string if there is no
    // common host.
    std::string common_host_;
    // The corresponding suffix.
    std::string common_host_histogram_suffix_;
    // Set of histograms for which we want to produce a custom histogram if
    // possible.
    std::set<std::string> custom_histograms_;

    DISALLOW_COPY_AND_ASSIGN(HistogramCustomizer);
  };

  HistogramCustomizer* histogram_customizer() {
    return &histogram_customizer_;
  }

  void SetFlingCurveParameters(const std::vector<float>& new_touchpad,
                               const std::vector<float>& new_touchscreen);

 private:
  virtual bool OnControlMessageReceived(const IPC::Message& msg) OVERRIDE;

  void Init();

  void OnSetZoomLevelForCurrentURL(const std::string& scheme,
                                   const std::string& host,
                                   double zoom_level);
  void OnCreateNewView(const ViewMsg_New_Params& params);
  void OnTransferBitmap(const SkBitmap& bitmap, int resource_id);
  void OnPurgePluginListCache(bool reload_pages);
  void OnNetworkStateChanged(bool online);
  void OnGetAccessibilityTree();
  void OnTempCrashWithData(const GURL& data);
  void OnSetWebKitSharedTimersSuspended(bool suspend);

  void IdleHandlerInForegroundTab();

  scoped_ptr<WebGraphicsContext3DCommandBufferImpl> CreateOffscreenContext3d();

  // These objects live solely on the render thread.
  scoped_ptr<AppCacheDispatcher> appcache_dispatcher_;
  scoped_ptr<DomStorageDispatcher> dom_storage_dispatcher_;
  scoped_ptr<IndexedDBDispatcher> main_thread_indexed_db_dispatcher_;
  scoped_ptr<RendererWebKitPlatformSupportImpl> webkit_platform_support_;

  // Used on the render thread and deleted by WebKit at shutdown.
  WebKit::WebMediaStreamCenter* media_stream_center_;

  // Used on the renderer and IPC threads.
  scoped_refptr<DBMessageFilter> db_message_filter_;
  scoped_refptr<AudioInputMessageFilter> audio_input_message_filter_;
  scoped_refptr<AudioMessageFilter> audio_message_filter_;
  scoped_refptr<MIDIMessageFilter> midi_message_filter_;
  scoped_refptr<DevToolsAgentFilter> devtools_agent_message_filter_;

  scoped_ptr<MediaStreamDependencyFactory> media_stream_factory_;

  // This is used to communicate to the browser process the status
  // of all the peer connections created in the renderer.
  scoped_ptr<PeerConnectionTracker> peer_connection_tracker_;

  // Dispatches all P2P sockets.
  scoped_refptr<P2PSocketDispatcher> p2p_socket_dispatcher_;

  // Used on multiple threads.
  scoped_refptr<VideoCaptureImplManager> vc_manager_;

  // Used on multiple script execution context threads.
  scoped_ptr<WebDatabaseObserverImpl> web_database_observer_impl_;

#if defined(OS_WIN)
  // Initialize COM when using plugins outside the sandbox.
  scoped_ptr<base::win::ScopedCOMInitializer> initialize_com_;
#endif

  // The count of RenderWidgets running through this thread.
  int widget_count_;

  // The count of hidden RenderWidgets running through this thread.
  int hidden_widget_count_;

  // The current value of the idle notification timer delay.
  int64 idle_notification_delay_in_ms_;

  // The number of idle handler calls that skip sending idle notifications.
  int idle_notifications_to_skip_;

  bool suspend_webkit_shared_timer_;
  bool notify_webkit_of_modal_loop_;

  // The following flag is used to control layout test specific behavior.
  bool layout_test_mode_;

  // Timer that periodically calls IdleHandler.
  base::RepeatingTimer<RenderThreadImpl> idle_timer_;

  // The channel from the renderer process to the GPU process.
  scoped_refptr<GpuChannelHost> gpu_channel_;

  // A lazily initiated thread on which file operations are run.
  scoped_ptr<base::Thread> file_thread_;

  // May be null if overridden by ContentRendererClient.
  scoped_ptr<base::Thread> compositor_thread_;

  // Thread for running multimedia operations (e.g., video decoding).
  scoped_ptr<base::Thread> media_thread_;

  // Will point to appropriate MessageLoopProxy after initialization,
  // regardless of whether |compositor_thread_| is overriden.
  scoped_refptr<base::MessageLoopProxy> compositor_message_loop_proxy_;

  // May be null if unused by the |input_handler_manager_|.
  scoped_refptr<InputEventFilter> input_event_filter_;
  scoped_ptr<InputHandlerManager> input_handler_manager_;
  scoped_refptr<IPC::ForwardingMessageFilter> compositor_filter_;
  scoped_refptr<IPC::ForwardingMessageFilter> compositor_output_surface_filter_;

  class RendererContextProviderCommandBuffer;
  scoped_refptr<RendererContextProviderCommandBuffer>
      shared_contexts_main_thread_;
  scoped_refptr<RendererContextProviderCommandBuffer>
      shared_contexts_compositor_thread_;

  ObserverList<RenderProcessObserver> observers_;

  class GpuVDAContextLostCallback;
  scoped_ptr<GpuVDAContextLostCallback> context_lost_cb_;
  scoped_ptr<WebGraphicsContext3DCommandBufferImpl> gpu_vda_context3d_;

  scoped_ptr<AudioRendererMixerManager> audio_renderer_mixer_manager_;
  scoped_ptr<media::AudioHardwareConfig> audio_hardware_config_;

  HistogramCustomizer histogram_customizer_;

  DISALLOW_COPY_AND_ASSIGN(RenderThreadImpl);
};

}  // namespace content

#endif  // CONTENT_RENDERER_RENDER_THREAD_IMPL_H_
