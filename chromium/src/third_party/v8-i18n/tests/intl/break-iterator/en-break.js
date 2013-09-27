// Copyright 2012 the v8-i18n authors.
//
// Licensed under the Apache License, Version 2.0 (the 'License');
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an 'AS IS' BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Segment plain English sentence and check results.

var iterator = new Intl.v8BreakIterator(['en']);

var textToSegment = 'Jack and Jill, went over hill, and got lost. Alert!';
iterator.adoptText(textToSegment);

var slices = [];
var types = [];
var pos = iterator.first();
while (pos !== -1) {
  var nextPos = iterator.next();
  if (nextPos === -1) break;

  slices.push(textToSegment.slice(pos, nextPos));
  types.push(iterator.breakType());

  pos = nextPos;
}

assertEquals('Jack', slices[0]);
assertEquals(' ', slices[1]);
assertEquals('and', slices[2]);
assertEquals(' ', slices[3]);
assertEquals('Jill', slices[4]);
assertEquals(',', slices[5]);
assertEquals('!', slices[slices.length - 1]);

assertEquals('letter', types[0]);
assertEquals('none', types[1]);
assertEquals('letter', types[2]);
assertEquals('none', types[3]);
assertEquals('letter', types[4]);
assertEquals('none', types[types.length - 1]);
