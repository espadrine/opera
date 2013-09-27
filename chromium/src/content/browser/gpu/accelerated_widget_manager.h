// Copyright (c) 2013 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>

#include "base/basictypes.h"
#include "base/cancelable_callback.h"
#include "base/memory/singleton.h"
#include "ui/gfx/native_widget_types.h"

#ifndef CONTENT_BROWSER_GPU_ACCELERATED_WIDGET_MANAGER_H_
#define CONTENT_BROWSER_GPU_ACCELERATED_WIDGET_MANAGER_H_

template <typename T> struct DefaultSingletonTraits;

namespace content {

// The accelerated widget manager singleton manages access to a widget that can
// be used for rendering using the GPU. Only one user can exist at a given time,
// and only one widget can exist at a given time.
//
// It is required that this singleton is created and accessed only on the UI
// thread.
class AcceleratedWidgetManager {
 public:
  typedef base::Callback<void(int32 surface_id)> ResultCallback;
  typedef base::Callback<void()> ReleaseCallback;
  typedef base::Callback<void()> ReleasedCallback;

  static AcceleratedWidgetManager* GetInstance();

  // Notify the widget manager of a surface id associated with an accelerated
  // widget. It is expected that only one such surface id will be available at
  // a time.
  void SetWidget(int32 surface_id);

  // Notify that a widget has been made unavailable and that the resource holder
  // must release it.
  void UnsetWidget(ReleasedCallback released_callback);

  // Asynchronously acquire the shared accelerated widget resource. Upon
  // success the `result_callback' will be invoked with a surface id associated
  // with an accelerated widget handle. If the resource for some reason failed
  // to be acquired, the result callback will be invoked with a `surface_id' 0.
  //
  // If it was not possible to invoke the result callback immediately, it will
  // be invoked whenever a widget is made available. If another call to this
  // function is made it will cancel any existing request.
  void AcquireWidget(ResultCallback result_callback,
                     ReleaseCallback release_callback);

  // Notify the widget manager that a widget resource was released and can be
  // used by some other surface.
  void ReleaseWidget(int32 surface_id);

 private:
  AcceleratedWidgetManager();
  ~AcceleratedWidgetManager();

  int32 surface_id_;

  ResultCallback result_callback_;
  ReleaseCallback release_callback_;
  ReleaseCallback pending_release_callback_;
  std::map<int32, ReleasedCallback> released_callbacks_;

  friend struct DefaultSingletonTraits<AcceleratedWidgetManager>;

  DISALLOW_COPY_AND_ASSIGN(AcceleratedWidgetManager);
};

}  // namespace content

#endif  // CONTENT_BROWSER_GPU_ACCELERATED_WIDGET_MANAGER_H_
