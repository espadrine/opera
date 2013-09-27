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

// Tests supportedLocalesOf method.

var undef = Intl.DateTimeFormat.supportedLocalesOf();
assertEquals(undefined, undef[0]);

var empty = Intl.DateTimeFormat.supportedLocalesOf([]);
assertEquals(undefined, empty[0]);

var strLocale = Intl.DateTimeFormat.supportedLocalesOf('sr');
assertEquals('sr', strLocale[0]);

var multiLocale =
    Intl.DateTimeFormat.supportedLocalesOf(['sr-Thai-RS', 'de', 'zh-CN']);
assertEquals('sr-Thai-RS', multiLocale[0]);
assertEquals('de', multiLocale[1]);
assertEquals('zh-CN', multiLocale[2]);
