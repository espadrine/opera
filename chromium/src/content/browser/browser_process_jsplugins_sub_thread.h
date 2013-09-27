// Copyright (c) 2013 Opera Software AS. All rights reserved.

#ifndef CONTENT_BROWSER_BROWSER_PROCESS_JSPLUGINS_SUB_THREAD_H_
#define CONTENT_BROWSER_BROWSER_PROCESS_JSPLUGINS_SUB_THREAD_H_

#include "base/memory/scoped_ptr.h"
#include "base/synchronization/waitable_event.h"
#include "content/browser/browser_process_sub_thread.h"

namespace opera {
namespace jsapi {

class BrowserProcessJSPluginsSubThread
    : public content::BrowserProcessSubThread {
 public:
  BrowserProcessJSPluginsSubThread();

  base::WaitableEvent* GetShutDownEvent();

 protected:
  virtual void Init() OVERRIDE;
  virtual void CleanUp() OVERRIDE;

  scoped_ptr<base::WaitableEvent> shutdown_event_;
};

}  // namespace jsapi
}  // namespace opera

#endif  // CONTENT_BROWSER_BROWSER_PROCESS_JSPLUGINS_SUB_THREAD_H_
