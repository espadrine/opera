// Copyright 2012 the v8-i18n authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// ECMAScript 402 API implementation is broken into separate files for
// each service. The build system combines them together into one
// Intl namespace.

// Fix RegExp global state so we don't fail WebKit layout test:
// fast/js/regexp-caching.html
// It seems that 'g' or test() operations leave state changed.
var CLEANUP_RE = new RegExp('');
CLEANUP_RE.test('');

return Intl;
}());

// Alias v8Intl to Intl so we don't break existing applications.
// TODO(cira): Remove in a couple of months (starting at Oct 1st 2012).
var v8Intl = Intl;
