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
    'v8_path%': '../../..',
  },
  'targets': [
    {
      'target_name': 'test-runner',
      'type': 'executable',
      'sources': [
        '../tests/test-runner.cc',
      ],
      'include_dirs': [
        '..',
        '<(v8_path)',
      ],
      'dependencies': [
        'all.gyp:v8-i18n',
        '<(icu_path)/icu.gyp:*',
        '<(v8_path)/v8/tools/gyp/v8.gyp:v8',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '../..',
        ],
      },
    },
  ],  # targets
}
