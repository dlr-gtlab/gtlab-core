# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

cmake_minimum_required(VERSION 3.18)
project(GTlab-Unittests)

if (EXISTS ${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
    cmake_policy(SET CMP0091 NEW)
    include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
endif()

project(GTlabUnittests)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

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

FILE (GLOB_RECURSE DM_SRCS datamodel/*.cpp datamodel/*.h datamodel/data/data.qrc)
FILE (GLOB_RECURSE CAL_SRCS calculators/*.cpp calculators/*.h)
FILE (GLOB_RECURSE CORE_SRCS core/*.cpp core/*.h)
FILE (GLOB_RECURSE TP_SRCS thirdparty/*.cpp thirdparty/*.h)
FILE (GLOB_RECURSE EXP_SRCS exporter/*.cpp exporter/*.h)
FILE (GLOB_RECURSE MDI_SRCS mdi/*.cpp mdi/*.h)
FILE (GLOB_RECURSE GUI_SRCS gui/*.cpp gui/*.h)
list(FILTER GUI_SRCS EXCLUDE REGEX
    ".*/gui/(test_gt_processdockextensions|gt_processdockextensions_main)\\.cpp$")
FILE (GLOB_RECURSE NET_SRCS network/*.cpp network/*.h)

set(SOURCES
    ${DM_SRCS}
    ${CAL_SRCS}
    ${CORE_SRCS}
    ${TP_SRCS}
    ${EXP_SRCS}
    ${MDI_SRCS}
    ${GUI_SRCS}
    ${NET_SRCS}
)

set (SOURCES 
    ${SOURCES}
    main.cpp 
    gt_testhelper.cpp 
    slotadaptor.cpp
)

add_executable(GTlabUnitTest ${SOURCES})
target_sources(GTlabUnitTest PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../src/batch/gt_consoleruntaskfrommemento.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../src/batch/gt_consolepropertyoverrides.cpp
)
set_target_properties(GTlabUnitTest PROPERTIES AUTOMOC ON AUTORCC ON)

target_compile_definitions(GTlabUnitTest PRIVATE GT_MODULE_ID="Unittests" "GT_LOG_USE_QT_BINDINGS")
target_include_directories(GTlabUnitTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../src/batch
)
target_link_libraries(GTlabUnitTest PRIVATE
    GTlab::Core
    GTlab::Gui
    GTlab::DataProcessor
    gtest
    gmock
    mpark::variant
)

include(GoogleTest)
gtest_discover_tests(GTlabUnitTest TEST_PREFIX "Core." DISCOVERY_MODE PRE_TEST)

add_executable(GTlabProcessDockUnitTest
    gui/gt_processdockextensions_main.cpp
    gui/test_gt_processdockextensions.cpp
)
set_target_properties(GTlabProcessDockUnitTest PROPERTIES AUTOMOC ON)
target_compile_definitions(GTlabProcessDockUnitTest PRIVATE
    GT_MODULE_ID="Unittests" "GT_LOG_USE_QT_BINDINGS")
target_link_libraries(GTlabProcessDockUnitTest PRIVATE
    GTlab::Core GTlab::Gui gtest)
add_test(NAME ProcessDockExtensions COMMAND GTlabProcessDockUnitTest)
set_tests_properties(ProcessDockExtensions PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

