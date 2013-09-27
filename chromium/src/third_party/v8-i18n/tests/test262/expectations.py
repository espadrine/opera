#!/usr/bin/python
#
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

# Bugs
expect_fail = {
  # ISO and CLDR data mismatch. http://unicode.org/cldr/trac/ticket/5302
  'data/test/suite/intl402/ch11/11.1/11.1.1_20_c.js': 'FAIL',
  # ICU bug http://bugs.icu-project.org/trac/ticket/9547
  'data/test/suite/intl402/ch11/11.3/11.3.2_TRP.js': 'FAIL',
  # ICU bug http://bugs.icu-project.org/trac/ticket/9265
  'data/test/suite/intl402/ch09/9.2/9.2.5_11_g_ii_2.js': 'FAIL',
  # ICU bug http://bugs.icu-project.org/trac/ticket/9562
  'data/test/suite/intl402/ch06/6.2/6.2.3.js': 'FAIL',
  # V8 issue with prototype overrides
  'data/test/suite/intl402/ch10/10.1/10.1.2.1_4.js': 'FAIL',
  'data/test/suite/intl402/ch10/10.2/10.2.1.js': 'FAIL',
  'data/test/suite/intl402/ch10/10.3/10.3_a.js': 'FAIL',
  'data/test/suite/intl402/ch11/11.1/11.1.2.1_4.js': 'FAIL',
  'data/test/suite/intl402/ch11/11.2/11.2.1.js': 'FAIL',
  'data/test/suite/intl402/ch11/11.3/11.3_a.js': 'FAIL',
  'data/test/suite/intl402/ch12/12.1/12.1.2.1_4.js': 'FAIL',
  'data/test/suite/intl402/ch12/12.2/12.2.1.js': 'FAIL',
  'data/test/suite/intl402/ch12/12.3/12.3_a.js': 'FAIL',
  # Will use V8 internals once we switch.
  'data/test/suite/intl402/ch09/9.2/9.2.1_2.js': 'FAIL',
  'data/test/suite/intl402/ch09/9.2/9.2.6_2.js': 'FAIL',
  # Should use methods from v8/src/regexp.js to maintain clean global state.
  # We could remove regex clean in the footer.js after this.
  'data/test/suite/intl402/ch10/10.1/10.1.1_a.js': 'FAIL',
  'data/test/suite/intl402/ch11/11.1/11.1.1_a.js': 'FAIL',
  'data/test/suite/intl402/ch12/12.1/12.1.1_a.js': 'FAIL',
  # timeZone is allowed to be != undefined by default. Should check test.
  'data/test/suite/intl402/ch12/12.3/12.3.3.js': 'FAIL',
}

# Deliberate incompatibilities
incompatible = {
}
