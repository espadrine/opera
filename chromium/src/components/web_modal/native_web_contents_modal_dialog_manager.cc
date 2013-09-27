// -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
//
// Copyright (C) 2013 Opera Software ASA.  All rights reserved.
//
// This file is an original work developed by Opera Software ASA

#include "components/web_modal/native_web_contents_modal_dialog_manager.h"

bool web_modal::NativeWebContentsModalDialogManager::ShouldExpire(
    web_modal::NativeWebContentsModalDialog dialog) {
  return true;
}
