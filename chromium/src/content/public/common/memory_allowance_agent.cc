// -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
//
// Copyright (C) 2013 Opera Software ASA.  All rights reserved.
//
// This file is an original work developed by Opera Software ASA

#include "content/public/common/memory_allowance_agent.h"
#include "content/common/memory_allowance_agent_impl.h"

namespace content {

MemoryAllowanceAgent* MemoryAllowanceAgent::instance_ = NULL;

// Returns the instance.
MemoryAllowanceAgent* MemoryAllowanceAgent::GetInstance() {
  if (!instance_)
    instance_ = new MemoryAllowanceAgentImpl();
  return instance_;
}

void MemoryAllowanceAgent::SetInstance(MemoryAllowanceAgent* instance) {
  CHECK(!instance_);
  instance_ = instance;
}

MemoryAllowanceAgent::MemoryAllowanceAgent() {
}

MemoryAllowanceAgent::~MemoryAllowanceAgent() {
}

}  // namespace content
