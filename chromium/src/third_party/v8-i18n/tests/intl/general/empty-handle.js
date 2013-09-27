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

// Minimal test case for b/161999.
// We have to check if ObjectTemplate::NewInstance returned empty handle, which
// can happen if there was a stack overflow.
// This test can take some time to fail.

var didThrowRangeError = false;
try {
  var X = '})()';
  function C() { X[C("asd".localeCompare("asdf") < 0)] = C("a"); }
  var b = C(C(new Date(Number.b, "").getTime()),
            function() {
              if (!X.C()) {
              }
            }[0].b++);
} catch (e) {
  if (e instanceof RangeError) {
    didThrowRangeError = true;
  }
}

assertTrue(didThrowRangeError);
