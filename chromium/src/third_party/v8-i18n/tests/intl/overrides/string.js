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

// Tests String.prototype.localeCompare method override.

var testData = {
  'en': ['blood', 'bull', 'ascend', 'zed', 'down'],
  'sr': ['новине', 'ограда', 'жирафа', 'Никола', 'Андрија', 'Стара Планина',
         'џак', 'алав', 'ћук', 'чука'],
  'de': ['März', 'Fuße', 'FUSSE', 'Fluße', 'Flusse', 'flusse', 'fluße',
         'flüße', 'flüsse']
};


function testArrays(locale) {
  var data;
  if (locale === undefined) {
    data = testData['en'];
    locale = [];
  } else {
    data = testData[locale];
  }

  var collator = new Intl.Collator(locale, options);
  var collatorResult = data.sort(collator.compare);
  var localeCompareResult = data.sort(function(a, b) {
    return a.localeCompare(b, locale, options)
  });
  assertEquals(collatorResult, localeCompareResult);
}


// Defaults
var options = undefined;
testArrays();


// Specify locale, keep default options.
options = undefined;
Object.keys(testData).forEach(testArrays);


// Specify locale and options.
options = {caseFirst: 'upper'};
Object.keys(testData).forEach(testArrays);
