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

// Test formatting method with specified date, invalid input.

var dtf = new Intl.DateTimeFormat('en-US', {timeZone: 'UTC'});

var someDate = dtf.format(144313200000);
assertEquals('7/29/1974', someDate);

var invalidValues = [NaN, Infinity, -Infinity];
invalidValues.forEach(function(value) {
  var error;
  try {
    dtf.format(value);
  } catch (e) {
    error = e;
  }

  assertTrue(error !== undefined);
  assertEquals('RangeError', error.name);
});
