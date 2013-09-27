// -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
//
// Copyright (C) 2013 Opera Software ASA.  All rights reserved.
//
// This file is an original work developed by Opera Software ASA

#include "content/public/browser/memory_allowance_controller.h"
#include "content/browser/memory_allowance_controller_impl.h"

namespace switches {
const char kMemoryAllowance[] = "memory-allowance";
}

namespace content {

MemoryAllowanceController* MemoryAllowanceController::instance_ = NULL;

MemoryAllowanceController* MemoryAllowanceController::GetInstance() {
  if (!instance_)
    instance_ = new MemoryAllowanceControllerImpl();
  return instance_;
}

void MemoryAllowanceController::SetInstance(
    MemoryAllowanceController* instance) {
  CHECK(!instance_);
  instance_ = instance;
}

MemoryAllowanceController::MemoryAllowanceController() {
}

MemoryAllowanceController::~MemoryAllowanceController() {
}

}  // namespace content
