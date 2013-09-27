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

// Sort plain German text using defaults.

var strings = ['März', 'Fuße', 'FUSSE', 'Fluße', 'Flusse',
	       'flusse', 'fluße', 'flüße', 'flüsse'];

var collator = Intl.Collator(['de']);
var result = strings.sort(collator.compare);

assertEquals('flusse', result[0]);
assertEquals('Flusse', result[1]);
assertEquals('fluße', result[2]);
assertEquals('Fluße', result[3]);
assertEquals('flüsse', result[4]);
assertEquals('flüße', result[5]);
assertEquals('FUSSE', result[6]);
assertEquals('Fuße', result[7]);
assertEquals('März', result[8]);
