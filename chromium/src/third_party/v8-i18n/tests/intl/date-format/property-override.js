// Copyright 2012 the v8-i18n authors.
//
// Licensed under the Apache License, Version 2.0 (the 'License');
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an 'AS IS' BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Checks for security holes introduced by Object.property overrides.
// For example:
// Object.defineProperty(Array.prototype, 'locale', {
//   set: function(value) {
//     throw new Error('blah');
//   },
//   configurable: true,
//   enumerable: false
// });
//
// would throw in case of (JS) x.locale = 'us' or (C++) x->Set('locale', 'us').
//
// Update both date-format.js and date-format.cc so they have the same list of
// properties.

// First get supported properties.
// Some of the properties are optional, so we request them.
var properties = [];
var options = Intl.DateTimeFormat(
  'en-US', {weekday: 'short', era: 'short', year: 'numeric', month: 'short',
            day: 'numeric', hour: 'numeric', minute: 'numeric',
            second: 'numeric', timeZoneName: 'short'}).resolvedOptions();
for (var prop in options) {
  if (options.hasOwnProperty(prop)) {
    properties.push(prop);
  }
}

var expectedProperties = [
  'calendar', 'day', 'era', 'hour12', 'hour', 'locale',
  'minute', 'month', 'numberingSystem',
  'second', 'timeZone', 'timeZoneName', 'weekday', 'year'
];

assertEquals(expectedProperties.length, properties.length);

properties.forEach(function(prop) {
  assertFalse(expectedProperties.indexOf(prop) === -1);
});

taintProperties(properties);

var locale = Intl.DateTimeFormat().resolvedOptions().locale;
