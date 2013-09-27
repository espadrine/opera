// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/cert/cert_verifier.h"

#include "net/cert/cert_verifier_factory.h"
#include "net/cert/cert_verify_proc.h"
#include "net/cert/multi_threaded_cert_verifier.h"

namespace net {

// Class factory singleton.
CertVerifierFactory* g_cert_verifier_factory = NULL;

// static
void CertVerifier::SetFactory(CertVerifierFactory* factory) {
  CHECK(!g_cert_verifier_factory);
  g_cert_verifier_factory = factory;
}

CertVerifier* CertVerifier::CreateDefault() {
  if (g_cert_verifier_factory)
    return g_cert_verifier_factory->CreateInstance();
  return new MultiThreadedCertVerifier(CertVerifyProc::CreateDefault());
}

}  // namespace net
