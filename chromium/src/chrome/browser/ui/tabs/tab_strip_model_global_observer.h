// Copyright (c) 2013 Yandex LLC. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_TABS_TAB_STRIP_MODEL_GLOBAL_OBSERVER_H_
#define CHROME_BROWSER_UI_TABS_TAB_STRIP_MODEL_GLOBAL_OBSERVER_H_

#include <set>
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/tabs/tab_strip_model_observer.h"
#include "chrome/browser/ui/browser_list_observer.h"

class Browser;

namespace opium {
// Base class for objects, which need observer tab strip models in all
// browsers for some profile.
class TabStripModelGlobalObserver : public TabStripModelObserver,
                                    public chrome::BrowserListObserver {
 public:
  explicit TabStripModelGlobalObserver(Profile* profile);
  virtual ~TabStripModelGlobalObserver();

 protected:
  void UnregisterObservers();
  // BrowserList::Observer
  virtual void OnBrowserAdded(Browser* browser) OVERRIDE;
  virtual void OnBrowserRemoved(Browser* browser) OVERRIDE;

 protected:
  void RegisterForBrowserNotifications(const Browser* browser);

  // Browsers that we are listening to via TabStripModelObserver
  std::set<const Browser*> observed_browsers_;
  bool registered_;
  Profile* profile_;
};

}  // namespace opium

#endif  // CHROME_BROWSER_UI_TABS_TAB_STRIP_MODEL_GLOBAL_OBSERVER_H_
