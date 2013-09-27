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
 * Canonicalizes the language tag, or throws in case the tag is invalid.
 */
function canonicalizeLanguageTag(localeID) {
  native function NativeJSCanonicalizeLanguageTag();

  // null is typeof 'object' so we have to do extra check.
  if (typeof localeID !== 'string' && typeof localeID !== 'object' ||
      localeID === null) {
    throw new TypeError('Language ID should be string or object.');
  }

  var localeString = String(localeID);

  if (isValidLanguageTag(localeString) === false) {
    throw new RangeError('Invalid language tag: ' + localeString);
  }

  // This call will strip -kn but not -kn-true extensions.
  // ICU bug filled - http://bugs.icu-project.org/trac/ticket/9265.
  // TODO(cira): check if -u-kn-true-kc-true-kh-true still throws after
  // upgrade to ICU 4.9.
  var tag = NativeJSCanonicalizeLanguageTag(localeString);
  if (tag === 'invalid-tag') {
    throw new RangeError('Invalid language tag: ' + localeString);
  }

  return tag;
}


/**
 * Returns an array where all locales are canonicalized and duplicates removed.
 * Throws on locales that are not well formed BCP47 tags.
 */
function initializeLocaleList(locales) {
  var seen = [];
  if (locales === undefined) {
    // Constructor is called without arguments.
    seen = [];
  } else {
    // We allow single string localeID.
    if (typeof locales === 'string') {
      seen.push(canonicalizeLanguageTag(locales));
      return freezeArray(seen);
    }

    var o = toObject(locales);
    // Converts it to UInt32 (>>> is shr on 32bit integers).
    var len = o.length >>> 0;

    for (var k = 0; k < len; k++) {
      if (k in o) {
        var value = o[k];

        var tag = canonicalizeLanguageTag(value);

        if (seen.indexOf(tag) === -1) {
          seen.push(tag);
        }
      }
    }
  }

  return freezeArray(seen);
}


/**
 * Validates the language tag. Section 2.2.9 of the bcp47 spec 
 * defines a valid tag. 
 * 
 * ICU is too permissible and lets invalid tags, like 
 * hant-cmn-cn, through. 
 *  
 * Returns false if the language tag is invalid.
 */
function isValidLanguageTag(locale) {
  // Check if it's well-formed, including grandfadered tags.
  if (LANGUAGE_TAG_RE.test(locale) === false) {
    return false;
  }

  // Just return if it's a x- form. It's all private.
  if (locale.indexOf('x-') === 0) {
    return true;
  }
    
  // Check if there are any duplicate variants or singletons (extensions).
 
  // Remove private use section.
  locale = locale.split(/-x-/)[0];

  // Skip language since it can match variant regex, so we start from 1.
  // We are matching i-klingon here, but that's ok, since i-klingon-klingon
  // is not valid and would fail LANGUAGE_TAG_RE test.
  var variants = [];
  var extensions = [];
  var parts = locale.split(/-/);
  for (var i = 1; i < parts.length; i++) {
    var value = parts[i];
    if (LANGUAGE_VARIANT_RE.test(value) === true && extensions.length === 0) {
      if (variants.indexOf(value) === -1) {
        variants.push(value);
      } else {
        return false;
      }
    }

    if (LANGUAGE_SINGLETON_RE.test(value) === true) {
      if (extensions.indexOf(value) === -1) {
        extensions.push(value);
      } else {
        return false;
      }
    }
  }

  return true;
 }


/**
 * Builds a regular expresion that validates the language tag 
 * against bcp47 spec. 
 * Uses http://tools.ietf.org/html/bcp47, section 2.1, ABNF. 
 * Runs on load and initializes the global REs. 
 */
(function() {
  var alpha = '[a-zA-Z]';
  var digit = '[0-9]';
  var alphanum = '(' + alpha + '|' + digit + ')';
  var regular = '(art-lojban|cel-gaulish|no-bok|no-nyn|zh-guoyu|zh-hakka|' +
                'zh-min|zh-min-nan|zh-xiang)';
  var irregular = '(en-GB-oed|i-ami|i-bnn|i-default|i-enochian|i-hak|' +
                  'i-klingon|i-lux|i-mingo|i-navajo|i-pwn|i-tao|i-tay|' +
                  'i-tsu|sgn-BE-FR|sgn-BE-NL|sgn-CH-DE)';
  var grandfathered = '(' + irregular + '|' + regular + ')';
  var privateUse = '(x(-' + alphanum + '{1,8})+)';

  var singleton = '(' + digit + '|[A-WY-Za-wy-z])';
  LANGUAGE_SINGLETON_RE = new RegExp('^' + singleton + '$', 'i');

  var extension = '(' + singleton + '(-' + alphanum + '{2,8})+)';

  var variant = '(' + alphanum + '{5,8}|(' + digit + alphanum + '{3}))';
  LANGUAGE_VARIANT_RE = new RegExp('^' + variant + '$', 'i');

  var region = '(' + alpha + '{2}|' + digit + '{3})';
  var script = '(' + alpha + '{4})';
  var extLang = '(' + alpha + '{3}(-' + alpha + '{3}){0,2})';
  var language = '(' + alpha + '{2,3}(-' + extLang + ')?|' + alpha + '{4}|' +
                 alpha + '{5,8})';
  var langTag = language + '(-' + script + ')?(-' + region + ')?(-' +
                variant + ')*(-' + extension + ')*(-' + privateUse + ')?';

  var languageTag =
      '^(' + langTag + '|' + privateUse + '|' + grandfathered + ')$';
  LANGUAGE_TAG_RE = new RegExp(languageTag, 'i');
})();
