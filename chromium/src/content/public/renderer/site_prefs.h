// -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
//
// Copyright (C) 2013 Opera Software ASA.  All rights reserved.
//
// This file is an original work developed by Opera Software ASA

#ifndef CONTENT_PUBLIC_RENDERER_SITE_PREFS_H_
#define CONTENT_PUBLIC_RENDERER_SITE_PREFS_H_

#include <string>

#include "base/memory/ref_counted.h"
#include "googleurl/src/gurl.h"

namespace content {

class SitePrefs
    : public base::RefCountedThreadSafe<SitePrefs> {
 public:
  virtual std::string GetUserAgent(const GURL& url) = 0;
 protected:
  friend class base::RefCountedThreadSafe<SitePrefs>;
  virtual ~SitePrefs() {}
};

}  // namespace content

#endif  // CONTENT_PUBLIC_RENDERER_SITE_PREFS_H_
