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

// Tests time zone support.

var df = Intl.DateTimeFormat();
assertEquals(getDefaultTimeZone(), df.resolvedOptions().timeZone);

df = Intl.DateTimeFormat(undefined, {timeZone: 'UtC'});
assertEquals('UTC', df.resolvedOptions().timeZone);

df = Intl.DateTimeFormat(undefined, {timeZone: 'gmt'});
assertEquals('UTC', df.resolvedOptions().timeZone);

df = Intl.DateTimeFormat(undefined, {timeZone: 'America/Los_Angeles'});
assertEquals('America/Los_Angeles', df.resolvedOptions().timeZone);

df = Intl.DateTimeFormat(undefined, {timeZone: 'Europe/Belgrade'});
assertEquals('Europe/Belgrade', df.resolvedOptions().timeZone);

// Check Etc/XXX variants. They should work too.
df = Intl.DateTimeFormat(undefined, {timeZone: 'Etc/UTC'});
assertEquals('UTC', df.resolvedOptions().timeZone);

df = Intl.DateTimeFormat(undefined, {timeZone: 'Etc/GMT'});
assertEquals('UTC', df.resolvedOptions().timeZone);

df = Intl.DateTimeFormat(undefined, {timeZone: 'euRope/beLGRade'});
assertEquals('Europe/Belgrade', df.resolvedOptions().timeZone);

// : + - are not allowed, only / _ are.
assertThrows('Intl.DateTimeFormat(undefined, {timeZone: \'GMT+07:00\'})');
assertThrows('Intl.DateTimeFormat(undefined, {timeZone: \'GMT+0700\'})');
assertThrows('Intl.DateTimeFormat(undefined, {timeZone: \'GMT-05:00\'})');
assertThrows('Intl.DateTimeFormat(undefined, {timeZone: \'GMT-0500\'})');
assertThrows('Intl.DateTimeFormat(undefined, {timeZone: \'Etc/GMT+0\'})');
assertThrows('Intl.DateTimeFormat(undefined, ' +
    '{timeZone: \'America/Los-Angeles\'})');

// Throws for unsupported time zones.
assertThrows('Intl.DateTimeFormat(undefined, {timeZone: \'Aurope/Belgrade\'})');
