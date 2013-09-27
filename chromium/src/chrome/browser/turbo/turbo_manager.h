// Copyright (c) 2013 Yandex LLC. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_TURBO_TURBO_MANAGER_H_
#define CHROME_BROWSER_TURBO_TURBO_MANAGER_H_

#include "base/observer_list.h"
#include "base/prefs/pref_change_registrar.h"
#include "base/timer.h"
#include "components/browser_context_keyed_service/browser_context_keyed_service.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"
#include "chrome/browser/ui/tabs/tab_strip_model_global_observer.h"

class Browser;
class PrefChangeRegistrar;
class GURL;
class Profile;
class TabContents;
class TabSpecificContentSettings;

namespace net {
class URLRequestContextGetter;
}

namespace opium {

class TurboManagerObserver;

enum TurboState {
  TURBO_ENABLED,
  TURBO_DISABLED
};

// At present responsible only for showing information about turbo blocked
// content. In future may preserve preferences of content blocking for different
// sites, information about number of bytes, saved by Turbo, etc.
class TurboManager : public BrowserContextKeyedService,
                     public TabStripModelGlobalObserver {
 public:
  explicit TurboManager(Profile* profile);
  virtual ~TurboManager();

  // Returns true if Turbo is currently on and false otherwise. Status
  // ENABLED_AUTOMATICALLY is resolved internally by this manager.
  bool enabled() const;

  TurboState state() const;
  // Send turbo core the message about turning on the mode.
  void TurnOnTurbo();
  // Send turbo core the message about turning off the mode.
  void TurnOffTurbo();
  // Determines the contents has any content blocked by turbo or not.
  bool HasContentBlockedByTurboFor(content::WebContents* contents);
  // Unblock cintent in active tabs of all browsers.
  void UnblockContentInAllBrowsers();
  void RefreshWebKitPreferences();
  // Unblock content for active tab of a specific browser.
  void UnblockContentInBrowser(Browser* browser);
  // Unblock content of specific tab.
  void UnblockContentFor(content::WebContents* content);
  bool ShouldUseTurboForUrl(const GURL& url) const;

protected:
  // Event when state of TurboManager should be changed.
  void UpdateTurboMode();
  void SwitchTurbo(TurboState state);

private:
  // Overriden from TabStripModelObserver
  virtual void ActiveTabChanged(content::WebContents* old_contents,
                                content::WebContents* new_contents,
                                int index,
                                int reason) OVERRIDE;
  virtual void TabReplacedAt(TabStripModel* tab_strip_model,
                             content::WebContents* old_contents,
                             content::WebContents* new_contents,
                             int index) OVERRIDE;
  // Gets content settings from wrapped content

  TabSpecificContentSettings* GetContentSettings(
      content::WebContents* contents);
  static const unsigned int kTurboClientIDSize;
  std::string GenerateClientID();
  // State flag.
  TurboState state_;
  // Unique client ID
  std::string turbo_client_id_;
  // Profile where the manager is created.
  Profile* profile_;

  scoped_ptr<PrefChangeRegistrar> pref_change_registrar_; 
  base::WeakPtrFactory<TurboManager> weak_factory_;
  DISALLOW_COPY_AND_ASSIGN(TurboManager);
};

}  // namespace opium

#endif  // CHROME_BROWSER_TURBO_TURBO_MANAGER_H_
