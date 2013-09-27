// -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
//
// Copyright (C) 2013 Opera Software ASA.  All rights reserved.
//
// This file is an original work developed by Opera Software ASA

#include "content/common/memory_allowance_agent_impl.h"

namespace content {

bool MemoryAllowanceAgentImpl::Enabled() {
  return false;
}

void MemoryAllowanceAgentImpl::SpecializeForProcess() {
}

IPC::ChannelProxy::MessageFilter* MemoryAllowanceAgentImpl::message_filter() {
  return NULL;
}

bool MemoryAllowanceAgentImpl::ValidateMemoryChange(int64 change, bool test) {
  return true;
}

int64 MemoryAllowanceAgentImpl::GetAllocated() {
  return 0;
}

MemoryAllowanceAgentImpl::MemoryAllowanceAgentImpl() {
}

MemoryAllowanceAgentImpl::~MemoryAllowanceAgentImpl() {
}

}  // namespace content
