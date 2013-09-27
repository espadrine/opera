# -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
#
# Copyright (C) 2013 Opera Software ASA.  All rights reserved.
#
# This file is an original work developed by Opera Software ASA

{
  'includes': [
    'build/opera_common.gypi',
    'opera_variables.gypi',
  ],
  'targets': [
    {
      'variables': {
        'opera_framework_install_name_base': '@executable_path/../Versions/<(mac_version)',
        'opera_framework_install_name': '<(opera_framework_install_name_base)/<(opera_product_name) Framework.framework/<(opera_product_name) Framework',
      },
      'target_name': 'opera_framework',
      'type': 'shared_library',
      'product_name': '<(opera_product_name) Framework',
      'mac_bundle': 1,
      'xcode_settings': {
        # The dylib versions are of the form a[.b[.c]], where a is a
        # 16-bit unsigned integer, and b and c are 8-bit unsigned
        # integers.  Any missing component is taken to be 0.  The
        # best mapping from product version numbers into this scheme
        # is to just use a=MAJOR, b=(PATCH/256), c=(PATCH%256). There
        # is no ambiguity in this scheme because the build and patch
        # numbers are guaranteed unique even across distinct major
        # and minor version numbers.  These settings correspond to
        # -compatibility_version and -current_version.
        'DYLIB_COMPATIBILITY_VERSION': '<(mac_dylib_version)',
        'DYLIB_CURRENT_VERSION': '<(mac_dylib_version)',

        # The framework is placed within the .app's versioned
        # directory.  DYLIB_INSTALL_NAME_BASE and
        # LD_DYLIB_INSTALL_NAME affect -install_name.
        'DYLIB_INSTALL_NAME_BASE':
            '<(opera_framework_install_name_base)',
        'LD_DYLIB_INSTALL_NAME':
            '$(DYLIB_INSTALL_NAME_BASE:standardizepath)/$(WRAPPER_NAME)/$(PRODUCT_NAME)',
      },
      'libraries': [
        '<(opera_source_release_lib)',
        '$(SDKROOT)/System/Library/Frameworks/AddressBook.framework',
        '$(SDKROOT)/System/Library/Frameworks/SecurityInterface.framework',
      ],
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/base/base.gyp:base_prefs',
        '<(DEPTH)/base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
        '<(DEPTH)/breakpad/breakpad.gyp:breakpad',
        '<(DEPTH)/chrome/chrome_resources.gyp:chrome_strings',
        '<(DEPTH)/chrome/chrome_resources.gyp:theme_resources_gen',
        '<(DEPTH)/components/components.gyp:autofill_content_browser',
        '<(DEPTH)/components/components.gyp:autofill_content_renderer',
        '<(DEPTH)/components/components.gyp:browser_context_keyed_service',
        '<(DEPTH)/components/components.gyp:encryptor',
        '<(DEPTH)/components/components.gyp:user_prefs',
        '<(DEPTH)/components/components.gyp:visitedlink_browser',
        '<(DEPTH)/components/components.gyp:visitedlink_renderer',
        '<(DEPTH)/components/components.gyp:webdata_common',
        '<(DEPTH)/components/components.gyp:web_modal',
        '<(DEPTH)/content/content.gyp:content_app',
        '<(DEPTH)/content/content.gyp:content_browser',
        '<(DEPTH)/content/content.gyp:content_common',
        '<(DEPTH)/content/content.gyp:content_gpu',
        '<(DEPTH)/content/content.gyp:content_plugin',
        '<(DEPTH)/content/content.gyp:content_ppapi_plugin',
        '<(DEPTH)/content/content.gyp:content_renderer',
        '<(DEPTH)/content/content.gyp:content_utility',
        '<(DEPTH)/content/content.gyp:content_worker',
        '<(DEPTH)/google_apis/google_apis.gyp:google_apis',
        '<(DEPTH)/ipc/ipc.gyp:ipc',
        '<(DEPTH)/media/media.gyp:media',
        '<(DEPTH)/net/net.gyp:net',
        '<(DEPTH)/net/net.gyp:net_resources',
        '<(DEPTH)/net/net.gyp:net_with_v8',
        '<(DEPTH)/printing/printing.gyp:printing',
        '<(DEPTH)/skia/skia.gyp:skia',
        '<(DEPTH)/sql/sql.gyp:sql',
        '<(DEPTH)/third_party/cld/cld.gyp:cld',
        '<(DEPTH)/third_party/icu/icu.gyp:icui18n',
        '<(DEPTH)/third_party/libxml/libxml.gyp:libxml',
        '<(DEPTH)/third_party/leveldatabase/leveldatabase.gyp:leveldatabase',
        '<(DEPTH)/third_party/mach_override/mach_override.gyp:mach_override',
        '<(DEPTH)/third_party/modp_b64/modp_b64.gyp:modp_b64',
        '<(DEPTH)/third_party/protobuf/protobuf.gyp:protobuf_lite',
        '<(DEPTH)/third_party/re2/re2.gyp:re2',
        '<(DEPTH)/third_party/widevine/cdm/widevine_cdm.gyp:widevine_cdm_version_h',
        '<(DEPTH)/third_party/zlib/zlib.gyp:zip',
        '<(DEPTH)/tools/json_schema_compiler/api_gen_util.gyp:api_gen_util',
        '<(DEPTH)/ui/base/strings/ui_strings.gyp:ui_strings',
        '<(DEPTH)/ui/message_center/message_center.gyp:message_center',
        '<(DEPTH)/ui/surface/surface.gyp:surface',
        '<(DEPTH)/ui/ui.gyp:shell_dialogs',
        '<(DEPTH)/ui/ui.gyp:ui',
        '<(DEPTH)/ui/ui.gyp:ui_cocoa_third_party_toolkits',
        '<(DEPTH)/ui/ui.gyp:ui_resources',
        '<(DEPTH)/url/url.gyp:url_lib',
        '<(DEPTH)/v8/tools/gyp/v8.gyp:v8',
        '<(DEPTH)/webkit/support/webkit_support.gyp:webkit_support',
        '<(webkit_src_dir)/Source/WebKit/chromium/WebKit.gyp:webkit',
        'common/spellchecker/hunspell.gyp:hunspell',
      ],
      'include_dirs': [
        '<(DEPTH)',
        '..',
      ],
      'sources': [
        'mac/opera_content_main.cc',
        'mac/opera_content_main.h',
      ],
      'postbuilds': [
        {
          'postbuild_name': 'Copy <(opera_product_name) Framework',
          'action': [
            'cp',
            '${BUILT_PRODUCTS_DIR}/${CONTENTS_FOLDER_PATH}/${EXECUTABLE_NAME}',
            '${BUILT_PRODUCTS_DIR}/<(opera_product_name).app/Contents/Versions/<(mac_version)/<(opera_product_name) Framework.framework',
          ],
        },
      ],
    },  # target opera_framework
  ],
}
