// -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
//
// Copyright (C) 2013 Opera Software ASA.  All rights reserved.
//
// This file is an original work developed by Opera Software ASA

#ifndef CONTENT_PUBLIC_COMMON_MEMORY_ALLOWANCE_AGENT_H_
#define CONTENT_PUBLIC_COMMON_MEMORY_ALLOWANCE_AGENT_H_

#include "base/basictypes.h"
#include "base/message_loop/message_loop_proxy.h"
#include "content/common/content_export.h"
#include "ipc/ipc_channel_proxy.h"

// Each process has a MemoryAllowanceAgent that works in concert with
// the MemoryAllowanceController to limit the amount of system memory
// used by the browser

// A MemoryAllowanceAgent provides a way to monitor system memory
// usage of a process and implement a memory limitation strategy.  The
// main allocators of system memory are hooked up to check with the
// MemoryAllowanceAgent prior to allocating or deallocating system
// memory. It gives the MemoryAllowanceAgent an opportunity to count
// the memory usage, but also to deny allocations that would otherwise
// cause the process to use to much memory. Normally, a
// MemoryAllowanceAgent would also communicate with the
// MemoryAllowanceController to get permission to use more memory, or
// inversely, to lower the memory usage of the process in case the
// browser is running low.

namespace content {

// Interface that the embedder implements to override the default
// empty implementation.
class CONTENT_EXPORT MemoryAllowanceAgent {
 public:
  // Get a handle to the memory allowance agent for this process.
  static MemoryAllowanceAgent* GetInstance();

  // Overrides the default instance. Should be called only once, before
  // GetInstance() is called for the first time.
  // NOTE: Calling SetInstance() transfers ownership of the
  // instance. The caller must not delete the instance.
  static void SetInstance(MemoryAllowanceAgent* instance);

  MemoryAllowanceAgent();

  // A memory limit is active.
  virtual bool Enabled() = 0;

  // Run before the process main function to allow for early
  // process specific setup.
  virtual void SpecializeForProcess() = 0;

  // Returns a message filter to be installed in the current process.
  // May return NULL if no message filter is needed.
  virtual IPC::ChannelProxy::MessageFilter* message_filter() = 0;

  // Called by allocators of system memory to report allocations and
  // deallocations to the agent.  The change should be validated against
  // the limit and either be accepted and counted, or denied and
  // discarded. The return value reflects the result of the
  // validation. If the change is accepted the allocator can go
  // through and allocate the specified amount, if not, the allocation
  // should not be carried through and the allocator may abort the
  // process if no other option is available. Deallocations (negative
  // changes) are trivially accepted.
  // This function can also be used to test if an allocation is likely
  // to go through.  When test is true, the change will be validated,
  // but not counted. A test is never a go ahead for an allocation. If
  // an allocation is to be made it must still be validated with test
  // set to false. Note that due to race conditions between threads,
  // the later validation might not go through even if the initial
  // test succeeded.
  virtual bool ValidateMemoryChange(int64 change, bool test) = 0;

  // Returns the amount of memory currently allocated by the process.
  virtual int64 GetAllocated() = 0;

 protected:
  // NOTE: Once created, the memory allowance agent is never deleted.
  // So, don't write code that depends on the destructor getting called.
  // Consider running cleanup code when the message filter goes down instead.
  virtual ~MemoryAllowanceAgent();

 private:
  static MemoryAllowanceAgent* instance_;
  DISALLOW_COPY_AND_ASSIGN(MemoryAllowanceAgent);
};

}  // namespace content

#endif  // CONTENT_PUBLIC_COMMON_MEMORY_ALLOWANCE_AGENT_H_
