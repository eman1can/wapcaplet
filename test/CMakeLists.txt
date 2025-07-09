cmake_minimum_required(VERSION 3.14)

project(wapcapletTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(wapcaplet REQUIRED)
  enable_testing()
endif()

find_package(GTest REQUIRED)

# ---- Tests ----

add_executable(wapcaplet_test src/wapcaplet_test.cpp)
target_link_libraries(
    wapcaplet_test PRIVATE
    wapcaplet::wapcaplet
    GTest::gtest
    GTest::gtest_main
)
target_compile_features(wapcaplet_test PRIVATE cxx_std_20)

gtest_discover_tests(wapcaplet_test)

# ---- End-of-file commands ----

add_folders(test)
