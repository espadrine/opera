#!/usr/bin/python
#
# Copyright 2013 the v8-i18n authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


import os
import subprocess
import sys
import time


def ListTests(start_dir):
  '''
  Walks the perf test tree and gathers all tests.
  '''
  tests = []
  for root, dirs, files in os.walk(start_dir):
    if (len(files) == 0):
      continue
    for file in files:
      if file.endswith('.js'):
        tests.append(os.path.join(root, file))
    tests.sort()

  return tests


def main(argv):
  if (len(argv) < 2):
    print 'Usage:\n\tpython run-tests.py path/to/test-runner [count]'
    print '\tcount - If not specified defaults to 500 runs per test'
    return

  script_dir = os.path.normpath(os.path.dirname(__file__))
  tests = ListTests(script_dir)

  count = '500'
  if len(argv) == 3:
    count = argv[2]

  for test in tests:
    print "Running %s %s times" % (test, count)
    status = subprocess.call([argv[1], '-t', count, test])
    if status != 0:
      raise Exception('Test failed: ' + test);
    print

if __name__ == '__main__':
  main(sys.argv)
