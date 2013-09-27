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
 * Initializes the given object so it's a valid BreakIterator instance.
 * Useful for subclassing.
 */
function initializeBreakIterator(iterator, locales, options) {
  native function NativeJSCreateBreakIterator();

  if (iterator.hasOwnProperty('__initializedIntlObject')) {
    throw new TypeError('Trying to re-initialize v8BreakIterator object.');
  }

  if (options === undefined) {
    options = {};
  }

  var getOption = getGetOption(options, 'breakiterator');

  var internalOptions = {};

  defineWEProperty(internalOptions, 'type', getOption(
    'type', 'string', ['character', 'word', 'sentence', 'line'], 'word'));

  var locale = resolveLocale('breakiterator', locales, options);
  var resolved = Object.defineProperties({}, {
    requestedLocale: {value: locale.locale, writable: true},
    type: {value: internalOptions.type, writable: true},
    locale: {writable: true}
  });

  var internalIterator = NativeJSCreateBreakIterator(locale.locale,
                                                     internalOptions,
                                                     resolved);

  Object.defineProperty(iterator, 'iterator', {value: internalIterator});
  Object.defineProperty(iterator, 'resolved', {value: resolved});
  Object.defineProperty(iterator, '__initializedIntlObject',
                        {value: 'breakiterator'});

  return iterator;
}


/**
 * Constructs Intl.v8BreakIterator object given optional locales and options
 * parameters.
 *
 * @constructor
 */
%SetProperty(Intl, 'v8BreakIterator', function() {
    var locales = arguments[0];
    var options = arguments[1];

    if (!this || this === Intl) {
      // Constructor is called as a function.
      return new Intl.v8BreakIterator(locales, options);
    }

    return initializeBreakIterator(toObject(this), locales, options);
  },
  ATTRIBUTES.DONT_ENUM
);


/**
 * BreakIterator resolvedOptions method.
 */
%SetProperty(Intl.v8BreakIterator.prototype, 'resolvedOptions', function() {
    if (%_IsConstructCall()) {
      throw new TypeError(ORDINARY_FUNCTION_CALLED_AS_CONSTRUCTOR);
    }

    if (!this || typeof this !== 'object' ||
        this.__initializedIntlObject !== 'breakiterator') {
      throw new TypeError('resolvedOptions method called on a non-object or ' +
          'on a object that is not Intl.v8BreakIterator.');
    }

    var segmenter = this;
    var locale = getOptimalLanguageTag(segmenter.resolved.requestedLocale,
                                       segmenter.resolved.locale);

    return {
      locale: locale,
      type: segmenter.resolved.type
    };
  },
  ATTRIBUTES.DONT_ENUM
);
%FunctionRemovePrototype(Intl.v8BreakIterator.prototype.resolvedOptions);


/**
 * Returns the subset of the given locale list for which this locale list
 * has a matching (possibly fallback) locale. Locales appear in the same
 * order in the returned list as in the input list.
 * Options are optional parameter.
 */
%SetProperty(Intl.v8BreakIterator, 'supportedLocalesOf', function(locales) {
    if (%_IsConstructCall()) {
      throw new TypeError(ORDINARY_FUNCTION_CALLED_AS_CONSTRUCTOR);
    }

    return supportedLocalesOf('breakiterator', locales, arguments[1]);
  },
  ATTRIBUTES.DONT_ENUM
);
%FunctionRemovePrototype(Intl.v8BreakIterator.supportedLocalesOf);


/**
 * Adopts text to segment using the iterator. Old text, if present,
 * gets discarded.
 */
function adoptText(iterator, text) {
  native function NativeJSBreakIteratorAdoptText();
  NativeJSBreakIteratorAdoptText(iterator.iterator, String(text));
}


/**
 * Returns index of the first break in the string and moves current pointer.
 */
function first(iterator) {
  native function NativeJSBreakIteratorFirst();
  return NativeJSBreakIteratorFirst(iterator.iterator);
}


/**
 * Returns the index of the next break and moves the pointer.
 */
function next(iterator) {
  native function NativeJSBreakIteratorNext();
  return NativeJSBreakIteratorNext(iterator.iterator);
}


/**
 * Returns index of the current break.
 */
function current(iterator) {
  native function NativeJSBreakIteratorCurrent();
  return NativeJSBreakIteratorCurrent(iterator.iterator);
}


/**
 * Returns type of the current break.
 */
function breakType(iterator) {
  native function NativeJSBreakIteratorBreakType();
  return NativeJSBreakIteratorBreakType(iterator.iterator);
}


addBoundMethod(Intl.v8BreakIterator, 'adoptText', adoptText, 1);
addBoundMethod(Intl.v8BreakIterator, 'first', first, 0);
addBoundMethod(Intl.v8BreakIterator, 'next', next, 0);
addBoundMethod(Intl.v8BreakIterator, 'current', current, 0);
addBoundMethod(Intl.v8BreakIterator, 'breakType', breakType, 0);
