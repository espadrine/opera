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

// Test if resolvedOptions() returns expected fields/values.

// Default (year, month, day) formatter.
var dtfDefault = Intl.DateTimeFormat('en-US');
var resolved = dtfDefault.resolvedOptions();

assertTrue(resolved.hasOwnProperty('locale'));
assertEquals('en-US', resolved.locale);
assertTrue(resolved.hasOwnProperty('numberingSystem'));
assertEquals('latn', resolved.numberingSystem);
assertTrue(resolved.hasOwnProperty('calendar'));
assertEquals('gregory', resolved.calendar);
assertTrue(resolved.hasOwnProperty('timeZone'));
assertEquals(getDefaultTimeZone(), resolved.timeZone);
// These are in by default.
assertTrue(resolved.hasOwnProperty('year'));
assertEquals('numeric', resolved.year);
assertTrue(resolved.hasOwnProperty('month'));
assertEquals('numeric', resolved.month);
assertTrue(resolved.hasOwnProperty('day'));
assertEquals('numeric', resolved.day);
// These shouldn't be in by default.
assertFalse(resolved.hasOwnProperty('era'));
assertFalse(resolved.hasOwnProperty('timeZoneName'));
assertFalse(resolved.hasOwnProperty('weekday'));
assertFalse(resolved.hasOwnProperty('hour12'));
assertFalse(resolved.hasOwnProperty('hour'));
assertFalse(resolved.hasOwnProperty('minute'));
assertFalse(resolved.hasOwnProperty('second'));

// Time formatter.
var dtfTime = Intl.DateTimeFormat(
  'sr-RS', {hour: 'numeric', minute: 'numeric', second: 'numeric'});
resolved = dtfTime.resolvedOptions();

assertTrue(resolved.hasOwnProperty('locale'));
assertTrue(resolved.hasOwnProperty('numberingSystem'));
assertTrue(resolved.hasOwnProperty('calendar'));
assertTrue(resolved.hasOwnProperty('timeZone'));
assertTrue(resolved.hasOwnProperty('hour12'));
assertEquals(false, resolved.hour12);
assertTrue(resolved.hasOwnProperty('hour'));
assertEquals('2-digit', resolved.hour);
assertTrue(resolved.hasOwnProperty('minute'));
assertEquals('2-digit', resolved.minute);
assertTrue(resolved.hasOwnProperty('second'));
assertEquals('2-digit', resolved.second);
// Didn't ask for them.
assertFalse(resolved.hasOwnProperty('year'));
assertFalse(resolved.hasOwnProperty('month'));
assertFalse(resolved.hasOwnProperty('day'));
assertFalse(resolved.hasOwnProperty('era'));
assertFalse(resolved.hasOwnProperty('timeZoneName'));
assertFalse(resolved.hasOwnProperty('weekday'));

// Full formatter.
var dtfFull = Intl.DateTimeFormat(
  'en-US', {weekday: 'short', era: 'short', year: 'numeric', month: 'short',
            day: 'numeric', hour: 'numeric', minute: 'numeric',
            second: 'numeric', timeZoneName: 'short', timeZone: 'UTC'});
resolved = dtfFull.resolvedOptions();

assertTrue(resolved.hasOwnProperty('locale'));
assertTrue(resolved.hasOwnProperty('numberingSystem'));
assertTrue(resolved.hasOwnProperty('calendar'));
assertTrue(resolved.hasOwnProperty('timeZone'));
assertTrue(resolved.hasOwnProperty('hour12'));
assertEquals(true, resolved.hour12);
assertTrue(resolved.hasOwnProperty('hour'));
assertTrue(resolved.hasOwnProperty('minute'));
assertTrue(resolved.hasOwnProperty('second'));
assertTrue(resolved.hasOwnProperty('year'));
assertTrue(resolved.hasOwnProperty('month'));
assertTrue(resolved.hasOwnProperty('day'));
assertTrue(resolved.hasOwnProperty('era'));
assertEquals('short', resolved.era);
assertTrue(resolved.hasOwnProperty('timeZoneName'));
assertEquals('short', resolved.timeZoneName);
assertTrue(resolved.hasOwnProperty('weekday'));
assertEquals('short', resolved.weekday);
