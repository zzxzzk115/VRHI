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

# ============================================================================
# Test Summary
# ============================================================================

message(STATUS "Tests configured:")
message(STATUS "  CoreTypesTests: Unit tests for core types and error handling")
