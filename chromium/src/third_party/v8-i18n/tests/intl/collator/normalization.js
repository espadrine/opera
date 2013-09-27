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

// Make sure normalization is always on, and normalization flag is ignored.

// We need a character with two combining marks, from two different classes,
// to make ICU fail comparison without normalization (upper, lower accent).
// We will just switch order of combining characters to try to induce failure.

// FYI, this one wouldn't work, since both accents are from the same class:
// http://unicode.org/cldr/utility/character.jsp?a=01DF

// See http://demo.icu-project.org/icu-bin/nbrowser?t=&s=1E09&uv=0 and
// http://unicode.org/cldr/utility/character.jsp?a=1E09 for character details.
var toCompare = ['\u0063\u0327\u0301', '\u0063\u0301\u0327'];

// Try with normalization off (as an option).
var collator = Intl.Collator([], {normalization: false});
// If we accepted normalization parameter, this would have failed.
assertEquals(0, collator.compare(toCompare[0], toCompare[1]));
assertFalse(collator.resolvedOptions().hasOwnProperty('normalization'));

// Try with normalization off (as Unicode extension).
collator = Intl.Collator(['de-u-kk-false']);
// If we accepted normalization parameter, this would have failed.
assertEquals(0, collator.compare(toCompare[0], toCompare[1]));
assertFalse(collator.resolvedOptions().hasOwnProperty('normalization'));

// Normalization is on by default.
collator = Intl.Collator();
assertEquals(0, collator.compare(toCompare[0], toCompare[1]));
assertFalse(collator.resolvedOptions().hasOwnProperty('normalization'));
