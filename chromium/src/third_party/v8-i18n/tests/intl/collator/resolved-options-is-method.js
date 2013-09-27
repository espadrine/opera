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

// Test that resolvedOptions is a method, not a property getter and that
// the result is mutable.

var collator = new Intl.Collator();

var result = collator.resolvedOptions();

assertTrue(result instanceof Object);

// Result should be mutable.
result.locale = 'xx';

assertEquals(result.locale, 'xx');
