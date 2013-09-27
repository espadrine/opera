# -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
#
# Copyright (C) 2013 Opera Software ASA.  All rights reserved.
#
# This file is an original work developed by Opera Software ASA

{
  'variables': {
    'opera_desktop_src_dir': '.',
    'conditions': [
      ['OS=="mac"', {
        'apply_locales_cmd': ['python', '<(DEPTH)/build/apply_locales.py'],
        'mac_app_icon_name': '<!(python common/mkversion.py --mac-app-icon-name)',
        'mac_bundle_id': '<!(python common/mkversion.py --mac-bundle-identifier)',
        'mac_dylib_version': '<!(python common/mkversion.py --mac-dylib-version)',
        'mac_full_version': '<!(python common/mkversion.py --mac-full-version)',
        'mac_short_version': '<!(python common/mkversion.py --mac-short-version)',
        'mac_version': '<!(python common/mkversion.py --mac-version)',
        'opera_plist_file': 'mac/resources/app-Info.plist',
        'opera_framework_plist_file': 'mac/resources/framework-Info.plist',
      }],  # OS=="mac"
    ],
  },
}
