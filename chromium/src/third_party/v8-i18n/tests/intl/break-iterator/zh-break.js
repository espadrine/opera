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

// Segment plain Chinese sentence and check results.

var iterator = new Intl.v8BreakIterator(['zh']);

var textToSegment = '\u56FD\u52A1\u9662\u5173\u4E8E\u300A\u571F\u5730' +
                    '\u623F\u5C4B\u7BA1\u7406\u6761\u4F8B\u300B';
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

assertEquals('\u56FD\u52A1\u9662', slices[0]);
assertEquals('\u5173\u4E8E', slices[1]);
assertEquals('\u300A', slices[2]);
assertEquals('\u571F\u5730', slices[3]);
assertEquals('\u623F\u5C4B', slices[4]);
assertEquals('\u7BA1\u7406', slices[5]);
assertEquals('\u6761\u4F8B', slices[6]);
assertEquals('\u300B', slices[7]);

assertEquals('ideo', types[0]);
assertEquals('ideo', types[1]);
assertEquals('none', types[2]);
assertEquals('ideo', types[3]);
assertEquals('ideo', types[4]);
assertEquals('none', types[types.length - 1]);
