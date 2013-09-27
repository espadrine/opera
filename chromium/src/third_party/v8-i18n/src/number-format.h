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

#ifndef V8_I18N_SRC_NUMBER_FORMAT_H_
#define V8_I18N_SRC_NUMBER_FORMAT_H_

#include "unicode/uversion.h"
#include "v8/include/v8.h"

namespace U_ICU_NAMESPACE {
class DecimalFormat;
}

namespace v8_i18n {

class NumberFormat {
 public:
  static void JSCreateNumberFormat(
      const v8::FunctionCallbackInfo<v8::Value>& args);

  // Helper methods for various bindings.

  // Unpacks date format object from corresponding JavaScript object.
  static icu::DecimalFormat* UnpackNumberFormat(v8::Handle<v8::Object> obj);

  // Release memory we allocated for the NumberFormat once the JS object that
  // holds the pointer gets garbage collected.
  static void DeleteNumberFormat(v8::Isolate* isolate,
                                 v8::Persistent<v8::Object>* object,
                                 void* param);

  // Formats number and returns corresponding string.
  static void JSInternalFormat(const v8::FunctionCallbackInfo<v8::Value>& args);

  // Parses a string and returns a number.
  static void JSInternalParse(const v8::FunctionCallbackInfo<v8::Value>& args);

 private:
  NumberFormat();
};

}  // namespace v8_i18n

#endif  // V8_I18N_SRC_NUMBER_FORMAT_H_
