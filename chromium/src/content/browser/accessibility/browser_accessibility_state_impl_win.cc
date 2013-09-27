// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/accessibility/browser_accessibility_state_impl.h"

#include <windows.h>
#include <psapi.h>

#include "base/files/file_path.h"
#include "base/metrics/histogram.h"
#include "base/strings/string_util.h"

namespace {
// Name of the mutex that is created by "Microsoft Narrator" while it is
// running. We use this to detect if Narrator is running or not in response
// to a WM_GETOBJECT message. Narrator doesn't respond to
// NotifyWinEvent(EVENT_SYSTEM_ALERT, ...) calls, it also doesn't register
// itself as a reader so that we can query SPI_GETSCREENREADER and check if it
// is running or not. Therefore this seems to be the only way of detecting it.
const WCHAR* kMicrosoftNarratorMutexName = L"NarratorRunning";
}  // namespace

namespace content {

void BrowserAccessibilityStateImpl::UpdatePlatformSpecificHistograms() {
  // NOTE: this method is run from the file thread to reduce jank, since
  // there's no guarantee these system calls will return quickly. Be careful
  // not to add any code that isn't safe to run from a non-main thread!

  // Check if "Microsoft Narrator" is running by checking if
  // the mutex named "NarratorRunning" is created or not.
  bool narrator_running = false;
  HANDLE narrator_mutex =
      OpenMutexW(MUTEX_ALL_ACCESS, FALSE, kMicrosoftNarratorMutexName);
  if (narrator_mutex) {
    narrator_running = true;
    CloseHandle(narrator_mutex);
  }

  AUDIODESCRIPTION audio_description = {0};
  audio_description.cbSize = sizeof(AUDIODESCRIPTION);
  SystemParametersInfo(SPI_GETAUDIODESCRIPTION, 0, &audio_description, 0);
  UMA_HISTOGRAM_BOOLEAN("Accessibility.WinAudioDescription",
                        !!audio_description.Enabled);

  BOOL win_screen_reader = FALSE;
  SystemParametersInfo(SPI_GETSCREENREADER, 0, &win_screen_reader, 0);
  UMA_HISTOGRAM_BOOLEAN("Accessibility.WinScreenReader",
                        !!win_screen_reader);

  STICKYKEYS sticky_keys = {0};
  sticky_keys.cbSize = sizeof(STICKYKEYS);
  SystemParametersInfo(SPI_GETSTICKYKEYS, 0, &sticky_keys, 0);
  UMA_HISTOGRAM_BOOLEAN("Accessibility.WinStickyKeys",
                        0 != (sticky_keys.dwFlags & SKF_STICKYKEYSON));

  // Get the file paths of all DLLs loaded.
  HANDLE process = GetCurrentProcess();
  HMODULE* modules = NULL;
  DWORD bytes_required;
  if (!EnumProcessModules(process, modules, 0, &bytes_required))
    return;

  scoped_ptr<char[]> buffer(new char[bytes_required]);
  modules = reinterpret_cast<HMODULE*>(buffer.get());
  DWORD ignore;
  if (!EnumProcessModules(process, modules, bytes_required, &ignore))
    return;

  // Look for DLLs of assistive technology known to work with Chrome.
  bool jaws = false;
  bool nvda = false;
  bool satogo = false;
  bool zoomtext = false;
  size_t module_count = bytes_required / sizeof(HMODULE);
  for (size_t i = 0; i < module_count; i++) {
    TCHAR filename[MAX_PATH];
    GetModuleFileName(modules[i], filename, sizeof(filename));
    string16 module_name(base::FilePath(filename).BaseName().value());
    if (LowerCaseEqualsASCII(module_name, "fsdomsrv.dll"))
      jaws = true;
    if (LowerCaseEqualsASCII(module_name, "vbufbackend_gecko_ia2.dll"))
      nvda = true;
    if (LowerCaseEqualsASCII(module_name, "stsaw32.dll"))
      satogo = true;
    if (LowerCaseEqualsASCII(module_name, "zslhook.dll"))
      zoomtext = true;
  }

  UMA_HISTOGRAM_BOOLEAN("Accessibility.WinJAWS", jaws);
  UMA_HISTOGRAM_BOOLEAN("Accessibility.WinNVDA", nvda);
  UMA_HISTOGRAM_BOOLEAN("Accessibility.WinSAToGo", satogo);
  UMA_HISTOGRAM_BOOLEAN("Accessibility.WinZoomText", zoomtext);
  UMA_HISTOGRAM_BOOLEAN("Accessibility.WinNarrator", narrator_running);
}

}  // namespace content
