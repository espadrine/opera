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


/**
 * List of available services.
 */
var AVAILABLE_SERVICES = ['collator',
			  'numberformat',
			  'dateformat',
			  'breakiterator'];

/**
 * Caches available locales for each service.
 */
var AVAILABLE_LOCALES = {
  'collator': undefined,
  'numberformat': undefined,
  'dateformat': undefined,
  'breakiterator': undefined
};

/**
 * Caches default ICU locale.
 */
var DEFAULT_ICU_LOCALE = undefined;

/**
 * Unicode extension regular expression.
 */
var UNICODE_EXTENSION_RE = new RegExp('-u(-[a-z0-9]{2,8})+', 'g');

/**
 * Matches any Unicode extension.
 */
var ANY_EXTENSION_RE = new RegExp('-[a-z0-9]{1}-.*', 'g');

/**
 * Replace quoted text (single quote, anything but the quote and quote again).
 */
var QUOTED_STRING_RE = new RegExp("'[^']+'", 'g');

/**
 * Matches valid service name.
 */
var SERVICE_RE =
    new RegExp('^(collator|numberformat|dateformat|breakiterator)$');

/**
 * Validates a language tag against bcp47 spec.
 * Actual value is assigned on first run.
 */
var LANGUAGE_TAG_RE = undefined;

/**
 * Helps find duplicate variants in the language tag.
 */
var LANGUAGE_VARIANT_RE = undefined;

/**
 * Helps find duplicate singletons in the language tag.
 */
var LANGUAGE_SINGLETON_RE = undefined;

/**
 * Matches valid IANA time zone names.
 */
var TIMEZONE_NAME_CHECK_RE =
    new RegExp('^([A-Za-z]+)/([A-Za-z]+)(?:_([A-Za-z]+))*$');

/**
 * Maps ICU calendar names into LDML type.
 */
var ICU_CALENDAR_MAP = {
  'gregorian': 'gregory',
  'japanese': 'japanese',
  'buddhist': 'buddhist',
  'roc': 'roc',
  'persian': 'persian',
  'islamic-civil': 'islamicc',
  'islamic': 'islamic',
  'hebrew': 'hebrew',
  'chinese': 'chinese',
  'indian': 'indian',
  'coptic': 'coptic',
  'ethiopic': 'ethiopic',
  'ethiopic-amete-alem': 'ethioaa'
};

/**
 * Map of Unicode extensions to option properties, and their values and types,
 * for a collator.
 */
var COLLATOR_KEY_MAP = {
  'kn': {'property': 'numeric', 'type': 'boolean'},
  'kf': {'property': 'caseFirst', 'type': 'string',
         'values': ['false', 'lower', 'upper']}
};

/**
 * Map of Unicode extensions to option properties, and their values and types,
 * for a number format.
 */
var NUMBER_FORMAT_KEY_MAP = {
  'nu': {'property': undefined, 'type': 'string'}
};

/**
 * Map of Unicode extensions to option properties, and their values and types,
 * for a date/time format.
 */
var DATETIME_FORMAT_KEY_MAP = {
  'ca': {'property': undefined, 'type': 'string'},
  'nu': {'property': undefined, 'type': 'string'}
};

/**
 * Allowed -u-co- values. List taken from:
 * http://unicode.org/repos/cldr/trunk/common/bcp47/collation.xml
 */
var ALLOWED_CO_VALUES = [
  'big5han', 'dict', 'direct', 'ducet', 'gb2312', 'phonebk', 'phonetic',
  'pinyin', 'reformed', 'searchjl', 'stroke', 'trad', 'unihan', 'zhuyin'
];

/**
 * Object attributes (configurable, writable, enumerable).
 * To combine attributes, OR them.
 * Values/names are copied from v8/include/v8.h:PropertyAttribute
 */
var ATTRIBUTES = {
  'NONE': 0,
  'READ_ONLY': 1,
  'DONT_ENUM': 2,
  'DONT_DELETE': 4
};

/**
 * Error message for when function object is created with new and it's not
 * a constructor.
 */
var ORDINARY_FUNCTION_CALLED_AS_CONSTRUCTOR =
  'Function object that\'s not a constructor was created with new';
