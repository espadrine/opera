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

var nf = new Intl.NumberFormat(['en'], {style: 'percent'});

assertEquals(1.2343, nf.v8Parse('123.43%'));
assertEquals(1.23, nf.v8Parse('123%'));
assertEquals(NaN, nf.v8Parse(NaN));
assertEquals(123.23, nf.v8Parse('123,23%'));
assertEquals(123.23456, nf.v8Parse('123,23.456%'));
assertEquals(123.23456, nf.v8Parse('0000000123,23.456%'));
assertEquals(-123.23456, nf.v8Parse('-123,23.456%'));
