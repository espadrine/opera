// Copyright (c) 2013 Yandex LLC. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/turbo/turbo_manager_factory.h"

#include "chrome/browser/profiles/incognito_helpers.h"
#include "chrome/browser/turbo/turbo_manager.h"
#include "components/browser_context_keyed_service/browser_context_dependency_manager.h"

namespace opium {

// static
TurboManager* TurboManagerFactory::GetForProfile(Profile* profile) {
  return static_cast<TurboManager*>(
        profile ? GetInstance()->GetServiceForBrowserContext(profile, true) : NULL);
}

// static
TurboManagerFactory* TurboManagerFactory::GetInstance() {
  return Singleton<TurboManagerFactory>::get();
}

TurboManagerFactory::TurboManagerFactory()
    : BrowserContextKeyedServiceFactory("TurboManager",
					BrowserContextDependencyManager::GetInstance()) {
}

TurboManagerFactory::~TurboManagerFactory() {
}

BrowserContextKeyedService* TurboManagerFactory::BuildServiceInstanceFor(
    content::BrowserContext* profile) const {
  return new TurboManager(static_cast<Profile*>(profile));
}

bool TurboManagerFactory::ServiceIsCreatedWithBrowserContext() const {
  return true;
}

content::BrowserContext* TurboManagerFactory::GetBrowserContextToUse(
    content::BrowserContext* context) const {
  return chrome::GetBrowserContextRedirectedInIncognito(context);
}

bool TurboManagerFactory::ServiceIsNULLWhileTesting() const {
  return true;
}

}  // namespace opium
