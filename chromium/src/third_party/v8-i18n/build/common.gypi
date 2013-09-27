# Copyright 2012 the v8-i18n authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

{
  'variables': {
    'strict_aliasing%': 'false',
    # V8's visibility setting
    'visibility%': 'hidden',
    'target_arch%': 'x64',
    'host_arch%': 'x64',
    # V8 should not build target and host
    'want_separate_host_toolset': 0,
    # Allow override to 'shared_library' for DLL/.so builds
    'component%': 'static_library',

    'conditions': [
      # A flag for POSIX platforms
      ['OS=="win"', {
        'os_posix%': 0,
      }, {
        'os_posix%': 1,
      }],
    ],
  },
}
