// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/policy/turbo_blacklist_manager.h"

#include "base/bind.h"
#include "base/time.h"
#include "base/message_loop.h"
#include "base/stl_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_piece.h"
#include "base/strings/string_util.h"
#include "base/values.h"
#include "chrome/browser/net/url_fixer_upper.h"
#include "chrome/browser/prefs/scoped_user_pref_update.h"
#include "chrome/common/chrome_notification_types.h"
#include "chrome/common/pref_names.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/notification_details.h"
#include "content/public/browser/notification_source.h"
#include "googleurl/src/gurl.h"

using content::BrowserThread;
using base::StringPiece;

namespace policy {

namespace {

typedef std::map<std::string, base::Time> StringTimeMap;

StringTimeMap* DictValueToStringTimeMap(const base::DictionaryValue* dict) {
  StringTimeMap* map = new StringTimeMap;

  if (dict == NULL)
    return map;

  std::string s;
  base::Time t;
  for (base::DictionaryValue::Iterator it(*dict);
       !it.IsAtEnd(); it.Advance()) {
    std::string value = it.key();
    std::string s;
    dict->GetStringWithoutPathExpansion(value, &s);
    int64 expiry;
    base::StringToInt64(s, &expiry);
    map->insert(std::pair<std::string,
                base::Time>(value, base::Time::FromInternalValue(expiry)));
  }

  return map;
}

// A task that builds the blacklist on the FILE thread. Takes ownership
// of |block| and but not of |blacklist|.
void BuildBlacklist(TurboBlacklist* blacklist,
                    StringTimeMap* block) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::FILE));

  scoped_ptr<StringTimeMap> scoped_block(block);

  for (StringTimeMap::iterator it = block->begin(); it != block->end(); ++it) {
    blacklist->Block(it->first, it->second);
  }
}

// A task that owns the TurboBlacklist, and passes it to the TurboBlacklistManager
// on the IO thread, if the TurboBlacklistManager still exists.
void SetBlacklistOnIO(base::WeakPtr<TurboBlacklistManager> blacklist_manager,
                      TurboBlacklist* blacklist) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  if (blacklist_manager) {
    blacklist_manager->SetBlacklist(blacklist);
  } else {
    delete blacklist;
  }
}

}  // namespace

TurboBlacklist::TurboBlacklist():forced_turbo_exclusions_(NULL) {
}

TurboBlacklist::~TurboBlacklist() {
  delete forced_turbo_exclusions_;
  forced_turbo_exclusions_ = NULL;
}

void TurboBlacklist::Block(const std::string& value, base::Time expiry) {
  std::string host;
  std::string temp = value;

  if (!forced_turbo_exclusions_)
    forced_turbo_exclusions_ = new std::list<TurboBlacklistItem*>();

  bool subdomains = false;
  size_t found;
  found = temp.find('*');
  if (found != std::string::npos) {
    subdomains = true;
    temp = temp.substr(found+1);
  }
  found = temp.find('/');
  if (found != std::string::npos) {
    host = temp.substr(0, found);
    temp = temp.substr(found);
  }
  else
  {
    host = temp;
    temp = "/";
  }

  std::list<TurboBlacklistItem*>::iterator it;
  for (it = forced_turbo_exclusions_->begin();
       it != forced_turbo_exclusions_->end(); ++it) {
    if ((*it)->Exists(host, temp, subdomains))
      return;
  }

  TurboBlacklistItem *item = new TurboBlacklistItem(host, temp, subdomains);
  forced_turbo_exclusions_->push_back(item);
}

bool TurboBlacklist::IsURLBlocked(const GURL& url) const {
  std::string endpoint(url.HostNoBrackets());
  std::string path(url.PathForRequest());
  std::list<TurboBlacklistItem*>::iterator it;
  std::list<TurboBlacklistItem*>* exclusions = forced_turbo_exclusions_;
  if (!exclusions) {
    return false;
  }
  for (it = exclusions->begin(); it != exclusions->end(); ++it) {
    if ((*it)->IsBlocked(endpoint, path))
      return true;
  }
  return false;
}

TurboBlacklistItem::TurboBlacklistItem(std::string host, std::string path, bool subdomains) : host_(host), path_(path), subdomains_(subdomains) {
}

bool TurboBlacklistItem::IsBlocked(std::string host_a, std::string path_a) {
  bool host_match = false;

  if (((!subdomains_ && host_a == host_) || (subdomains_ && host_a.size() >= host_.size() && std::equal(host_.rbegin(), host_.rend(), host_a.rbegin()))))
    host_match = true;

  if (host_match && (path_=="/" || (path_a.size() >= path_.size() && std::equal(path_.begin(), path_.end(), path_a.begin()))))
    return true;
  return false;
}

bool TurboBlacklistItem::Exists(std::string host, std::string path, bool subdomains) {
  return subdomains_ == subdomains && host == host_ && path == path_;
}

TurboBlacklistManager::TurboBlacklistManager(PrefService* pref_service)
    : ui_weak_ptr_factory_(this),
      pref_service_(pref_service),
      io_weak_ptr_factory_(this),
      blacklist_(new TurboBlacklist) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  pref_change_registrar_.Init(pref_service_);
  base::Closure callback = base::Bind(&TurboBlacklistManager::ScheduleUpdate,
                                      base::Unretained(this));

  pref_change_registrar_.Add(prefs::kTurboUrlBlacklist, callback);

  // Start enforcing the policies without a delay when they are present at
  // startup.
  if (pref_service_->HasPrefPath(prefs::kTurboUrlBlacklist))
    Update();
}

