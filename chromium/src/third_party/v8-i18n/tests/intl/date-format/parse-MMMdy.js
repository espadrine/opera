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

// Testing v8Parse method for date and time pattern.
// Month is represented as a short name.

var dtf = new Intl.DateTimeFormat(['en'],
				  {year: 'numeric', month: 'short',
				   day: 'numeric'});

// Make sure we have pattern we expect (may change in the future).
assertEquals('MMM d, y', dtf.resolved.pattern);

assertEquals('Sat May 04 1974 00:00:00 GMT-0007 (PDT)',
	     usePDT(String(dtf.v8Parse('May 4, 1974'))));

// Missing , in the pattern.
assertEquals(undefined, dtf.v8Parse('May 4 1974'));

// Extra "th" after 4 in the pattern.
assertEquals(undefined, dtf.v8Parse('May 4th, 1974'));

// Wrong pattern.
assertEquals(undefined, dtf.v8Parse('5/4/1974'));
