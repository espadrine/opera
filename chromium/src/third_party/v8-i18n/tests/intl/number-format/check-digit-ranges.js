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

// Digit ranges are obeyed.

// Invalid ranges
assertThrows('Intl.NumberFormat(undefined, {minimumIntegerDigits: 0})');
assertThrows('Intl.NumberFormat(undefined, {minimumIntegerDigits: 22})');
assertThrows('Intl.NumberFormat(undefined, {minimumIntegerDigits: null})');
assertThrows('Intl.NumberFormat(undefined, {minimumIntegerDigits: Infinity})');
assertThrows('Intl.NumberFormat(undefined, {minimumIntegerDigits: -Infinity})');
assertThrows('Intl.NumberFormat(undefined, {minimumIntegerDigits: x})');

assertThrows('Intl.NumberFormat(undefined, {minimumFractionDigits: -1})');
assertThrows('Intl.NumberFormat(undefined, {maximumFractionDigits: 21})');

assertThrows('Intl.NumberFormat(undefined, {minimumSignificantDigits: 0})');
assertThrows('Intl.NumberFormat(undefined, {maximumSignificantDigits: 22})');
assertThrows('Intl.NumberFormat(undefined, ' +
    '{minimumSignificantDigits: 5, maximumSignificantDigits: 2})');

// Valid ranges
assertDoesNotThrow('Intl.NumberFormat(undefined, {minimumIntegerDigits: 1})');
assertDoesNotThrow('Intl.NumberFormat(undefined, {minimumIntegerDigits: 21})');

assertDoesNotThrow('Intl.NumberFormat(undefined, {minimumFractionDigits: 0})');
assertDoesNotThrow('Intl.NumberFormat(undefined, {minimumFractionDigits: 20})');

assertDoesNotThrow('Intl.NumberFormat(undefined, ' +
    '{minimumSignificantDigits: 1})');
assertDoesNotThrow('Intl.NumberFormat(undefined, ' +
    '{maximumSignificantDigits: 21})');