void TurboBlacklistManager::ShutdownOnUIThread() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  // Cancel any pending updates, and stop listening for pref change updates.
  ui_weak_ptr_factory_.InvalidateWeakPtrs();
  pref_change_registrar_.RemoveAll();
}

TurboBlacklistManager::~TurboBlacklistManager() {
}

void TurboBlacklistManager::Observe(int type,
                                  const content::NotificationSource& source,
                                  const content::NotificationDetails& details) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  //DCHECK(type == chrome::NOTIFICATION_PREF_CHANGED);
  PrefService* prefs = content::Source<PrefService>(source).ptr();
  DCHECK(prefs == pref_service_);
  std::string* pref_name = content::Details<std::string>(details).ptr();
  DCHECK(*pref_name == prefs::kTurboUrlBlacklist);
  ScheduleUpdate();
}

void TurboBlacklistManager::ScheduleUpdate() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  // Cancel pending updates, if any. This can happen if two preferences that
  // change the blacklist are updated in one message loop cycle. In those cases,
  // only rebuild the blacklist after all the preference updates are processed.
  ui_weak_ptr_factory_.InvalidateWeakPtrs();
  base::MessageLoop::current()->PostTask(
      FROM_HERE,
      base::Bind(&TurboBlacklistManager::Update,
                 ui_weak_ptr_factory_.GetWeakPtr()));
}

void TurboBlacklistManager::Update() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  // The preferences can only be read on the UI thread.
  // StringVector* block =
  //     ListValueToStringVector(pref_service_->GetList(prefs::kTurboUrlBlacklist));
  StringTimeMap* block =
      DictValueToStringTimeMap(pref_service_->GetDictionary(prefs::kTurboUrlBlacklist));

  // Go through the IO thread to grab a WeakPtr to |this|. This is safe from
  // here, since this task will always execute before a potential deletion of
  // ProfileIOData on IO.
  BrowserThread::PostTask(BrowserThread::IO, FROM_HERE,
                          base::Bind(&TurboBlacklistManager::UpdateOnIO,
                                     base::Unretained(this), block));
}

void TurboBlacklistManager::UpdateOnIO(StringTimeMap* block) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  TurboBlacklist* blacklist = new TurboBlacklist;
  // The TurboBlacklist is built on the FILE thread. Once it's ready, it is passed
  // to the TurboBlacklistManager on IO.
  // |blacklist| and |block| can leak on the unlikely event of a
  // policy update and shutdown happening at the same time.
  BrowserThread::PostTaskAndReply(BrowserThread::FILE, FROM_HERE,
                                  base::Bind(&BuildBlacklist,
                                             blacklist, block),
                                  base::Bind(&SetBlacklistOnIO,
                                             io_weak_ptr_factory_.GetWeakPtr(),
                                             blacklist));
}

void TurboBlacklistManager::SetBlacklist(TurboBlacklist* blacklist) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  blacklist_.reset(blacklist);
}

bool TurboBlacklistManager::IsURLBlocked(const GURL& url) const {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  return blacklist_->IsURLBlocked(url);
}

void split_string(std::string& str, char sepa,
                  std::vector<std::string>* sub_strs) {
  size_t b = 0;
  size_t e = str.find_first_of(sepa, b);
  while (e != std::string::npos && e > b) {
    sub_strs->push_back(str.substr(b, e - b));
    b = e + 1;
    e = str.find_first_of(sepa, b);
  }
  if (b < str.size()) {
    sub_strs->push_back(str.substr(b));
  }
}

void TurboBlacklistManager::Block(const std::string& value, base::Time expiry) const {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

  if (base::Time::Now() > expiry) {
    BrowserThread::PostTask(BrowserThread::UI, FROM_HERE,
                            base::Bind(&TurboBlacklistManager::RemoveBlockPrefOnUI,
                            base::Unretained(this), value));
    return;
  }

  std::vector<std::string> query;
  std::string domain;
  std::string ip;
  std::string path;
  std::string temp = value;
  split_string(temp, ' ', &query);

  for (unsigned long i=0; i<3 && i < query.size(); i++) {
    std::vector<std::string> params;
    split_string(query[i], '=', &params);
    if (params[1].length() > 3)
    {
      std::string test = params[1];
      params[1] = params[1].substr(1, params[1].length() - 3);
      if (params[0] == "domain") {
        domain = params[1];
      }
      else if (params[0] == "path") {
        path = params[1];
      }
    }
  }

  BrowserThread::PostTask(BrowserThread::UI, FROM_HERE,
                          base::Bind(&TurboBlacklistManager::AddBlockPrefOnUI,
                                     base::Unretained(this), domain + path, expiry));
  return;
}

void TurboBlacklistManager::AddBlockPrefOnUI(const std::string& value, base::Time expiry) const {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  DictionaryPrefUpdate update(pref_service_, prefs::kTurboUrlBlacklist);
  DictionaryValue* dict = update.Get();
  if (dict) {
    int64 t = expiry.ToInternalValue();
    dict->SetWithoutPathExpansion(value, Value::CreateStringValue(base::Int64ToString(t)));
  }
  return;
}

void TurboBlacklistManager::RemoveBlockPrefOnUI(const std::string& value) const {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  const DictionaryValue* cdict = pref_service_->GetDictionary(prefs::kTurboUrlBlacklist);
  if (cdict && !cdict->HasKey(value))
    return;

  DictionaryPrefUpdate update(pref_service_, prefs::kTurboUrlBlacklist);
  DictionaryValue* dict = update.Get();
  if (dict) {
    dict->RemoveWithoutPathExpansion(value, NULL);
  }
  return;
}
}  // namespace policy
