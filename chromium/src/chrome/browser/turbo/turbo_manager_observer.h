// Copyright (c) 2012 Yandex LLC. All rights reserved.
// Author: Sergey Poletaev <spylogsster@yandex-team.ru>

#ifndef CHROME_BROWSER_TURBO_TURBO_MANAGER_OBSERVER_H_
#define CHROME_BROWSER_TURBO_TURBO_MANAGER_OBSERVER_H_
#pragma once

#include "chrome/browser/turbo/turbo_manager.h"

namespace opium {

class TurboManager;

// Observer for the TurboManager.
class TurboManagerObserver {
 public:
  // Invoked when the TurboManager state changed.
  virtual void TurboStateChanged(yandex::TurboState state) = 0;
  // Invoked when TurboManager has destroyed
  virtual void TurboManagerDestroyed() = 0;

 protected:
  virtual ~TurboManagerObserver() {}
};

}  // namespace yandex

#endif  // CHROME_BROWSER_TURBO_TURBO_MANAGER_OBSERVER_H_
