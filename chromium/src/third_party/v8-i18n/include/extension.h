// Copyright 2011 the v8-i18n authors.
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

#ifndef V8_I18N_INCLUDE_EXTENSION_H_
#define V8_I18N_INCLUDE_EXTENSION_H_

#include "v8/include/v8.h"

namespace v8_i18n {

class Extension : public v8::Extension {
 public:
  Extension();

  virtual v8::Handle<v8::FunctionTemplate> GetNativeFunction(
      v8::Handle<v8::String> name);

  // V8 code prefers Register, while Chrome and WebKit use get kind of methods.
  static void Register();
  static Extension* get();

 private:
  static Extension* extension_;
};

}  // namespace v8_i18n

#endif  // V8_I18N_INCLUDE_EXTENSION_H_
