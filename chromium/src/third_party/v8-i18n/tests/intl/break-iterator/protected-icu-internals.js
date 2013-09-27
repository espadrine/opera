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

// Internal object we got from native code should not be writable,
// configurable or enumerable. One can still change its public properties, but
// we don't use them to do actual work.

var iterator = new Intl.v8BreakIterator([]);

// Direct write should fail.
iterator.iterator = {'zzz':'some random object'};

assertFalse(iterator.iterator.hasOwnProperty('zzz'));

// Try redefining the property.
var didThrow = false;
try {
  Object.defineProperty(iterator, 'iterator', {value: undefined}); 
} catch(e) {
  didThrow = true;
}
assertTrue(didThrow);

// Try deleting the property.
assertFalse(delete iterator.iterator);
