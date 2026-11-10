# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

cmake_minimum_required(VERSION 3.18)
project(GTlab-ProfilingBench)

if (EXISTS ${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
    cmake_policy(SET CMP0091 NEW)
    include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
endif()

project(GTlabProfilingBench)

# Profiling benchmarks (heavy A/B benchmark cases).
# Kept in a separate executable so the regular unit tests (GTlabUnitTest)
# stay fast. Drop new profiling benchmark sources into this directory.
#
# Run on demand:
#   ./build/GTlabProfilingBench
#   ./build/GTlabProfilingBench --gtest_filter=MementoDiffBench.*
#   ctest -R "Profiling\." --test-dir build

# Share the gtest integration helpers and the common test main with the
# unit tests.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../unittests/cmake)

if (NOT TARGET Qt::Core)
    # Todo version agnostic qt version search
    find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
endif()

if (NOT TARGET GTlab::Core)
    find_package(GTlab REQUIRED)
endif()

if (NOT TARGET gtest)
    include(AddGoogleTest)
endif()

FILE (GLOB_RECURSE PROF_SRCS *.cpp *.h)

set(SOURCES
    ${PROF_SRCS}
    ${CMAKE_CURRENT_LIST_DIR}/../unittests/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../unittests/gt_testhelper.cpp
)

add_executable(GTlabProfilingBench ${SOURCES})
target_sources(GTlabProfilingBench PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../unittests/datamodel/test_gt_object.h
    ${CMAKE_CURRENT_LIST_DIR}/../unittests/datamodel/test_propertycontainerobject.h
)
set_target_properties(GTlabProfilingBench PROPERTIES AUTOMOC ON)

target_compile_definitions(GTlabProfilingBench PRIVATE GT_MODULE_ID="ProfilingBench" "GT_LOG_USE_QT_BINDINGS")
target_include_directories(GTlabProfilingBench PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../unittests
    ${CMAKE_CURRENT_LIST_DIR}/../unittests/datamodel
)
target_link_libraries(GTlabProfilingBench PRIVATE
    GTlab::Core
    GTlab::Gui
    GTlab::DataProcessor
    gtest
    gmock
    mpark::variant
)

include(GoogleTest)
gtest_discover_tests(GTlabProfilingBench TEST_PREFIX "Profiling." DISCOVERY_MODE PRE_TEST)
