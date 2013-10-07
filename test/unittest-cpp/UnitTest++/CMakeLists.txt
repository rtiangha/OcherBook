include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

list (APPEND UNIT_TEST_FILES
    src/AssertException.cpp
    src/Test.cpp
    src/Checks.cpp
    src/TestRunner.cpp
    src/TestResults.cpp
    src/TestReporter.cpp
    src/TestReporterStdout.cpp
    src/ReportAssert.cpp
    src/TestList.cpp
    src/TimeConstraint.cpp
    src/TestDetails.cpp
    src/MemoryOutStream.cpp
    src/DeferredTestReporter.cpp
    src/DeferredTestResult.cpp
    src/XmlTestReporter.cpp
    src/CurrentTest.cpp
    )

if (UNIX)
    list(APPEND UNIT_TEST_FILES
        src/Posix/SignalTranslator.cpp
        src/Posix/TimeHelpers.cpp
        )
elseif (WIN32)
    list(APPEND UNIT_TEST_FILES
        src/Win32/TimeHelpers.cpp
        )
endif()


add_library (UnitTest++ STATIC ${UNIT_TEST_FILES})
