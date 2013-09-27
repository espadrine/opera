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

#ifndef V8_I18N_SRC_LOCALE_H_
#define V8_I18N_SRC_LOCALE_H_

#include "unicode/uversion.h"
#include "v8/include/v8.h"

namespace v8_i18n {

// Canonicalizes the BCP47 language tag using BCP47 rules.
// Returns 'invalid-tag' in case input was not well formed.
void JSCanonicalizeLanguageTag(const v8::FunctionCallbackInfo<v8::Value>& args);

// Returns a list of available locales for collator, date or number formatter.
void JSAvailableLocalesOf(const v8::FunctionCallbackInfo<v8::Value>& args);

// Returns default ICU locale.
void JSGetDefaultICULocale(const v8::FunctionCallbackInfo<v8::Value>& args);

// Returns an array of objects, that have maximized and base names of inputs.
// Unicode extensions are dropped from both.
// Input: ['zh-TW-u-nu-thai', 'sr']
// Output: [{maximized: 'zh-Hant-TW', base: 'zh-TW'},
//          {maximized: 'sr-Cyrl-RS', base: 'sr'}]
void JSGetLanguageTagVariants(const v8::FunctionCallbackInfo<v8::Value>& args);

}  // namespace v8_i18n

#endif  // V8_I18N_SRC_LOCALE_H_
