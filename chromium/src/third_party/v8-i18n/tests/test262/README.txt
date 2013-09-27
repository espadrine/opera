To test v8-i18n implementation against Ecma 402 tests:

1. Download EcmaScript test suite (may take a while):

hg clone http://hg.ecmascript.org/tests/test262/ data


2. Run (pick proper path to test-runner binary):

python run-tests.py ../../out/Debug/test-runner
