// -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
//
// Copyright (C) 2013 Opera Software ASA.  All rights reserved.
//
// This file is an original work developed by Opera Software ASA

#include "chrome/browser/download/download_enhance_manager.h"

#include "base/logging.h"
#include "base/memory/singleton.h"
#include "base/stl_util.h"
#include "content/public/browser/browser_thread.h"
#include "net/url_request/url_request_job_manager.h"

DownloadEnhanceManager::DownloadEnhanceManager() {
}

DownloadEnhanceManager::~DownloadEnhanceManager() {
  enhancers_.clear();
}

DownloadEnhanceManager* DownloadEnhanceManager::GetInstance() {
  return Singleton<DownloadEnhanceManager>::get();
}

void DownloadEnhanceManager::AddEnhancer(DownloadEnhancer* enhancer) {
  DCHECK(enhancer);

  if (enhancer)
    enhancers_.push_back(enhancer);
}

EnhanceAction
DownloadEnhanceManager::EnhanceShouldOpenDownload(content::DownloadItem* item,
                                                  Profile* profile) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));

  for (int i = 0, len = enhancers_.size(); i < len; i++) {
    EnhanceAction action = enhancers_[i]->OnShouldOpenDownload(item, profile);

    if (action != CONTINUE)
      return action;
  }

  return CONTINUE;
}
