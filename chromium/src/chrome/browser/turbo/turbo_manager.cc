// Copyright (c) 2013 Yandex LLC. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/turbo/turbo_manager.h"

#include <string>
#include "base/command_line.h"
#include "base/logging.h"
#include "chrome/common/content_settings_types.h"
#include "chrome/common/chrome_notification_types.h"
#include "chrome/common/pref_names.h"
#include "chrome/browser/content_settings/tab_specific_content_settings.h"
#include "chrome/browser/prefs/scoped_user_pref_update.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_finder.h"
#include "chrome/browser/ui/browser_list.h"
#include "chrome/browser/ui/browser_tabstrip.h"
#include "chrome/browser/ui/browser_iterator.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "content/common/view_messages.h"
#include "content/browser/renderer_host/render_view_host_delegate.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/notification_source.h"
#include "content/public/browser/notification_types.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/url_constants.h"
#include "crypto/sha2.h"
#include "net/http/http_stream_factory.h"

namespace opium {

const unsigned int TurboManager::kTurboClientIDSize = 32;

TurboManager::TurboManager(Profile* profile)
     : TabStripModelGlobalObserver(profile),
       state_(TURBO_DISABLED),
       profile_(profile),
       pref_change_registrar_(new PrefChangeRegistrar), 
       weak_factory_(this) {
  PrefService* preferences = profile_->GetPrefs(); 
  turbo_client_id_ = preferences->GetString(prefs::kTurboClientID);
  if (turbo_client_id_.empty() || turbo_client_id_.length() != TurboManager::kTurboClientIDSize * 2) {
    turbo_client_id_ = GenerateClientID();
    preferences->SetString(prefs::kTurboClientID, turbo_client_id_);
  }
  net::HttpStreamFactory::set_turbo_client_id(turbo_client_id_);
  UpdateTurboMode();
  pref_change_registrar_->Init(profile->GetPrefs());
  pref_change_registrar_->Add(
      prefs::kTurboEnabled,
      base::Bind(&TurboManager::UpdateTurboMode,
                 base::Unretained(this)));
}

void TurboManager::UpdateTurboMode() {
  PrefService* preferences = profile_->GetPrefs(); 
  TurboState new_state = preferences->GetBoolean(prefs::kTurboEnabled) ? TURBO_ENABLED : TURBO_DISABLED;
  if (new_state == state())
    return;

  SwitchTurbo(new_state);
}

TurboManager::~TurboManager() {
}

std::string TurboManager::GenerateClientID() {
  std::string result;
  const char hexchars[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

  srand(time(NULL));

  for (unsigned int i = 0; i < TurboManager::kTurboClientIDSize; i++) {
    char x = rand()%255;
    result += x;
  }
  std::string digest = crypto::SHA256HashString(result);
  result.clear();

  for (unsigned int i = 0; i < digest.length(); i++) {
    unsigned char val = digest.c_str()[i];
    result += hexchars[(val>>4) & 0x0f];
    result += hexchars[val & 0x0f];
  }

  return result;
}

TurboState TurboManager::state() const {
  return state_;
}

bool TurboManager::enabled() const {
  return state() == TURBO_ENABLED;
}

void TurboManager::ActiveTabChanged(content::WebContents* old_contents,
                                    content::WebContents* new_contents,
                                    int index,
                                    int reason) {
  if (!enabled() && new_contents && HasContentBlockedByTurboFor(new_contents))
    UnblockContentFor(new_contents);
}

void TurboManager::TabReplacedAt(TabStripModel* tab_strip_model,
                                 content::WebContents* old_contents,
                                 content::WebContents* new_contents,
                                 int index) {
  if (!enabled() && new_contents && HasContentBlockedByTurboFor(new_contents))
    UnblockContentFor(new_contents);
}

void TurboManager::TurnOnTurbo() {
  SwitchTurbo(TURBO_ENABLED);
}

void TurboManager::TurnOffTurbo() {
  SwitchTurbo(TURBO_DISABLED);
}

bool TurboManager::HasContentBlockedByTurboFor(content::WebContents* contents) {
  if (!contents) {
    NOTREACHED() << "We have to use a valid |wrapper|";
    return false;
  }

  TabSpecificContentSettings* content_settings = GetContentSettings(contents);
  for (int i = 0; i < TURBO_CONTENT_SETTINGS_NUM_TYPES; ++i) {
    TurboContentSettingsType type =
        static_cast<TurboContentSettingsType>(i);
    if (content_settings->IsContentBlockedByTurbo(type)) {
      return true;
    }
  }
  return false;
}

void TurboManager::UnblockContentInBrowser(Browser* browser) {
  DCHECK(browser);
  content::WebContents* contents =
      browser->tab_strip_model()->GetActiveWebContents();
  DCHECK(contents);
  if (contents && HasContentBlockedByTurboFor(contents))
    UnblockContentFor(contents);
}

void TurboManager::UnblockContentInAllBrowsers() {
  for (chrome::BrowserIterator iter; !iter.done(); iter.Next()) {
    UnblockContentInBrowser(*iter);
  }
}

void TurboManager::RefreshWebKitPreferences() {
  for (chrome::BrowserIterator iter; !iter.done(); iter.Next()) {
    Browser* browser = *iter;
    TabStripModel* tab_strip_model = browser->tab_strip_model();
    for (int t = 0; t < tab_strip_model->count(); ++t) {
      content::WebContents* contents =
          tab_strip_model->GetWebContentsAt(t);
      content::RenderViewHost* render_view_host =
          contents->GetRenderViewHost();
      if (!render_view_host)
        continue;
      content::RenderViewHostDelegate* rvhd = render_view_host->GetDelegate();
      render_view_host->UpdateWebkitPreferences(rvhd->GetWebkitPrefs());
    }
  }
}

void TurboManager::UnblockContentFor(content::WebContents* content) {
  if (!content) {
    NOTREACHED() << "We have to use a valid |content|";
    return;
  }
  content::RenderViewHost* render_view_host = content->GetRenderViewHost();
  if (!render_view_host) {
    NOTREACHED() << "No RenderViewHost to unblock content";
    return;
  }

  content->Send(new ViewMsg_LoadBlockedByTurboContent(
      render_view_host->GetRoutingID()));
}

void TurboManager::SwitchTurbo(TurboState new_state) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
  if (new_state == state())
    return;
  if (new_state == TURBO_DISABLED) {
      UnblockContentInAllBrowsers();
  }

  state_ = new_state;

  RefreshWebKitPreferences();
}

TabSpecificContentSettings*
TurboManager::GetContentSettings(content::WebContents* contents) {
  if (!contents)
    return NULL;
  return TabSpecificContentSettings::FromWebContents(contents);
}

bool TurboManager::ShouldUseTurboForUrl(const GURL& url) const {
  if (!enabled())
    return false;
  return url.scheme() != chrome::kFileScheme;
}

}  // namespace opium
