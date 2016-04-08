#include "TestReporterStderr.h"
#include <cstdio>
#include <iostream>

#include "TestDetails.h"

namespace UnitTest {

void TestReporterStderr::ReportFailure(TestDetails const& details, char const* failure)
{
  using namespace std;
#if defined(__APPLE__) || defined(__GNUG__)
    cerr << details.filename << ":" << details.lineNumber << ": error: Failure in "
         << details.testName << ": " << failure << endl;
#else
  cerr << details.filename << "(" << details.lineNumber << ") error: Failure in "
       << details.testName << ": " << failure << endl;
#endif
}

void TestReporterStderr::ReportTestStart(TestDetails const& /*test*/)
{
}

void TestReporterStderr::ReportTestFinish(TestDetails const& /*test*/, float)
{
}

void TestReporterStderr::ReportSummary(int const totalTestCount, int const failedTestCount,
                                       int const failureCount, float secondsElapsed)
{
	using namespace std;

  cerr << "FAILURE: " << failedTestCount << " out of " << totalTestCount << " (" << failureCount << ")." << endl;
  cerr << "Test time: " << secondsElapsed << " seconds." << endl;
}

}
