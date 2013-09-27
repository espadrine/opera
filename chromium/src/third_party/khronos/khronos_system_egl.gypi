# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# This file is meant to be included into the khronos_headers target in
# khronos.gyp. It arranges so that the EGL folder is picked up from the
# system and GLES2 and KHR from third_party/khronos/ is used. This to
# be able to use the platform specific EGLNative* types and not the ones
# specified in third_party/khronos/EGL/eglplatform.h.

# The khronos_headers_location variable should point out the folder
# where EGL/ resides on the system (absolute path).

{
  'variables': {
    'khronos_headers_location%': '',
  },
  'conditions': [
    ['khronos_headers_location == ""', {
        # Use the default include_dirs from the khronos_headers target if
        # no attempt of overriding it was made.
        'all_dependent_settings': {
          'include_dirs': [
            '.',
            '../../gpu',  # Contains GLES2/gl2chromium.h
          ],
        }
    }, {
        # Make a folder with symlinks to the different EGL/GLES2/KHR folders we want to use.
        'variables': {
          'khronos_headers_symlink_target': '<(SHARED_INTERMEDIATE_DIR)/khronos_symlinks',
        },
        'all_dependent_settings': {
          'include_dirs': [
            '<(khronos_headers_symlink_target)',
            '../../gpu',  # Contains GLES2/gl2chromium.h
          ],
        },
        'actions': [
          {
            'action_name': 'create_khronos_symlinks_egl',
            'variables': {
              'dummy_file':  '<(khronos_headers_symlink_target)/.dummy_egl',
              'khronos_headers_symlink_source': '<(khronos_headers_location)',
              # Forces a gyp failure if the file does not exist
              'dummy_check': '<!(ls <(khronos_headers_location)/EGL/eglplatform.h)',
            },
            'inputs': [
              'khronos.gyp',
              'khronos_system_egl.gypi',
            ],
            'outputs': [
              '<(dummy_file)',
            ],
            'action': [
              '../../build/symlink.py',
              '--force',
              '--touch', '<(dummy_file)',
              '<(khronos_headers_symlink_source)/EGL',
              '<(khronos_headers_symlink_target)',
            ],
            'message': 'Creating EGL headers symlink.',
          },
          {
            'action_name': 'create_khronos_symlinks_gles2',
            'variables': {
              'dummy_file':  '<(khronos_headers_symlink_target)/.dummy_gles2',
              # Need to use the absolute path
              'khronos_headers_symlink_source': '<!(cd <(DEPTH) && pwd -P)/third_party/khronos',
            },
            'inputs': [
              'khronos.gyp',
              'khronos_system_egl.gypi',
            ],
            'outputs': [
              '<(dummy_file)',
            ],
            'action': [
              '../../build/symlink.py',
              '--force',
              '--touch', '<(dummy_file)',
              '<(khronos_headers_symlink_source)/GLES2',
              '<(khronos_headers_symlink_target)',
            ],
            'message': 'Creating GLES2 headers symlink.',
          },
          {
            'action_name': 'create_khronos_symlinks_khr',
            'variables': {
              'dummy_file':  '<(khronos_headers_symlink_target)/.dummy_khr',
              # Need to use the absolute path
              'khronos_headers_symlink_source': '<!(cd <(DEPTH) && pwd -P)/third_party/khronos',
            },
            'inputs': [
              'khronos.gyp',
              'khronos_system_egl.gypi',
            ],
            'outputs': [
              '<(dummy_file)',
            ],
            'action': [
              '../../build/symlink.py',
              '--force',
              '--touch', '<(dummy_file)',
              '<(khronos_headers_symlink_source)/KHR',
              '<(khronos_headers_symlink_target)',
            ],
            'message': 'Creating KHR headers symlink.',
          },
        ],
    }],
  ]
}
