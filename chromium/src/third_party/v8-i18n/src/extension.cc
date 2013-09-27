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

#include "include/extension.h"

#include "src/break-iterator.h"
#include "src/collator.h"
#include "src/date-format.h"
#include "src/locale.h"
#include "src/natives.h"
#include "src/number-format.h"

namespace v8_i18n {

Extension* Extension::extension_ = NULL;

Extension::Extension()
    : v8::Extension("v8/i18n", Natives::GetScriptSource()) {
}

v8::Handle<v8::FunctionTemplate> Extension::GetNativeFunction(
    v8::Handle<v8::String> name) {
  // Standalone, helper methods.
  if (name->Equals(v8::String::New("NativeJSCanonicalizeLanguageTag"))) {
    return v8::FunctionTemplate::New(JSCanonicalizeLanguageTag);
  } else if (name->Equals(v8::String::New("NativeJSAvailableLocalesOf"))) {
    return v8::FunctionTemplate::New(JSAvailableLocalesOf);
  } else if (name->Equals(v8::String::New("NativeJSGetDefaultICULocale"))) {
    return v8::FunctionTemplate::New(JSGetDefaultICULocale);
  } else if (name->Equals(v8::String::New("NativeJSGetLanguageTagVariants"))) {
    return v8::FunctionTemplate::New(JSGetLanguageTagVariants);
  }

  // Date format and parse.
  if (name->Equals(v8::String::New("NativeJSCreateDateTimeFormat"))) {
    return v8::FunctionTemplate::New(DateFormat::JSCreateDateTimeFormat);
  } else if (name->Equals(v8::String::New("NativeJSInternalDateFormat"))) {
    return v8::FunctionTemplate::New(DateFormat::JSInternalFormat);
  } else if (name->Equals(v8::String::New("NativeJSInternalDateParse"))) {
    return v8::FunctionTemplate::New(DateFormat::JSInternalParse);
  }

  // Number format and parse.
  if (name->Equals(v8::String::New("NativeJSCreateNumberFormat"))) {
    return v8::FunctionTemplate::New(NumberFormat::JSCreateNumberFormat);
  } else if (name->Equals(v8::String::New("NativeJSInternalNumberFormat"))) {
    return v8::FunctionTemplate::New(NumberFormat::JSInternalFormat);
  } else if (name->Equals(v8::String::New("NativeJSInternalNumberParse"))) {
    return v8::FunctionTemplate::New(NumberFormat::JSInternalParse);
  }

  // Collator.
  if (name->Equals(v8::String::New("NativeJSCreateCollator"))) {
    return v8::FunctionTemplate::New(Collator::JSCreateCollator);
  } else if (name->Equals(v8::String::New("NativeJSInternalCompare"))) {
    return v8::FunctionTemplate::New(Collator::JSInternalCompare);
  }

  // Break iterator.
  if (name->Equals(v8::String::New("NativeJSCreateBreakIterator"))) {
    return v8::FunctionTemplate::New(BreakIterator::JSCreateBreakIterator);
  } else if (name->Equals(v8::String::New("NativeJSBreakIteratorAdoptText"))) {
    return v8::FunctionTemplate::New(
        BreakIterator::JSInternalBreakIteratorAdoptText);
  } else if (name->Equals(v8::String::New("NativeJSBreakIteratorFirst"))) {
    return v8::FunctionTemplate::New(
        BreakIterator::JSInternalBreakIteratorFirst);
  } else if (name->Equals(v8::String::New("NativeJSBreakIteratorNext"))) {
    return v8::FunctionTemplate::New(
        BreakIterator::JSInternalBreakIteratorNext);
  } else if (name->Equals(v8::String::New("NativeJSBreakIteratorCurrent"))) {
    return v8::FunctionTemplate::New(
        BreakIterator::JSInternalBreakIteratorCurrent);
  } else if (name->Equals(v8::String::New("NativeJSBreakIteratorBreakType"))) {
    return v8::FunctionTemplate::New(
	BreakIterator::JSInternalBreakIteratorBreakType);
  }

  return v8::Handle<v8::FunctionTemplate>();
}

Extension* Extension::get() {
  if (!extension_) {
    extension_ = new Extension();
  }
  return extension_;
}

void Extension::Register() {
  static v8::DeclareExtension extension_declaration(Extension::get());
}

}  // namespace v8_i18n
