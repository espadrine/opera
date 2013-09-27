// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/android/content_view_render_view.h"

#include "content/browser/gpu/accelerated_widget_manager.h"
#include "content/common/gpu/gpu_surface_tracker.h"

#include "base/android/jni_android.h"
#include "base/android/jni_string.h"
#include "base/android/scoped_java_ref.h"
#include "base/bind.h"
#include "base/lazy_instance.h"
#include "base/memory/scoped_ptr.h"
#include "base/message_loop.h"
#include "cc/layers/layer.h"
#include "content/browser/android/content_view_core_impl.h"
#include "content/browser/renderer_host/render_view_host_impl.h"
#include "content/browser/renderer_host/render_widget_host_impl.h"
#include "content/public/browser/android/compositor.h"
#include "content/public/browser/android/content_view_layer_renderer.h"
#include "content/public/browser/compositor_util.h"
#include "jni/ContentViewRenderView_jni.h"
#include "ui/gfx/size.h"

#include <android/native_window_jni.h>

using base::android::ScopedJavaLocalRef;

namespace content {

namespace {

const void* kContentViewRenderViewUserDataKey =
    &kContentViewRenderViewUserDataKey;
}

class ContentViewRenderView::ContentViewRenderViewUserData
    : public base::SupportsUserData::Data {
 public:
  explicit ContentViewRenderViewUserData(ContentViewRenderView* view)
      : content_view_render_view_(view) {
  }

  virtual ~ContentViewRenderViewUserData() {
    content_view_render_view_->current_web_contents_ = NULL;
  }

 private:
  ContentViewRenderView* content_view_render_view_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(ContentViewRenderViewUserData);
};

// static
bool ContentViewRenderView::RegisterContentViewRenderView(JNIEnv* env) {
  return RegisterNativesImpl(env);
}

ContentViewRenderView::ContentViewRenderView()
    : scheduled_composite_(false),
      surface_id_(0),
      current_web_contents_(NULL),
      weak_factory_(this) {
}

ContentViewRenderView::~ContentViewRenderView() {
  if (current_web_contents_)
    current_web_contents_->RemoveUserData(kContentViewRenderViewUserDataKey);
}

// static
jint Init(JNIEnv* env, jclass clazz) {
  ContentViewRenderView* content_view_render_view =
      new ContentViewRenderView();
  return reinterpret_cast<jint>(content_view_render_view);
}

void ContentViewRenderView::Destroy(JNIEnv* env, jobject obj) {
  delete this;
}

void ContentViewRenderView::SetCurrentContentView(
    JNIEnv* env, jobject obj, int native_content_view) {
  InitCompositor();
  ContentViewCoreImpl* content_view =
      reinterpret_cast<ContentViewCoreImpl*>(native_content_view);
  if (content_view && compositor_)
    compositor_->SetRootLayer(content_view->GetLayer());

  if (current_web_contents_)
    current_web_contents_->RemoveUserData(kContentViewRenderViewUserDataKey);
  current_web_contents_ = content_view->GetWebContents();
  current_web_contents_->SetUserData(kContentViewRenderViewUserDataKey,
                                     new ContentViewRenderViewUserData(this));
}

void ContentViewRenderView::SurfaceCreated(
    JNIEnv* env, jobject obj, jobject jsurface) {
  InitCompositor();

  if (UsesCompositor()) {
    compositor_->SetSurface(jsurface);
  } else {
    ANativeWindow* native_window = ANativeWindow_fromSurface(env, jsurface);
    if (!native_window)
      return;
    DCHECK(!surface_id_);
    GpuSurfaceTracker* tracker = GpuSurfaceTracker::Get();
    surface_id_ = tracker->AddSurfaceForNativeWidget(native_window);
    AcceleratedWidgetManager::GetInstance()->SetWidget(surface_id_);

    if (current_web_contents_) {
      RenderWidgetHostView* rwhv =
          current_web_contents_->GetRenderWidgetHostView();
      if (rwhv)
        RenderWidgetHostImpl::From(rwhv->GetRenderWidgetHost())->
            ActivateOutputSurface();
    }
  }
}

void ContentViewRenderView::AcceleratedWidgetReleasedCallback(int surface_id) {
    GpuSurfaceTracker* tracker = GpuSurfaceTracker::Get();
    ANativeWindow* native_window = tracker->GetNativeWidget(surface_id);
    tracker->RemoveSurface(surface_id);
    ANativeWindow_release(native_window);
}

void ContentViewRenderView::SurfaceDestroyed(JNIEnv* env, jobject obj) {
  if (UsesCompositor()) {
    compositor_->SetSurface(NULL);
  } else {
    DCHECK(surface_id_);
    AcceleratedWidgetManager::GetInstance()->UnsetWidget(
        base::Bind(&ContentViewRenderView::AcceleratedWidgetReleasedCallback,
                   base::Unretained(this),
                   surface_id_));
    surface_id_ = 0;
  }
}

void ContentViewRenderView::SurfaceSetSize(
    JNIEnv* env, jobject obj, jint width, jint height) {
  if (UsesCompositor()) {
    compositor_->SetWindowBounds(gfx::Size(width, height));
  } else {
    if (current_web_contents_) {
      RenderWidgetHostViewPort* rwhvp =
          RenderWidgetHostViewPort::FromRWHV(
              current_web_contents_->GetRenderWidgetHostView());
      if (rwhvp) {
        RenderWidgetHostImpl* rhwi = RenderWidgetHostImpl::From(
            rwhvp->GetRenderWidgetHost());
        current_web_contents_->GetRenderViewHost()->NotifyMoveOrResizeStarted();

        // Only set physical backing size directly if the view is not active or
        // if it is empty.
        // If the view is active, i.e. it has been shown and drawn its first
        // frame, the compositor output surface will detect a buffer size
        // change and properly update the physical backing size.
        RenderWidgetHostViewAndroid* rwhva =
            reinterpret_cast<RenderWidgetHostViewAndroid*>(rwhvp);
        if (rwhva->GetPhysicalBackingSize().IsEmpty() || !rwhva->IsActive()) {
          rwhva->SetPhysicalBackingSize(gfx::Size(width, height));
        } else {
          rwhva->ResizeStarted(gfx::Size(width, height));
          rhwi->Frame(true);
        }
      }
    }
  }
}

void ContentViewRenderView::ScheduleComposite() {
  if (scheduled_composite_)
    return;

  scheduled_composite_ = true;
  base::MessageLoop::current()->PostTask(
      FROM_HERE,
      base::Bind(&ContentViewRenderView::Composite,
                 weak_factory_.GetWeakPtr()));
}

void ContentViewRenderView::OnSwapBuffersCompleted() {
  if (current_web_contents_) {
    RenderWidgetHostViewPort* rwhvp =
        RenderWidgetHostViewPort::FromRWHV(
            current_web_contents_->GetRenderWidgetHostView());
    if (rwhvp)
      rwhvp->CompositorBuffersPresented();
  }
}

bool ContentViewRenderView::UsesCompositor() {
  return IsBrowserSideCompositorEnabled();
}

void ContentViewRenderView::InitCompositor() {
  // Only create a browser side compositor if we use image transport surfaces.
  // If we don't use image transport surfaces, i.e. render directly to Android
  // native window's we have no use for a browser side compositor because
  // SurfaceFlinger will composit the webview output for us.
  if (UsesCompositor() && !compositor_)
    compositor_.reset(Compositor::Create(this));
}

void ContentViewRenderView::Composite() {
  if (!compositor_)
    return;

  scheduled_composite_ = false;
  compositor_->Composite();
}

}  // namespace content
