// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_POLICY_TURBO_BLACKLIST_MANAGER_H_
#define CHROME_BROWSER_POLICY_TURBO_BLACKLIST_MANAGER_H_

#include <map>
#include <string>
#include <vector>
#include <list>

#include "base/basictypes.h"
#include "base/callback_forward.h"
#include "base/compiler_specific.h"
#include "base/containers/hash_tables.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/time.h"
#include "base/prefs/pref_change_registrar.h"
#include "content/public/browser/notification_observer.h"

class GURL;
class PrefService;

namespace policy {

class TurboBlacklistItem {
public:

  TurboBlacklistItem(std::string host, std::string path, bool subdomains);

  bool IsBlocked(std::string host, std::string path);
  bool Exists(std::string host, std::string path, bool subdomains);
private:
  std::string host_;
  std::string path_;
  bool subdomains_;

  DISALLOW_COPY_AND_ASSIGN(TurboBlacklistItem);
};

// Contains a set of filters to block certain URLs, and matches GURLs
// against this set.
class TurboBlacklist {
 public:

  TurboBlacklist();
  virtual ~TurboBlacklist();

  // Add block for host, which expires at expiry
  void Block(const std::string& value, base::Time expiry);

  // Returns true if the URL is blocked.
  bool IsURLBlocked(const GURL& url) const;

 private:
  std::list<TurboBlacklistItem*>* forced_turbo_exclusions_;

  DISALLOW_COPY_AND_ASSIGN(TurboBlacklist);
};

// Tracks the blacklist policies for a given profile, and updates it on changes.
//
// This class interacts with both the UI thread, where notifications of pref
// changes are received from, and the IO thread, which owns it (in the
// ProfileIOData) and checks for blacklisted URLs (from ChromeNetworkDelegate).
//
// It must be constructed on the UI thread, to set up |ui_weak_ptr_factory_| and
// the prefs listeners.
//
// ShutdownOnUIThread must be called from UI before destruction, to release
// the prefs listeners on the UI thread. This is done from ProfileIOData.
//
// Update tasks from the UI thread can post safely to the IO thread, since the
// destruction order of Profile and ProfileIOData guarantees that if this
// exists in UI, then a potential destruction on IO will come after any task
// posted to IO from that method on UI. This is used to go through IO before
// the actual update starts, and grab a WeakPtr.
class TurboBlacklistManager : public content::NotificationObserver {
 public:
  // Must be constructed on the UI thread.
  explicit TurboBlacklistManager(PrefService* pref_service);
  virtual ~TurboBlacklistManager();

  // Must be called on the UI thread, before destruction.
  void ShutdownOnUIThread();

  // Returns true if |url| is blocked by the current blacklist. Must be called
  // from the IO thread.
  bool IsURLBlocked(const GURL& url) const;

  // Replaces the current blacklist. Must be called on the IO thread.
  void SetBlacklist(TurboBlacklist* blacklist);

  // Add new host to blacklist
  void Block(const std::string& value, base::Time expiry) const;

 protected:
  typedef std::map<std::string, base::Time> StringTimeMap;

  // Used to delay updating the blacklist while the preferences are
  // changing, and execute only one update per simultaneous prefs changes.
  void ScheduleUpdate();

  // Updates the blacklist using the current preference values.
  // Virtual for testing.
  virtual void Update();

  // Starts the blacklist update on the IO thread, using the filters in
  // |block| and |allow|. Protected for testing.
  void UpdateOnIO(StringTimeMap* block);

 private:
  virtual void Observe(int type,
                       const content::NotificationSource& source,
                       const content::NotificationDetails& details) OVERRIDE;

  // ---------
  // UI thread
  // ---------

  // Used to post update tasks to the UI thread.
  base::WeakPtrFactory<TurboBlacklistManager> ui_weak_ptr_factory_;

  // Add new host to blacklist to preferences
  void AddBlockPrefOnUI(const std::string& value, base::Time expiry) const;

  // Remove expired block to blacklist to preferences
  void RemoveBlockPrefOnUI(const std::string& value) const;

  // Used to track the policies and update the blacklist on changes.
  PrefChangeRegistrar pref_change_registrar_;
  PrefService* pref_service_;  // Weak.

  // ---------
  // IO thread
  // ---------

  // Used to get |weak_ptr_| to self on the IO thread.
  base::WeakPtrFactory<TurboBlacklistManager> io_weak_ptr_factory_;

  // The current blacklist.
  scoped_ptr<TurboBlacklist> blacklist_;

  DISALLOW_COPY_AND_ASSIGN(TurboBlacklistManager);
};

}  // namespace policy

#endif  // CHROME_BROWSER_POLICY_TURBO_BLACKLIST_MANAGER_H_
