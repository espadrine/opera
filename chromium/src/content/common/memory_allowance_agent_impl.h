// -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
//
// Copyright (C) 2013 Opera Software ASA.  All rights reserved.
//
// This file is an original work developed by Opera Software ASA

#ifndef CONTENT_COMMON_MEMORY_ALLOWANCE_AGENT_IMPL_H_
#define CONTENT_COMMON_MEMORY_ALLOWANCE_AGENT_IMPL_H_

#include "content/public/common/memory_allowance_agent.h"

namespace content {

// Default MemoryAllowanceAgent implementation.
// The default implementation is empty. It is ment to be overridden by
// the embedder (See MemoryAllowanceAgent::SetInstance()).
class MemoryAllowanceAgentImpl : public MemoryAllowanceAgent {
 public:
  MemoryAllowanceAgentImpl();

  //
  // MemoryAllowanceAgent functions.
  //
  virtual bool Enabled() OVERRIDE;
  virtual void SpecializeForProcess() OVERRIDE;
  virtual IPC::ChannelProxy::MessageFilter* message_filter() OVERRIDE;
  virtual bool ValidateMemoryChange(int64 change, bool test) OVERRIDE;
  virtual int64 GetAllocated() OVERRIDE;

 protected:
  virtual ~MemoryAllowanceAgentImpl();

 private:
  DISALLOW_COPY_AND_ASSIGN(MemoryAllowanceAgentImpl);
};

}  // namespace content

#endif  // CONTENT_COMMON_MEMORY_ALLOWANCE_AGENT_IMPL_H_
