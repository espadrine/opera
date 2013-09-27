// -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
//
// Copyright (C) 2013 Opera Software ASA.  All rights reserved.
//
// This file is an original work developed by Opera Software ASA

#ifndef CHROME_BROWSER_DOWNLOAD_DOWNLOAD_ENHANCE_MANAGER_H_
#define CHROME_BROWSER_DOWNLOAD_DOWNLOAD_ENHANCE_MANAGER_H_

#include <vector>

#include "base/basictypes.h"
#include "base/memory/singleton.h"

class Profile;

namespace content {
class DownloadItem;
}

// Describe if the calling method should continue, or being stopped
// (and either return false or true).
enum EnhanceAction { CONTINUE, RETURN_FALSE, RETURN_TRUE };

// Interface used to modify the behavior of DownloadManagerDelegate.
class DownloadEnhancer {
 public:
  virtual ~DownloadEnhancer() { }

  // Entry point to enhance ShouldOpenDownload.
  virtual EnhanceAction OnShouldOpenDownload(content::DownloadItem* item,
                                             Profile* profile) = 0;
};

// Class that provide a mechanism to enhance the downloads, modifying the
// behavior of DownloadManagerDelegate.
class DownloadEnhanceManager {
 public:
  // Return the singleton instance of the manager, always not NULL.
  // Warning: this is a "Meyer's singleton", and it is used to avoid problems
  // with static constructors initializations.
  static DownloadEnhanceManager* GetInstance();

  // Adds an enhancer to the list, the caller loses ownership of the pointer,
  // and this class is responsible to delete it.
  void AddEnhancer(DownloadEnhancer* enhancer);

  // Called inside ShouldOpenDownload() to enhance its behavior.
  EnhanceAction EnhanceShouldOpenDownload(content::DownloadItem* item,
                                          Profile* profile);

 private:
  DownloadEnhanceManager();
  ~DownloadEnhanceManager();

  friend struct DefaultSingletonTraits<DownloadEnhanceManager>;

  // List of enhancers.
  std::vector<DownloadEnhancer*> enhancers_;

  DISALLOW_COPY_AND_ASSIGN(DownloadEnhanceManager);
};

#endif  // CHROME_BROWSER_DOWNLOAD_DOWNLOAD_ENHANCE_MANAGER_H_
