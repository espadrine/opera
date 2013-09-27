# Copyright (c) 2009 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# Modified by Opera Software ASA

{
  'targets': [
    {
      'target_name': 'hunspell',
      'type': 'static_library',
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/third_party/icu/icu.gyp:icuuc',
      ],
      'include_dirs': [
        '<(DEPTH)',
        '.',
        'third_party/hunspell/src/hunspell',
        'third_party/hunspell/src/parsers',
      ],
      'defines': [
        'HUNSPELL_OPERA_CLIENT',
        'HUNSPELL_STATIC',
        'OPENOFFICEORG',
      ],
      'sources': [
        'third_party/hunspell/src/hunspell/affentry.cxx',
        'third_party/hunspell/src/hunspell/affentry.hxx',
        'third_party/hunspell/src/hunspell/affixmgr.cxx',
        'third_party/hunspell/src/hunspell/affixmgr.hxx',
        'third_party/hunspell/src/hunspell/atypes.hxx',
        'third_party/hunspell/src/hunspell/baseaffix.hxx',
        'third_party/hunspell/src/hunspell/csutil.cxx',
        'third_party/hunspell/src/hunspell/csutil.hxx',
        'third_party/hunspell/src/hunspell/dictmgr.cxx',
        'third_party/hunspell/src/hunspell/dictmgr.hxx',
        'third_party/hunspell/src/hunspell/filemgr.cxx',
        'third_party/hunspell/src/hunspell/filemgr.hxx',
        'third_party/hunspell/src/hunspell/hashmgr.cxx',
        'third_party/hunspell/src/hunspell/hashmgr.hxx',
        'third_party/hunspell/src/hunspell/htypes.hxx',
        'third_party/hunspell/src/hunspell/hunspell.cxx',
        'third_party/hunspell/src/hunspell/hunspell.h',
        'third_party/hunspell/src/hunspell/hunspell.hxx',
        'third_party/hunspell/src/hunspell/hunzip.cxx',
        'third_party/hunspell/src/hunspell/hunzip.hxx',
        'third_party/hunspell/src/hunspell/langnum.hxx',
        'third_party/hunspell/src/hunspell/phonet.cxx',
        'third_party/hunspell/src/hunspell/phonet.hxx',
        'third_party/hunspell/src/hunspell/replist.cxx',
        'third_party/hunspell/src/hunspell/replist.hxx',
        'third_party/hunspell/src/hunspell/suggestmgr.cxx',
        'third_party/hunspell/src/hunspell/suggestmgr.hxx',
        'third_party/hunspell/src/hunspell/utf_info.hxx',
        'third_party/hunspell/src/hunspell/w_char.hxx',
        'third_party/hunspell/src/parsers/textparser.cxx',
        'third_party/hunspell/src/parsers/textparser.hxx',
      ],
      'direct_dependent_settings': {
        'defines': [
          'HUNSPELL_OPERA_CLIENT',
          'HUNSPELL_STATIC',
          'USE_HUNSPELL',
        ],
      },
      # TODO(jschuh): http://crbug.com/167187 size_t -> int
      'msvs_disabled_warnings': [ 4267 ],
      'conditions': [
        ['os_posix == 1 and OS != "mac"', {
          'cflags': [
            '-Wno-unused-value',
            '-Wno-unused-variable',
            '-Wno-write-strings',
          ],
        }],
        ['clang == 1', {
          'xcode_settings': {
            'WARNING_CFLAGS': [
              # affentry.cxx has one `while ((p = nextchar(p)));` parsing loop.
              '-Wno-empty-body',
              # affentry.hxx has NULL as default parameter for a FLAG in two
              # places.
              '-Wno-null-conversion',
            ],
          },
          'cflags': [
            '-Wno-empty-body',
            '-Wno-null-conversion',
          ],
        }],
      ],
    },
  ],
}
