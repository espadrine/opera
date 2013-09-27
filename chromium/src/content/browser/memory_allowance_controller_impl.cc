// -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
//
// Copyright (C) 2013 Opera Software ASA.  All rights reserved.
//
// This file is an original work developed by Opera Software ASA

#include "content/browser/memory_allowance_controller_impl.h"

namespace content {

bool MemoryAllowanceControllerImpl::Enabled() {
  return false;
}

BrowserMessageFilter* MemoryAllowanceControllerImpl::CreateMessageFilter(
    int id) {
  return NULL;
}

int64 MemoryAllowanceControllerImpl::InitializeProcessAllowance(int id) {
  return 0;
}

void MemoryAllowanceControllerImpl::OnIOThreadUp(
    const scoped_refptr<base::MessageLoopProxy>& message_loop_proxy) {
}

void MemoryAllowanceControllerImpl::OnIOThreadDown() {
}

MemoryAllowanceControllerImpl::MemoryAllowanceControllerImpl() {
}

MemoryAllowanceControllerImpl::~MemoryAllowanceControllerImpl() {
}

}  // namespace content
