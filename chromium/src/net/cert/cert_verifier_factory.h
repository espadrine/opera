// Copyright (c) 2013 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_BASE_CERT_VERIFIER_FACTORY_H_
#define NET_BASE_CERT_VERIFIER_FACTORY_H_

#include "net/base/net_export.h"

namespace net {

class CertVerifier;

// CertVerifierFactory provides a mechanism for overriding the default
// instance creation process of CertVerifier.
class CertVerifierFactory {
 public:
  CertVerifierFactory() {}
  virtual ~CertVerifierFactory() {}
  virtual CertVerifier* CreateInstance() = 0;
};

}  // namespace net
#endif  // NET_BASE_CERT_VERIFIER_FACTORY_H_
