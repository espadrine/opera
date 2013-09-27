// Copyright (c) 2013 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/gpu/accelerated_widget_manager.h"

#include "content/public/browser/browser_thread.h"

namespace content {

AcceleratedWidgetManager::AcceleratedWidgetManager()
    : surface_id_(0) {
}

AcceleratedWidgetManager::~AcceleratedWidgetManager() {
}

AcceleratedWidgetManager* AcceleratedWidgetManager::GetInstance() {
  return Singleton<AcceleratedWidgetManager>::get();
}

void AcceleratedWidgetManager::SetWidget(int32 surface_id) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(!surface_id_);
  surface_id_ = surface_id;

  if (!result_callback_.is_null()) {
    result_callback_.Run(surface_id_);
    result_callback_.Reset();
    release_callback_ = pending_release_callback_;
  }
}

void AcceleratedWidgetManager::UnsetWidget(ReleasedCallback released_callback) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  if (release_callback_.is_null()) {
    released_callback.Run();
  } else {
    released_callbacks_[surface_id_] = released_callback;
    release_callback_.Run();
    release_callback_.Reset();
  }

  surface_id_ = 0;
}

void AcceleratedWidgetManager::AcquireWidget(ResultCallback result_callback,
                                             ReleaseCallback release_callback) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  // Cancel any pending request.
  if (!result_callback_.is_null()) {
    result_callback_.Run(0);
    result_callback_.Reset();
    pending_release_callback_.Reset();
  }

  // Three scenarios needs to be dealt with.
  // 1. There is a holder: ask it to release and wait for release message.
  // 2. There is no holder and a surface is available: reply immediately.
  // 3. There is no holder and no surface available: wait for a surface.
  if (!release_callback_.is_null()) {
    release_callback_.Run();
    release_callback_.Reset();
    result_callback_ = result_callback;
    pending_release_callback_ = release_callback;
  } else if (surface_id_) {
    result_callback.Run(surface_id_);
    release_callback_ = release_callback;
  } else {
    result_callback_ = result_callback;
    pending_release_callback_ = release_callback;
  }
}

void AcceleratedWidgetManager::ReleaseWidget(int32 surface_id) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  // Notify the user of this manager that a widget has been released.
  if (!released_callbacks_[surface_id].is_null()) {
    released_callbacks_[surface_id].Run();
    released_callbacks_.erase(surface_id);
  }

  if (surface_id_) {
    if (!result_callback_.is_null()) {
      result_callback_.Run(surface_id_);
      result_callback_.Reset();
      release_callback_ = pending_release_callback_;
    } else {
      release_callback_.Reset();
    }
  }
}

}  // namespace content
