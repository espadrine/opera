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

// Performance test for overriden methods. Makes sure that default case
// is faster (cached) than the general case.

// Default, cached.
var startTime = new Date();
for (var i = 0; i < 1000; i++) {
  'a'.localeCompare('c');
}
var endTime = new Date();
var cachedTime = endTime.getTime() - startTime.getTime();

// Not cached.
startTime = new Date();
for (var i = 0; i < 1000; i++) {
  'a'.localeCompare('c', 'sr');
}
endTime = new Date();
var nonCachedTime = endTime.getTime() - startTime.getTime();

// Using collator. Faster than default, but not by much.
var collator = Intl.Collator();
startTime = new Date();
for (var i = 0; i < 1000; i++) {
  collator.compare('a', 'c');
}
endTime = new Date();
collatorTime = endTime.getTime() - startTime.getTime();

// Difference is within 20%.
assertTrue(collatorTime < cachedTime);
// Non-cached time is much slower, measured to 12.5 times.
assertTrue(cachedTime < nonCachedTime);

