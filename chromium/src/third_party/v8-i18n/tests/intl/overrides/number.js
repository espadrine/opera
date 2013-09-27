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

// Tests Number.prototype.toLocaleString method override.

var integer = 123456790;
var float = 1234567890.123434;


// Defaults
var nf = new Intl.NumberFormat();
assertEquals(nf.format(integer), integer.toLocaleString());
assertEquals(nf.format(float), float.toLocaleString());


// Specify locale, default options for toLocaleString method.
var locale = ['sr'];
nf = new Intl.NumberFormat(locale);
assertEquals(nf.format(integer), integer.toLocaleString(locale));
assertEquals(nf.format(float), float.toLocaleString(locale));


// Specify locale and options for toLocaleString method.
locale = ['ko'];
var options = {minimumIntegerDigits: 8, useGroupingSeparator: true,
               minimumFractionalDigits: 1, maximumFractionalDigits: 2};
nf = new Intl.NumberFormat(locale, options);
assertEquals(nf.format(integer), integer.toLocaleString(locale, options));
assertEquals(nf.format(float), float.toLocaleString(locale, options));
