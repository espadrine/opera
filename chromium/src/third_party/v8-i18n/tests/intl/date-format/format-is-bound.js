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

// Create default DateTimeFormat.
var dtf = new Intl.DateTimeFormat();

// Array we want to iterate, actual dates are not important.
var dateArray = [Date.now(), Date.now(), Date.now()];

// It shouldn't throw.
// The format() method should be properly bound to the dtf object.
dateArray.forEach(dtf.format);

// Formatting a date should work in a direct call.
dtf.format();
