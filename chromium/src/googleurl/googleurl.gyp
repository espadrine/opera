# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      'target_name': 'googleurl',
      'type': '<(component)',
      'dependencies': [
        '../base/base.gyp:base',
        '../third_party/icu/icu.gyp:icudata',
        '../third_party/icu/icu.gyp:icui18n',
        '../third_party/icu/icu.gyp:icuuc',
      ],
      'sources': [
        'src/gurl.cc',
        'src/gurl.h',
        'src/url_canon.h',
        'src/url_canon_etc.cc',
        'src/url_canon_fileurl.cc',
        'src/url_canon_filesystemurl.cc',
        'src/url_canon_host.cc',
        'src/url_canon_icu.cc',
        'src/url_canon_icu.h',
        'src/url_canon_internal.cc',
        'src/url_canon_internal.h',
        'src/url_canon_internal_file.h',
        'src/url_canon_ip.cc',
        'src/url_canon_ip.h',
        'src/url_canon_mailtourl.cc',
        'src/url_canon_path.cc',
        'src/url_canon_pathurl.cc',
        'src/url_canon_query.cc',
        'src/url_canon_relative.cc',
        'src/url_canon_stdstring.h',
        'src/url_canon_stdurl.cc',
        'src/url_file.h',
        'src/url_parse.cc',
        'src/url_parse.h',
        'src/url_parse_file.cc',
        'src/url_parse_internal.h',
        'src/url_util.cc',
        'src/url_util.h',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '../..',
        ],
      },
      'defines': [
        'FULL_FILESYSTEM_URL_SUPPORT=1',
      ],
      'conditions': [
        ['component=="shared_library"', {
          'defines': [
            'GURL_DLL',
            'GURL_IMPLEMENTATION=1',
          ],
          'direct_dependent_settings': {
            'defines': [
              'GURL_DLL',
            ],
          },
        }],
      ],
      # TODO(jschuh): crbug.com/167187 fix size_t to int truncations.
      'msvs_disabled_warnings': [4267, ],
    },
    {
      'target_name': 'googleurl_unittests',
      'type': 'executable',
      'dependencies': [
        'googleurl',
        '../base/base.gyp:base_i18n',
        '../base/base.gyp:run_all_unittests',
        '../testing/gtest.gyp:gtest',
        '../third_party/icu/icu.gyp:icuuc',
      ],
      'sources': [
        'src/gurl_unittest.cc',
        'src/url_canon_unittest.cc',
        'src/url_parse_unittest.cc',
        'src/url_test_utils.h',
        'src/url_util_unittest.cc',
      ],
      'defines': [
        'FULL_FILESYSTEM_URL_SUPPORT=1',
      ],
      'conditions': [
        ['os_posix==1 and OS!="mac" and OS!="ios"', {
          'conditions': [
            ['linux_use_tcmalloc==1', {
              'dependencies': [
                '../base/allocator/allocator.gyp:allocator',
              ],
            }],
          ],
        }],
      ],
      # TODO(jschuh): crbug.com/167187 fix size_t to int truncations.
      'msvs_disabled_warnings': [4267, ],
    },
  ],
}
