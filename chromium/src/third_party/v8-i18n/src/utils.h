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

#ifndef V8_I18N_SRC_UTILS_H_
#define V8_I18N_SRC_UTILS_H_

#include "unicode/uversion.h"
#include "v8/include/v8.h"

namespace U_ICU_NAMESPACE {
class UnicodeString;
}

namespace v8_i18n {

class Utils {
 public:
  // Safe string copy. Null terminates the destination. Copies at most
  // (length - 1) bytes.
  // We can't use snprintf since it's not supported on all relevant platforms.
  // We can't use OS::SNPrintF, it's only for internal code.
  static void StrNCopy(char* dest, int length, const char* src);

  // Converts v8::String into UnicodeString. Returns false if input
  // can't be converted into utf8.
  static bool V8StringToUnicodeString(const v8::Handle<v8::Value>& input,
				      icu::UnicodeString* output);

  // Extract a String setting named in |settings| and set it to |result|.
  // Return true if it's specified. Otherwise, return false.
  static bool ExtractStringSetting(const v8::Handle<v8::Object>& settings,
                                   const char* setting,
                                   icu::UnicodeString* result);

  // Extract a Integer setting named in |settings| and set it to |result|.
  // Return true if it's specified. Otherwise, return false.
  static bool ExtractIntegerSetting(const v8::Handle<v8::Object>& settings,
                                    const char* setting,
                                    int32_t* result);

  // Extract a Boolean setting named in |settings| and set it to |result|.
  // Return true if it's specified. Otherwise, return false.
  static bool ExtractBooleanSetting(const v8::Handle<v8::Object>& settings,
                                    const char* setting,
                                    bool* result);

  // Converts ASCII array into UChar array.
  // Target is always \0 terminated.
  static void AsciiToUChar(const char* source,
                           int32_t source_length,
                           UChar* target,
                           int32_t target_length);

  // Creates an ObjectTemplate with one internal field.
  static v8::Local<v8::ObjectTemplate> GetTemplate(v8::Isolate* isolate);

  // Creates an ObjectTemplate with two internal fields.
  static v8::Local<v8::ObjectTemplate> GetTemplate2(v8::Isolate* isolate);

 private:
  Utils() {}
};

}  // namespace v8_i18n

#endif  // V8_I18N_SRC_UTILS_H_
