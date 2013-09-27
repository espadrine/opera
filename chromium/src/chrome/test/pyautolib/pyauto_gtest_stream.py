import unittest
import socket
import sys

class GTestStreamProtocolTestResult(unittest._TextTestResult):
    """
    Streams the results to a specified host using the same format as
    the gtests (GTest Streaming Protocol).

    Can be used from pyauto using the --stream-results argument.

    TODO: Elapsed time - we currently pass 0.
    """

    def __init__(self, stream, descriptions, verbosity, address):
        unittest._TextTestResult.__init__(self, stream, descriptions, verbosity)
        (host, port) = address.split(":")
        self._socket = socket.socket()
        self._socket.connect((host, int(port)))
        self._socket.send("gtest_streaming_protocol_version=1.0\n")
        self._current_parent = None
        self._pass_count = 0
        self._pass_parent_count = 0
        self._started = False

    def __del__(self):
        if sys.hexversion < 0x020700F0:
            self.stopTestRun()

    def startTestRun(self):
        """This is new to Python 2.7 - called manually for older versions"""
        unittest.TestResult.startTestRun(self)
        self._socket.send("event=TestProgramStart\n")
        self._socket.send("event=TestIterationStart&iteration=0\n")
        self._started = True

    def stopTestRun(self):
        """This is new to Python 2.7 - called manually for older versions"""
        unittest.TestResult.stopTestRun(self)
        self._endParent(None)
        self._socket.send("event=TestIterationEnd&passed=%d&elapsed_time=0ms\n" % self._pass_count)
        self._socket.send("event=TestProgramEnd&passed=%d\n" % self._pass_count )
        self._socket.shutdown(socket.SHUT_RDWR)
        self._socket.close()

    def startTest(self, test):
        unittest.TestResult.startTest(self, test)

        # Needed on 2.6 as startTestRun() is new to 2.7
        if not self._started:
            self.startTestRun()

        test_parent = u"%s.%s" % (test.__class__.__module__, test.__class__.__name__)
        print self._current_parent, test_parent
        if not self._current_parent:
            self._current_parent = test_parent
            self._socket.send(u"event=TestCaseStart&name=%s\n" % self._current_parent)
        else:
            self._endParent(test_parent)
        self._socket.send(u"event=TestStart&name=%s\n" % test._testMethodName)

    def addSuccess(self, test):
        unittest.TestResult.addSuccess(self, test)
        self._pass_count += 1
        self._pass_parent_count += 1
        self._socket.send("event=TestEnd&passed=1&elapsed_time=0ms\n")

    def addError(self, test, err):
        unittest.TestResult.addError(self, test, err)
        # Should Error be handled different from Failure ?
        self._socket.send("event=TestEnd&passed=0&elapsed_time=0ms\n")

    def addFailure(self, test, err):
        unittest.TestResult.addFailure(self, test, err)
        self._socket.send("event=TestEnd&passed=0&elapsed_time=0ms\n")

    def _endParent(self, test_parent):
        if self._current_parent != test_parent:
            self._socket.send("event=TestCaseEnd&passed=%d&elapsed_time=0ms\n" % self._pass_parent_count)
            self._pass_parent_count = 0
        self._current_parent = test_parent
