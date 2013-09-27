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

// ECMAScript 402 API implementation is broken into separate files for
// each service. The build system combines them together into one
// Intl namespace.

/**
 * Initializes the given object so it's a valid Collator instance.
 * Useful for subclassing.
 */
function initializeCollator(collator, locales, options) {
  native function NativeJSCreateCollator();

  if (collator.hasOwnProperty('__initializedIntlObject')) {
    throw new TypeError('Trying to re-initialize Collator object.');
  }

  if (options === undefined) {
    options = {};
  }

  var getOption = getGetOption(options, 'collator');

  var internalOptions = {};

  defineWEProperty(internalOptions, 'usage', getOption(
    'usage', 'string', ['sort', 'search'], 'sort'));

  var sensitivity = getOption('sensitivity', 'string',
                              ['base', 'accent', 'case', 'variant']);
  if (sensitivity === undefined && internalOptions.usage === 'sort') {
    sensitivity = 'variant';
  }
  defineWEProperty(internalOptions, 'sensitivity', sensitivity);

  defineWEProperty(internalOptions, 'ignorePunctuation', getOption(
    'ignorePunctuation', 'boolean', undefined, false));

  var locale = resolveLocale('collator', locales, options);

  // ICU can't take kb, kc... parameters through localeID, so we need to pass
  // them as options.
  // One exception is -co- which has to be part of the extension, but only for
  // usage: sort, and its value can't be 'standard' or 'search'.
  var extensionMap = parseExtension(locale.extension);
  setOptions(
      options, extensionMap, COLLATOR_KEY_MAP, getOption, internalOptions);

  var collation = 'default';
  var extension = '';
  if (extensionMap.hasOwnProperty('co') && internalOptions.usage === 'sort') {
    if (ALLOWED_CO_VALUES.indexOf(extensionMap.co) !== -1) {
      extension = '-u-co-' + extensionMap.co;
      // ICU can't tell us what the collation is, so save user's input.
      collation = extensionMap.co;
    }
  } else if (internalOptions.usage === 'search') {
    extension = '-u-co-search';
  }
  defineWEProperty(internalOptions, 'collation', collation);

  var requestedLocale = locale.locale + extension;

  // We define all properties C++ code may produce, to prevent security
  // problems. If malicious user decides to redefine Object.prototype.locale
  // we can't just use plain x.locale = 'us' or in C++ Set("locale", "us").
  // Object.defineProperties will either succeed defining or throw an error.
  var resolved = Object.defineProperties({}, {
    caseFirst: {writable: true},
    collation: {value: internalOptions.collation, writable: true},
    ignorePunctuation: {writable: true},
    locale: {writable: true},
    numeric: {writable: true},
    requestedLocale: {value: requestedLocale, writable: true},
    sensitivity: {writable: true},
    strength: {writable: true},
    usage: {value: internalOptions.usage, writable: true}
  });

  var internalCollator = NativeJSCreateCollator(requestedLocale,
                                                internalOptions,
                                                resolved);

  // Writable, configurable and enumerable are set to false by default.
  Object.defineProperty(collator, 'collator', {value: internalCollator});
  Object.defineProperty(collator, '__initializedIntlObject',
                        {value: 'collator'});
  Object.defineProperty(collator, 'resolved', {value: resolved});

  return collator;
}


/**
 * Constructs Intl.Collator object given optional locales and options
 * parameters.
 *
 * @constructor
 */
%SetProperty(Intl, 'Collator', function() {
    var locales = arguments[0];
    var options = arguments[1];

    if (!this || this === Intl) {
      // Constructor is called as a function.
      return new Intl.Collator(locales, options);
    }

    return initializeCollator(toObject(this), locales, options);
  },
  ATTRIBUTES.DONT_ENUM
);


/**
 * Collator resolvedOptions method.
 */
%SetProperty(Intl.Collator.prototype, 'resolvedOptions', function() {
    if (%_IsConstructCall()) {
      throw new TypeError(ORDINARY_FUNCTION_CALLED_AS_CONSTRUCTOR);
    }

    if (!this || typeof this !== 'object' ||
        this.__initializedIntlObject !== 'collator') {
      throw new TypeError('resolvedOptions method called on a non-object ' +
                          'or on a object that is not Intl.Collator.');
    }

    var coll = this;
    var locale = getOptimalLanguageTag(coll.resolved.requestedLocale,
                                       coll.resolved.locale);

    return {
      locale: locale,
      usage: coll.resolved.usage,
      sensitivity: coll.resolved.sensitivity,
      ignorePunctuation: coll.resolved.ignorePunctuation,
      numeric: coll.resolved.numeric,
      caseFirst: coll.resolved.caseFirst,
      collation: coll.resolved.collation
    };
  },
  ATTRIBUTES.DONT_ENUM
);
%FunctionRemovePrototype(Intl.Collator.prototype.resolvedOptions);


/**
 * Returns the subset of the given locale list for which this locale list
 * has a matching (possibly fallback) locale. Locales appear in the same
 * order in the returned list as in the input list.
 * Options are optional parameter.
 */
%SetProperty(Intl.Collator, 'supportedLocalesOf', function(locales) {
    if (%_IsConstructCall()) {
      throw new TypeError(ORDINARY_FUNCTION_CALLED_AS_CONSTRUCTOR);
    }

    return supportedLocalesOf('collator', locales, arguments[1]);
  },
  ATTRIBUTES.DONT_ENUM
);
%FunctionRemovePrototype(Intl.Collator.supportedLocalesOf);


/**
 * When the compare method is called with two arguments x and y, it returns a
 * Number other than NaN that represents the result of a locale-sensitive
 * String comparison of x with y.
 * The result is intended to order String values in the sort order specified
 * by the effective locale and collation options computed during construction
 * of this Collator object, and will be negative, zero, or positive, depending
 * on whether x comes before y in the sort order, the Strings are equal under
 * the sort order, or x comes after y in the sort order, respectively.
 */
function compare(collator, x, y) {
  native function NativeJSInternalCompare();
  return NativeJSInternalCompare(collator.collator, String(x), String(y));
};


addBoundMethod(Intl.Collator, 'compare', compare, 2);
