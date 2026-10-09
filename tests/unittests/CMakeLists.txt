# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

project(GTlab-Unittests)

cmake_minimum_required(VERSION 3.12)

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

# Real plugins for module metadata integration tests. Keep them outside the
# scanned application module directory so tests control each load explicitly.
foreach(fixture IN ITEMS ModuleA ModuleB SuccessfulOverride FailedOverride FailedNew)
    set(fixture_id "${fixture}")
    set(fixture_metadata "{}")
    if(fixture STREQUAL "FailedOverride" OR fixture STREQUAL "FailedNew")
        set(fixture_metadata "{\"dependencies\":[{\"name\":\"MissingDependency\"}]}")
    endif()
    if(fixture STREQUAL "SuccessfulOverride")
        set(fixture_metadata "{\"dependencies\":[{\"name\":\"ModuleB\"}]}")
    endif()
    if(fixture STREQUAL "FailedOverride" OR fixture STREQUAL "SuccessfulOverride")
        set(fixture_id "ModuleA")
    endif()
    set(fixture_dir "${CMAKE_CURRENT_BINARY_DIR}/modulefixtures/${fixture}")
    file(MAKE_DIRECTORY "${fixture_dir}")
    configure_file(modulefixtures/module.cpp "${fixture_dir}/module.cpp" COPYONLY)
    file(WRITE "${fixture_dir}/metadata.json" "${fixture_metadata}")
    add_library(Footprint${fixture} MODULE "${fixture_dir}/module.cpp")
    set_target_properties(Footprint${fixture} PROPERTIES AUTOMOC ON
        LIBRARY_OUTPUT_DIRECTORY "${fixture_dir}"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${fixture_dir}"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${fixture_dir}"
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${fixture_dir}"
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${fixture_dir}"
        RUNTIME_OUTPUT_DIRECTORY "${fixture_dir}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${fixture_dir}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${fixture_dir}"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${fixture_dir}"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${fixture_dir}")
    target_compile_definitions(Footprint${fixture} PRIVATE GT_MODULE_ID="${fixture_id}")
    target_link_libraries(Footprint${fixture} PRIVATE GTlab::Core)
    add_dependencies(GTlabUnitTest Footprint${fixture})
    target_compile_definitions(GTlabUnitTest PRIVATE
        FOOTPRINT_${fixture}_PATH="$<TARGET_FILE:Footprint${fixture}>")
endforeach()
