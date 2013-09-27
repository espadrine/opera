// Copyright (c) 2013 Yandex LLC. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/tabs/tab_strip_model_global_observer.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_list.h"
#include "chrome/browser/ui/browser_iterator.h"

namespace opium {

TabStripModelGlobalObserver::TabStripModelGlobalObserver(Profile* profile)
    : registered_(false),
      profile_(profile) {
  BrowserList::AddObserver(this);
  for (chrome::BrowserIterator iter; !iter.done(); iter.Next()) {
    RegisterForBrowserNotifications(*iter);
  }
  registered_ = true;
}

TabStripModelGlobalObserver::~TabStripModelGlobalObserver() {
  UnregisterObservers();
}

void TabStripModelGlobalObserver::UnregisterObservers() {
  if (!registered_)
    return;
  registered_ = false;
  BrowserList::RemoveObserver(this);
  std::set<const Browser*>::iterator it = observed_browsers_.begin();
  for (; it != observed_browsers_.end(); ++it)
    (*it)->tab_strip_model()->RemoveObserver(this);
}

void TabStripModelGlobalObserver::RegisterForBrowserNotifications(
    const Browser* browser) {
  if (browser->profile() == profile_) {
    browser->tab_strip_model()->AddObserver(this);
    observed_browsers_.insert(browser);
  }
}

// BrowserList::Observer
void TabStripModelGlobalObserver::OnBrowserAdded(Browser* browser) {
  RegisterForBrowserNotifications(browser);
}

// BrowserList::Observer
void TabStripModelGlobalObserver::OnBrowserRemoved(Browser* browser) {
  browser->tab_strip_model()->RemoveObserver(this);
  observed_browsers_.erase(browser);
}

}  // namespace opium
