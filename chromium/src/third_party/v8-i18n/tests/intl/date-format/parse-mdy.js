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

// Testing v8Parse method for date only.

var dtf = new Intl.DateTimeFormat(['en']);

// Make sure we have pattern we expect (may change in the future).
assertEquals('M/d/y', dtf.resolved.pattern);

assertEquals('Sat May 04 1974 00:00:00 GMT-0007 (PDT)',
	     usePDT(String(dtf.v8Parse('5/4/74'))));
assertEquals('Sat May 04 1974 00:00:00 GMT-0007 (PDT)',
             usePDT(String(dtf.v8Parse('05/04/74'))));
assertEquals('Sat May 04 1974 00:00:00 GMT-0007 (PDT)',
             usePDT(String(dtf.v8Parse('5/04/74'))));
assertEquals('Sat May 04 1974 00:00:00 GMT-0007 (PDT)',
             usePDT(String(dtf.v8Parse('5/4/1974'))));

// Month is numeric, so it fails on "May".
assertEquals(undefined, dtf.v8Parse('May 4th 1974'));

// Time is ignored from the input, since the pattern doesn't have it.
assertEquals('Sat May 04 1974 00:00:00 GMT-0007 (PDT)',
             usePDT(String(dtf.v8Parse('5/4/74 12:30:12'))));
