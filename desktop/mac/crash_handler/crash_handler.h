// -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
//
// Copyright (C) 2013 Opera Software ASA.  All rights reserved.
//
// This file is an original work developed by Opera Software ASA

#ifndef DESKTOP_MAC_CRASH_HANDLER_CRASH_HANDLER_H_
#define DESKTOP_MAC_CRASH_HANDLER_CRASH_HANDLER_H_

namespace opera {

/**
 * Initializes crash handler.
 */
void InitCrashHandler(int argc, const char** argv);

/**
 * Collects additional information about the process that
 * will be used in case process crashed. Should be called
 * after initialization of command line.
 */
void InitCrashInfo();

}  // namespace opera

#endif  // DESKTOP_MAC_CRASH_HANDLER_CRASH_HANDLER_H_
