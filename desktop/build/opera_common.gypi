# -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
#
# Copyright (C) 2012 Opera Software ASA.  All rights reserved.
#
# This file is an original work developed by Opera Software ASA
#
# Common variables shared by all opera .gyp files, this file will be included by
# various of gyp files under different directories, so DO NOT use any relative
# path here. If a path variable is needed, use <(DEPTH), <(SHARED_INTERMEDIATE_DIR)
# or their derivates.
{
  'variables': {
    'allocator_target': '<(DEPTH)/base/allocator/allocator.gyp:allocator',
    'chromium_code': 1,  # Use higher warning level.
    'grit_out_dir': '<(SHARED_INTERMEDIATE_DIR)/opera',
    'opera_product_name': 'Opera',
    'opera_locales': [
      'af', 'ar', 'az', 'be', 'bg', 'bn', 'ca', 'cs', 'da', 'de', 'el', 'en-GB',
      'en-US', 'es', 'es-419', 'fi', 'fr', 'fr-CA', 'fy', 'gd', 'hi', 'hr',
      'hu', 'id', 'it', 'ja', 'kk', 'ko', 'lt', 'lv', 'me', 'mk', 'ms', 'nb',
      'nl', 'nn', 'pa', 'pl', 'pt-BR', 'pt-PT', 'ro', 'ru', 'sk', 'sr', 'sv',
      'sw', 'ta', 'te', 'th', 'tl', 'tr', 'uk', 'vi', 'zh-CN', 'zh-TW', 'zu'
    ],
    'webkit_src_dir': '<(DEPTH)/third_party/WebKit',
    'opera_source_release_lib': 'libopera<(STATIC_LIB_SUFFIX)',
    'opera_mapfiles_linker_flag': '-Xlinker -map -Xlinker',
    'generate_link_maps%': 0,
    'conditions': [
      # Link maps is for crash analysis, we don't need to generate link maps if:
      # - fastbuild==1: it won't contain any debug information.
      # - component=="shared_library": these are developer internal builds.
      ['OS=="mac" and fastbuild==0 and component=="static_library"', {
        'generate_link_maps%': 1,
      }],
    ],
  },
}

