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

// Test that we always use original Intl.Constructors for toLocaleString calls.

function throwError() {
  throw new Error('Malicious method invoked.');
}

Intl.Collator = Intl.NumberFormat = Intl.DateTimeFormat = throwError;

Intl.Collator.prototype.compare = throwError;
Intl.NumberFormat.prototype.format = throwError;
Intl.DateTimeFormat.prototype.format = throwError;

// Make sure constructors actually throw now.
assertThrows('new Intl.Collator()');
assertThrows('new Intl.NumberFormat()');
assertThrows('new Intl.DateTimeFormat()');

// None of these should throw.
assertDoesNotThrow('new Date().toLocaleString()');
assertDoesNotThrow('new Date().toLocaleDateString()');
assertDoesNotThrow('new Date().toLocaleTimeString()');
assertDoesNotThrow('new Number(12345.412).toLocaleString()');
assertDoesNotThrow('new String(\'abc\').localeCompare(\'bcd\')');
