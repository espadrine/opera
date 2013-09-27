# -*- Mode: c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
#
# Copyright (C) 2013 Opera Software ASA.  All rights reserved.
#
# This file is an original work developed by Opera Software ASA

{
  'variables': {
    'common_resources_prefix': 'opera',
    'disable_nacl': 1,
    'opera_extensions': 1,
    'search_in_error_page': 1,
    'use_system_python_in_tests': 1,
    'web_ui_scheme': 'opera',
    # Disabled features that we might want to port and enable later.
    'configuration_policy': 0,
    'enable_app_list': 0,
    'enable_google_now': 0,
    'enable_managed_users': 0,
    'enable_one_click_signin': 0,
    'enable_plugin_installation': 0,
    'enable_task_manager': 0,
    'notifications': 0,
    'disable_input_type_color': 1,
  },
  'target_defaults': {
    'configurations': {
      'Release_Base': {
        'variables': {
          'buildtype': 'Official',
        },
      },
    },
    'conditions': [
      ['search_in_error_page==1', {
        'defines': ['ENABLE_SEARCH_IN_ERROR_PAGE'],
      }],
    ],
  },
  'conditions': [
    ['search_in_error_page==1', {
      'variables': {
        'grit_defines': ['-D', 'search_in_error_page'],
      },
    }],
  ],
}
