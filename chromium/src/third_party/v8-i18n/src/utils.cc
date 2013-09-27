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

#include "src/utils.h"

#include <string.h>

#include "unicode/unistr.h"

namespace v8_i18n {

// static
void Utils::StrNCopy(char* dest, int length, const char* src) {
  if (!dest || !src) return;

  strncpy(dest, src, length);
  dest[length - 1] = '\0';
}

// static
bool Utils::V8StringToUnicodeString(const v8::Handle<v8::Value>& input,
				    icu::UnicodeString* output) {
  v8::String::Utf8Value utf8_value(input);

  if (*utf8_value == NULL) return false;

  output->setTo(icu::UnicodeString::fromUTF8(*utf8_value));

  return true;
}

// static
bool Utils::ExtractStringSetting(const v8::Handle<v8::Object>& settings,
                                 const char* setting,
                                 icu::UnicodeString* result) {
  if (!setting || !result) return false;

  v8::HandleScope handle_scope;
  v8::TryCatch try_catch;
  v8::Handle<v8::Value> value = settings->Get(v8::String::New(setting));
  if (try_catch.HasCaught()) {
    return false;
  }
  // No need to check if |value| is empty because it's taken care of
  // by TryCatch above.
  if (!value->IsUndefined() && !value->IsNull() && value->IsString()) {
    return V8StringToUnicodeString(value, result);
  }
  return false;
}

// static
bool Utils::ExtractIntegerSetting(const v8::Handle<v8::Object>& settings,
                                  const char* setting,
                                  int32_t* result) {
  if (!setting || !result) return false;

  v8::HandleScope handle_scope;
  v8::TryCatch try_catch;
  v8::Handle<v8::Value> value = settings->Get(v8::String::New(setting));
  if (try_catch.HasCaught()) {
    return false;
  }
  // No need to check if |value| is empty because it's taken care of
  // by TryCatch above.
  if (!value->IsUndefined() && !value->IsNull() && value->IsNumber()) {
    *result = static_cast<int32_t>(value->Int32Value());
    return true;
  }
  return false;
}

// static
bool Utils::ExtractBooleanSetting(const v8::Handle<v8::Object>& settings,
                                  const char* setting,
                                  bool* result) {
  if (!setting || !result) return false;

  v8::HandleScope handle_scope;
  v8::TryCatch try_catch;
  v8::Handle<v8::Value> value = settings->Get(v8::String::New(setting));
  if (try_catch.HasCaught()) {
    return false;
  }
  // No need to check if |value| is empty because it's taken care of
  // by TryCatch above.
  if (!value->IsUndefined() && !value->IsNull() && value->IsBoolean()) {
    *result = static_cast<bool>(value->BooleanValue());
    return true;
  }
  return false;
}

// static
void Utils::AsciiToUChar(const char* source,
                         int32_t source_length,
                         UChar* target,
                         int32_t target_length) {
  int32_t length =
      source_length < target_length ? source_length : target_length;

  if (length <= 0) {
    return;
  }

  for (int32_t i = 0; i < length - 1; ++i) {
    target[i] = static_cast<UChar>(source[i]);
  }

  target[length - 1] = 0x0u;
}

// static
// Chrome Linux doesn't like static initializers in class, so we create
// template on demand.
v8::Local<v8::ObjectTemplate> Utils::GetTemplate(v8::Isolate* isolate) {
  static v8::Persistent<v8::ObjectTemplate> icu_template;

  if (icu_template.IsEmpty()) {
    v8::Local<v8::ObjectTemplate> raw_template(v8::ObjectTemplate::New());

    // Set aside internal field for ICU class.
    raw_template->SetInternalFieldCount(1);

    icu_template.Reset(isolate, raw_template);
  }

  return v8::Local<v8::ObjectTemplate>::New(isolate, icu_template);
}

// static
// Chrome Linux doesn't like static initializers in class, so we create
// template on demand. This one has 2 internal fields.
v8::Local<v8::ObjectTemplate> Utils::GetTemplate2(v8::Isolate* isolate) {
  static v8::Persistent<v8::ObjectTemplate> icu_template_2;

  if (icu_template_2.IsEmpty()) {
    v8::Local<v8::ObjectTemplate> raw_template(v8::ObjectTemplate::New());

    // Set aside internal field for ICU class and additional data.
    raw_template->SetInternalFieldCount(2);

    icu_template_2.Reset(isolate, raw_template);
  }

  return v8::Local<v8::ObjectTemplate>::New(isolate, icu_template_2);
}

}  // namespace v8_i18n
