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

// Tests Date.prototype.toLocaleXXXString method overrides.

var date = new Date();


// Defaults for toLocaleXXXString
var dtfDate = new Intl.DateTimeFormat();
var dtfTime = new Intl.DateTimeFormat(
  [], {hour: 'numeric', minute: 'numeric', second: 'numeric'});
var dtfAll = new Intl.DateTimeFormat(
  [], {year: 'numeric', month: 'numeric', day: 'numeric',
       hour: 'numeric', minute: 'numeric', second: 'numeric'});
assertEquals(dtfAll.format(date), date.toLocaleString());
assertEquals(dtfDate.format(date), date.toLocaleDateString());
assertEquals(dtfTime.format(date), date.toLocaleTimeString());


// Specify locale, default options for toLocaleXXXString
var locale = ['sr'];
dtfDate = new Intl.DateTimeFormat(locale);
dtfTime = new Intl.DateTimeFormat(
  locale, {hour: 'numeric', minute: 'numeric', second: 'numeric'});
dtfAll = new Intl.DateTimeFormat(
  locale, {year: 'numeric', month: 'numeric', day: 'numeric',
           hour: 'numeric', minute: 'numeric', second: 'numeric'});
assertEquals(dtfAll.format(date), date.toLocaleString(locale));
assertEquals(dtfDate.format(date), date.toLocaleDateString(locale));
assertEquals(dtfTime.format(date), date.toLocaleTimeString(locale));


// Specify locale and options for toLocaleXXXString
locale = ['ko'];
var options = {year: 'numeric', month: 'long', day: 'numeric',
               hour: 'numeric', minute: '2-digit', second: 'numeric'};
var dtf = new Intl.DateTimeFormat(locale, options);
assertEquals(dtf.format(date), date.toLocaleString(locale, options));
assertEquals(dtf.format(date), date.toLocaleDateString(locale, options));
assertEquals(dtf.format(date), date.toLocaleTimeString(locale, options));
