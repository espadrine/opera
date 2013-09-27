// -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
//
// Copyright (C) 2013 Opera Software ASA.  All rights reserved.
//
// This file is an original work developed by Opera Software ASA

#ifndef CONTENT_BROWSER_MEMORY_ALLOWANCE_CONTROLLER_IMPL_H_
#define CONTENT_BROWSER_MEMORY_ALLOWANCE_CONTROLLER_IMPL_H_

#include "content/public/browser/memory_allowance_controller.h"

namespace content {

// Default MemoryAllowanceController implementation.
// The default implementation is empty. It is ment to be overridden by
// the embedder (See MemoryAllowanceController::SetInstance()).
class MemoryAllowanceControllerImpl : public MemoryAllowanceController {
 public:
  MemoryAllowanceControllerImpl();

  //
  // MemoryAllowanceController functions.
  //
  virtual bool Enabled() OVERRIDE;
  virtual BrowserMessageFilter* CreateMessageFilter(int id) OVERRIDE;
  virtual int64 InitializeProcessAllowance(int id) OVERRIDE;
  virtual void OnIOThreadUp(const scoped_refptr<base::MessageLoopProxy>&
                              message_loop_proxy) OVERRIDE;
  virtual void OnIOThreadDown() OVERRIDE;

 protected:
  virtual ~MemoryAllowanceControllerImpl();

 private:
  DISALLOW_COPY_AND_ASSIGN(MemoryAllowanceControllerImpl);
};


}  // namespace content

#endif  // CONTENT_BROWSER_MEMORY_ALLOWANCE_CONTROLLER_IMPL_H_
