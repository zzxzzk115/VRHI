# ============================================================================
# VRHI Tests
# ============================================================================

message(STATUS "Configuring tests...")

# ============================================================================
# Unit Tests
# ============================================================================

# Core type tests
add_executable(CoreTypesTests
    unit/CoreTypesTests.cpp
)

target_link_libraries(CoreTypesTests
    PRIVATE
        VRHI::VRHI
        gtest
        gtest_main
)

# Set C++ standard for tests
set_target_properties(CoreTypesTests PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Register tests with CTest
add_test(NAME CoreTypesTests COMMAND CoreTypesTests)

# Logging system tests
add_executable(LoggingTests
    unit/LoggingTests.cpp
)

target_link_libraries(LoggingTests
    PRIVATE
        VRHI::VRHI
        gtest
        gtest_main
)

set_target_properties(LoggingTests PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_test(NAME LoggingTests COMMAND LoggingTests)

# Backend interface tests
add_executable(BackendInterfaceTests
    unit/BackendInterfaceTests.cpp
)

target_link_libraries(BackendInterfaceTests
    PRIVATE
        VRHI::VRHI
        gtest
        gtest_main
)

set_target_properties(BackendInterfaceTests PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_test(NAME BackendInterfaceTests COMMAND BackendInterfaceTests)

# Core API tests
add_executable(CoreAPITests
    unit/CoreAPITests.cpp
)

target_link_libraries(CoreAPITests
    PRIVATE
        VRHI::VRHI
        gtest
        gtest_main
)

set_target_properties(CoreAPITests PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_test(NAME CoreAPITests COMMAND CoreAPITests)

# Backend scoring tests
add_executable(BackendScoringTests
    unit/BackendScoringTests.cpp
)

target_link_libraries(BackendScoringTests
    PRIVATE
        VRHI::VRHI
        gtest
        gtest_main
)

set_target_properties(BackendScoringTests PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_test(NAME BackendScoringTests COMMAND BackendScoringTests)

# Feature detection tests
add_executable(FeatureDetectionTests
    unit/FeatureDetectionTests.cpp
)

target_link_libraries(FeatureDetectionTests
    PRIVATE
        VRHI::VRHI
        gtest
        gtest_main
)

set_target_properties(FeatureDetectionTests PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_test(NAME FeatureDetectionTests COMMAND FeatureDetectionTests)

# ============================================================================
# Test Summary
# ============================================================================

message(STATUS "Tests configured:")
message(STATUS "  CoreTypesTests: Unit tests for core types and error handling")
message(STATUS "  LoggingTests: Unit tests for logging system")
message(STATUS "  BackendInterfaceTests: Unit tests for backend and device interfaces")
message(STATUS "  CoreAPITests: Unit tests for Core API functions")
message(STATUS "  BackendScoringTests: Unit tests for backend scoring system")
message(STATUS "  FeatureDetectionTests: Unit tests for feature detection system")
