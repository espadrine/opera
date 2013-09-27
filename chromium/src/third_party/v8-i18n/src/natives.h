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

#ifndef V8_I18N_SRC_NATIVES_H_
#define V8_I18N_SRC_NATIVES_H_

namespace v8_i18n {

class Natives {
 public:
  // Gets script source from generated file.
  // Source is statically allocated string.
  static const char* GetScriptSource();
};

}  // namespace v8_i18n

#endif  // V8_I18N_SRC_NATIVES_H_
