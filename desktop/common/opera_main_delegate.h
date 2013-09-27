// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Modified by Opera Software ASA
// @copied-from chromium/src/content/shell/shell_main_delegate.h
// @final-synchronized af00ceb9b66545cb85a4168dc72953d7caa97656

#ifndef DESKTOP_COMMON_OPERA_MAIN_DELEGATE_H_
#define DESKTOP_COMMON_OPERA_MAIN_DELEGATE_H_

#include <string>

#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/app/content_main_delegate.h"

#include "desktop/common/opera_content_client.h"

namespace opera {
class OperaContentBrowserClient;
class OperaContentRendererClient;
}

class OperaMainDelegate : public content::ContentMainDelegate {
 public:
  /**
   * @param local The locale that should be loaded.
   *
   * @return The locale that was loaded or empty string.
   */
  static const std::string InitializeResourceBundle(const std::string& locale);

  OperaMainDelegate();
  virtual ~OperaMainDelegate();

  // ContentMainDelegate implementation:
  virtual bool BasicStartupComplete(int* exit_code) OVERRIDE;
  virtual void PreSandboxStartup() OVERRIDE;
  virtual int RunProcess(
      const std::string& process_type,
      const content::MainFunctionParams& main_function_params) OVERRIDE;
  virtual content::ContentBrowserClient* CreateContentBrowserClient() OVERRIDE;
  virtual content::ContentPluginClient* CreateContentPluginClient() OVERRIDE;
  virtual content::ContentRendererClient*
      CreateContentRendererClient() OVERRIDE;
  virtual content::ContentUtilityClient*
      CreateContentUtilityClient() OVERRIDE;

 private:
  scoped_ptr<content::ContentBrowserClient> browser_client_;
  scoped_ptr<opera::OperaContentRendererClient> renderer_client_;
  opera::OperaContentClient content_client_;

  DISALLOW_COPY_AND_ASSIGN(OperaMainDelegate);
};

#endif  // DESKTOP_COMMON_OPERA_MAIN_DELEGATE_H_
