// Copyright (c) 2013 Yandex LLC. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_TURBO_TURBO_MANAGER_FACTORY_H_
#define CHROME_BROWSER_TURBO_TURBO_MANAGER_FACTORY_H_

#include "base/memory/singleton.h"
#include "components/browser_context_keyed_service/browser_context_keyed_service_factory.h"

class Profile;

namespace opium {

class TurboManager;

class TurboManagerFactory : public BrowserContextKeyedServiceFactory {
 public:
  static TurboManager* GetForProfile(Profile* profile);
  static TurboManagerFactory* GetInstance();
 private:
  friend struct DefaultSingletonTraits<TurboManagerFactory>;

  TurboManagerFactory();
  virtual ~TurboManagerFactory();

  // ProfileKeyedServiceFactory:
  virtual BrowserContextKeyedService* BuildServiceInstanceFor(
      content::BrowserContext* profile) const OVERRIDE;
  virtual bool ServiceIsCreatedWithBrowserContext() const OVERRIDE;
  virtual content::BrowserContext* GetBrowserContextToUse(
      content::BrowserContext* context) const OVERRIDE;
  virtual bool ServiceIsNULLWhileTesting() const OVERRIDE;

  DISALLOW_COPY_AND_ASSIGN(TurboManagerFactory);
};

}  // namespace opium

#endif // CHROME_BROWSER_TURBO_TURBO_MANAGER_FACTORY_H_
