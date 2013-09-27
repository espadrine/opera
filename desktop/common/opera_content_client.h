// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Modified by Opera Software ASA
// @copied-from chromium/src/chrome/common/chrome_content_client.h
// @final-synchronized d50b2c0904d92325d06055eacd9eba55a7b8c04f

#ifndef DESKTOP_COMMON_OPERA_CONTENT_CLIENT_H_
#define DESKTOP_COMMON_OPERA_CONTENT_CLIENT_H_

#include <string>
#include <vector>

#include "base/compiler_specific.h"
#include "content/public/common/content_client.h"

namespace opera {

class OperaContentClient : public content::ContentClient {
 public:
  OperaContentClient() {}

  static std::string GetDefaultUserAgent();
  static std::string GetDefaultUserAgent(const std::string& partner_postfix);

  virtual void SetActiveURL(const GURL& url) OVERRIDE;
  virtual void SetGpuInfo(const gpu::GPUInfo& gpu_info) OVERRIDE;
  virtual void AddPepperPlugins(
      std::vector<content::PepperPluginInfo>* plugins) OVERRIDE;
  virtual void AddNPAPIPlugins(
      webkit::npapi::PluginList* plugin_list) OVERRIDE;
  virtual void AddAdditionalSchemes(
      std::vector<std::string>* standard_schemes,
      std::vector<std::string>* saveable_shemes) OVERRIDE;
  virtual bool CanSendWhileSwappedOut(const IPC::Message* message) OVERRIDE;
  virtual std::string GetProduct() const OVERRIDE;
  virtual std::string GetUserAgent() const OVERRIDE;
  virtual string16 GetLocalizedString(int message_id) const OVERRIDE;
  virtual base::StringPiece GetDataResource(
      int resource_id,
      ui::ScaleFactor scale_factor) const OVERRIDE;
  virtual base::RefCountedStaticMemory* GetDataResourceBytes(
      int resource_id) const OVERRIDE;
  virtual gfx::Image& GetNativeImageNamed(int resource_id) const OVERRIDE;

#if defined(OS_MACOSX) && !defined(OS_IOS)
  virtual bool GetSandboxProfileForSandboxType(
      int sandbox_type,
      int* sandbox_profile_resource_id) const OVERRIDE;
  virtual std::string GetCarbonInterposePath() const OVERRIDE;
#endif
};

}  // namespace opera

#endif  // DESKTOP_COMMON_OPERA_CONTENT_CLIENT_H_
