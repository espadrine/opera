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

#ifndef V8_I18N_SRC_COLLATOR_H
#define V8_I18N_SRC_COLLATOR_H

#include "unicode/uversion.h"
#include "v8/include/v8.h"

namespace U_ICU_NAMESPACE {
class Collator;
class UnicodeString;
}

namespace v8_i18n {

class Collator {
 public:
  static void JSCreateCollator(const v8::FunctionCallbackInfo<v8::Value>& args);

  // Helper methods for various bindings.

  // Unpacks collator object from corresponding JavaScript object.
  static icu::Collator* UnpackCollator(v8::Handle<v8::Object> obj);

  // Release memory we allocated for the Collator once the JS object that
  // holds the pointer gets garbage collected.
  static void DeleteCollator(v8::Isolate* isolate,
                             v8::Persistent<v8::Object>* object,
                             void* param);

  // Compare two strings and returns -1, 0 and 1 depending on
  // whether string1 is smaller than, equal to or larger than string2.
  static void JSInternalCompare(
      const v8::FunctionCallbackInfo<v8::Value>& args);

 private:
  Collator() {}
};

}  // namespace v8_i18n

#endif  // V8_I18N_SRC_COLLATOR
