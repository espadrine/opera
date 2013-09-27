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


// Save references to Intl objects and methods we use, for added security.
var savedObjects = {
  'collator': Intl.Collator,
  'numberformat': Intl.NumberFormat,
  'dateformatall': Intl.DateTimeFormat,
  'dateformatdate': Intl.DateTimeFormat,
  'dateformattime': Intl.DateTimeFormat
};


// Default (created with undefined locales and options parameters) collator,
// number and date format instances. They'll be created as needed.
var defaultObjects = {
  'collator': undefined,
  'numberformat': undefined,
  'dateformatall': undefined,
  'dateformatdate': undefined,
  'dateformattime': undefined,
};


/**
 * Returns cached or newly created instance of a given service.
 * We cache only default instances (where no locales or options are provided).
 */
function cachedOrNewService(service, locales, options, defaults) {
  var useOptions = (defaults === undefined) ? options : defaults;
  if (locales === undefined && options === undefined) {
    if (defaultObjects[service] === undefined) {
      defaultObjects[service] = new savedObjects[service](locales, useOptions);
    }
    return defaultObjects[service];
  }
  return new savedObjects[service](locales, useOptions);
}


/**
 * Compares this and that, and returns less than 0, 0 or greater than 0 value.
 * Overrides the built-in method.
 */
Object.defineProperty(String.prototype, 'localeCompare', {
  value: function(that) {
    if (%_IsConstructCall()) {
      throw new TypeError(ORDINARY_FUNCTION_CALLED_AS_CONSTRUCTOR);
    }

    if (this === undefined || this === null) {
      throw new TypeError('Method invoked on undefined or null value.');
    }

    var locales = arguments[1];
    var options = arguments[2];
    var collator = cachedOrNewService('collator', locales, options);
    return compare(collator, this, that);
  },
  writable: true,
  configurable: true,
  enumerable: false
});
%FunctionRemovePrototype(String.prototype.localeCompare);


/**
 * Formats a Number object (this) using locale and options values.
 * If locale or options are omitted, defaults are used.
 */
Object.defineProperty(Number.prototype, 'toLocaleString', {
  value: function() {
    if (%_IsConstructCall()) {
      throw new TypeError(ORDINARY_FUNCTION_CALLED_AS_CONSTRUCTOR);
    }

    if (!(this instanceof Number) && typeof(this) !== 'number') {
      throw new TypeError('Method invoked on an object that is not Number.');
    }

    var locales = arguments[0];
    var options = arguments[1];
    var numberFormat = cachedOrNewService('numberformat', locales, options);
    return formatNumber(numberFormat, this);
  },
  writable: true,
  configurable: true,
  enumerable: false
});
%FunctionRemovePrototype(Number.prototype.toLocaleString);


/**
 * Returns actual formatted date or fails if date parameter is invalid.
 */
function toLocaleDateTime(date, locales, options, required, defaults, service) {
  if (!(date instanceof Date)) {
    throw new TypeError('Method invoked on an object that is not Date.');
  }

  if (isNaN(date)) {
    return 'Invalid Date';
  }

  var internalOptions = toDateTimeOptions(options, required, defaults);

  var dateFormat =
      cachedOrNewService(service, locales, options, internalOptions);

  return formatDate(dateFormat, date);
}


/**
 * Formats a Date object (this) using locale and options values.
 * If locale or options are omitted, defaults are used - both date and time are
 * present in the output.
 */
Object.defineProperty(Date.prototype, 'toLocaleString', {
  value: function() {
    if (%_IsConstructCall()) {
      throw new TypeError(ORDINARY_FUNCTION_CALLED_AS_CONSTRUCTOR);
    }

    var locales = arguments[0];
    var options = arguments[1];
    return toLocaleDateTime(
        this, locales, options, 'any', 'all', 'dateformatall');
  },
  writable: true,
  configurable: true,
  enumerable: false
});
%FunctionRemovePrototype(Date.prototype.toLocaleString);


/**
 * Formats a Date object (this) using locale and options values.
 * If locale or options are omitted, defaults are used - only date is present
 * in the output.
 */
Object.defineProperty(Date.prototype, 'toLocaleDateString', {
  value: function() {
    if (%_IsConstructCall()) {
      throw new TypeError(ORDINARY_FUNCTION_CALLED_AS_CONSTRUCTOR);
    }

    var locales = arguments[0];
    var options = arguments[1];
    return toLocaleDateTime(
        this, locales, options, 'date', 'date', 'dateformatdate');
  },
  writable: true,
  configurable: true,
  enumerable: false
});
%FunctionRemovePrototype(Date.prototype.toLocaleDateString);


/**
 * Formats a Date object (this) using locale and options values.
 * If locale or options are omitted, defaults are used - only time is present
 * in the output.
 */
Object.defineProperty(Date.prototype, 'toLocaleTimeString', {
  value: function() {
    if (%_IsConstructCall()) {
      throw new TypeError(ORDINARY_FUNCTION_CALLED_AS_CONSTRUCTOR);
    }

    var locales = arguments[0];
    var options = arguments[1];
    return toLocaleDateTime(
        this, locales, options, 'time', 'time', 'dateformattime');
  },
  writable: true,
  configurable: true,
  enumerable: false
});
%FunctionRemovePrototype(Date.prototype.toLocaleTimeString);
