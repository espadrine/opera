// Copyright (c) 2013 Opera Software AS. All rights reserved.

#include "content/browser/browser_process_jsplugins_sub_thread.h"

#include "content/public/browser/browser_thread.h"

using content::BrowserProcessSubThread;

namespace opera {
namespace jsapi {

BrowserProcessJSPluginsSubThread::BrowserProcessJSPluginsSubThread()
  : BrowserProcessSubThread(content::BrowserThread::JSPLUGINS),
    shutdown_event_(new base::WaitableEvent(true, false)) {
}

void BrowserProcessJSPluginsSubThread::Init() {
  BrowserProcessSubThread::Init();
}

void BrowserProcessJSPluginsSubThread::CleanUp() {
  shutdown_event_->Signal();
  BrowserProcessSubThread::CleanUp();
}

base::WaitableEvent* BrowserProcessJSPluginsSubThread::GetShutDownEvent() {
  return shutdown_event_.get();
}

}  // namespace jsapi
}  // namespace opera
