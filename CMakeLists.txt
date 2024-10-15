cmake_minimum_required(VERSION 3.14)
project(miniJSON VERSION 0.1.7)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Google Test Setup
include(FetchContent)
FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(miniJSON_tests tests/basic_parse_test.cpp tests/getter_test.cpp tests/setter_test.cpp tests/iterator_test.cpp)
include_directories(include)
target_link_libraries(miniJSON_tests GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(miniJSON_tests)