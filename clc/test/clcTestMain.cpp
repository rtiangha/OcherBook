#include <UnitTest++.h>
#include <stdio.h>

#define CLC_LOG_LEVEL 5
#include "clc/support/LogAppenders.h"

void resetUnitTestLoggers()
{
    static clc::LogAppenderCFile console(stderr);
    clc::Log::reset();
    clc::Log::get("")->setLevel(clc::Log::Warn);
    clc::Log::get("")->setAppender(&console);
}

#define PRINT_PROGRESS  printf("Testing %s::%s...\n", UnitTestSuite::GetSuiteName(), m_details.testName);fflush(stdout)

#define REQUIRE_RESOURCES_DIR \
    if (! resourcesDir) { \
        printf("\tSpecify resources directory on command line; skipping test...\n"); \
        return; \
    }
char* resourcesDir;

#include "BufferTest.h"
#include "ListTest.h"

#ifdef USE_LIBTASK
extern "C"
void taskmain(int /*argc*/, char** argv)
{
    resourcesDir = argv[1];
    resetUnitTestLoggers();
    UnitTest::RunAllTests();
}
#else
int main(int /*argc*/, char** argv)
{
    resourcesDir = argv[1];
    resetUnitTestLoggers();
    return UnitTest::RunAllTests();
}
#endif
