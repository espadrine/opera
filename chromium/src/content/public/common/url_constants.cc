// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/public/common/url_constants.h"

namespace chrome {

const char kAboutScheme[] = "about";
const char kBlobScheme[] = "blob";

// Before adding new chrome schemes please check with security@chromium.org.
// There are security implications associated with introducing new schemes.
const char kChromeDevToolsScheme[] = "chrome-devtools";
const char kChromeInternalScheme[] = "chrome-internal";
const char kChromeNativeScheme[] = "chrome-native";
const char kChromeUIScheme[] = WEB_UI_SCHEME;
const char kChromeInternalUIScheme[] = WEB_UI_SCHEME "ui";
const char kDataScheme[] = "data";
const char kFileScheme[] = "file";
const char kFileSystemScheme[] = "filesystem";
const char kFtpScheme[] = "ftp";
const char kGuestScheme[] = "chrome-guest";
const char kHttpScheme[] = "http";
const char kHttpsScheme[] = "https";
const char kJavaScriptScheme[] = "javascript";
const char kMailToScheme[] = "mailto";
const char kMetadataScheme[] = "metadata";
const char kSwappedOutScheme[] = "swappedout";
}  // namespace chrome

namespace content {

const char kViewSourceScheme[] = "view-source";

const char kAboutBlankURL[] = "about:blank";
const char kAboutSrcDocURL[] = "about:srcdoc";

const char kChromeUIAppCacheInternalsHost[] = "appcache-internals";
const char kChromeUIIndexedDBInternalsHost[] = "indexeddb-internals";
const char kChromeUIAccessibilityHost[] = "accessibility";
const char kChromeUIBlobInternalsHost[] = "blob-internals";
const char kChromeUIBrowserCrashHost[] = "inducebrowsercrashforrealz";
const char kChromeUIGpuHost[] = "gpu";
const char kChromeUIHistogramHost[] = "histograms";
const char kChromeUIMediaInternalsHost[] = "media-internals";
const char kChromeUINetworkViewCacheHost[] = "view-http-cache";
const char kChromeUIResourcesHost[] = "resources";
const char kChromeUITcmallocHost[] = "tcmalloc";
const char kChromeUITracingHost[] = "tracing";
const char kChromeUIWebRTCInternalsHost[] = "webrtc-internals";

const char kChromeUICrashURL[] = WEB_UI_SCHEME "://crash";
const char kChromeUIGpuCleanURL[] = WEB_UI_SCHEME "://gpuclean";
const char kChromeUIGpuCrashURL[] = WEB_UI_SCHEME "://gpucrash";
const char kChromeUIGpuHangURL[] = WEB_UI_SCHEME "://gpuhang";
const char kChromeUIHangURL[] = WEB_UI_SCHEME "://hang";
const char kChromeUIKillURL[] = WEB_UI_SCHEME "://kill";
const char kChromeUIPpapiFlashCrashURL[] = WEB_UI_SCHEME "://ppapiflashcrash";
const char kChromeUIPpapiFlashHangURL[] = WEB_UI_SCHEME "://ppapiflashhang";

const char kStandardSchemeSeparator[] = "://";

// This error URL is loaded in normal web renderer processes, so it should not
// have a chrome:// scheme that might let it be confused with a WebUI page.
const char kUnreachableWebDataURL[] = "data:text/html,chromewebdata";

const char kChromeUINetworkViewCacheURL[] = WEB_UI_SCHEME "://view-http-cache/";
const char kChromeUIShorthangURL[] = WEB_UI_SCHEME "://shorthang";

// This URL is loaded when a page is swapped out and replaced by a page in a
// different renderer process.  It must have a unique origin that cannot be
// scripted by other pages in the process.
const char kSwappedOutURL[] = "swappedout://";

}  // namespace content
