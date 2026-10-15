# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

cmake_minimum_required(VERSION 3.18)
project(GTlabExternalAbiConsumer LANGUAGES CXX)
enable_testing()

include(GNUInstallDirs)
set(_consumer_install_bindir "${CMAKE_INSTALL_BINDIR}")
set(_consumer_install_libdir "${CMAKE_INSTALL_LIBDIR}")

find_package(GTlab CONFIG REQUIRED)

if (NOT CMAKE_INSTALL_BINDIR STREQUAL _consumer_install_bindir
        OR NOT CMAKE_INSTALL_LIBDIR STREQUAL _consumer_install_libdir)
    message(FATAL_ERROR "find_package(GTlab) must not change the consumer's CMAKE_INSTALL_* values")
endif()

if (NOT GTLAB_CORE_VERSION OR NOT GTLAB_MODULE_ABI)
    message(FATAL_ERROR "The installed GTlab package must export version and ABI")
endif()

add_gtlab_module(ExternalAbiSmoke
    MODULE_ID "External ABI Smoke Test"
    SOURCES external_abi_smoke.cpp
)
target_link_libraries(ExternalAbiSmoke PRIVATE GTlab::Core)

if (APPLE OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if (NOT CMAKE_SKIP_RPATH AND NOT CMAKE_SKIP_INSTALL_RPATH)
        get_target_property(_module_install_rpath ExternalAbiSmoke INSTALL_RPATH)
        if (DEFINED CMAKE_INSTALL_RPATH AND NOT "${CMAKE_INSTALL_RPATH}" STREQUAL "")
            set(_expected_module_rpath "${CMAKE_INSTALL_RPATH}")
        elseif (APPLE)
            set(_expected_module_rpath "@loader_path/../../${GTLAB_INSTALL_LIBDIR}")
        else()
            set(_expected_module_rpath "$ORIGIN/../../${GTLAB_INSTALL_LIBDIR}")
        endif()
        if (NOT _module_install_rpath STREQUAL _expected_module_rpath)
            message(FATAL_ERROR
                "Expected relocatable module install RPATH '${_expected_module_rpath}', "
                "got '${_module_install_rpath}'")
        endif()
    endif()

    function(assert_gtlab_rpath_opt_out variable target)
        set(${variable} TRUE)
        set(CMAKE_INSTALL_RPATH "")
        add_executable(${target} EXCLUDE_FROM_ALL external_abi_smoke.cpp)
        gtlab_set_install_rpath(${target} "bin/modules")
        get_target_property(_skip_rpath ${target} INSTALL_RPATH)
        if (_skip_rpath AND NOT _skip_rpath STREQUAL "_skip_rpath-NOTFOUND")
            message(FATAL_ERROR "${variable} did not disable GTlab install RPATH: ${_skip_rpath}")
        endif()
    endfunction()
    assert_gtlab_rpath_opt_out(CMAKE_SKIP_RPATH RpathSkipRpathProbe)
    assert_gtlab_rpath_opt_out(CMAKE_SKIP_INSTALL_RPATH RpathSkipInstallProbe)

    function(assert_gtlab_rpath_override)
        set(CMAKE_SKIP_RPATH FALSE)
        set(CMAKE_SKIP_INSTALL_RPATH FALSE)
        set(CMAKE_INSTALL_RPATH "$ORIGIN/packager-path")
        add_executable(RpathOverrideProbe EXCLUDE_FROM_ALL external_abi_smoke.cpp)
        gtlab_set_install_rpath(RpathOverrideProbe "bin/modules")
        get_target_property(_override_rpath RpathOverrideProbe INSTALL_RPATH)
        if (NOT _override_rpath STREQUAL "$ORIGIN/packager-path")
            message(FATAL_ERROR
                "CMAKE_INSTALL_RPATH override was not preserved: ${_override_rpath}")
        endif()
    endfunction()
    assert_gtlab_rpath_override()
endif()

add_subdirectory(override)

add_test(NAME ExternalModuleInstallLayout
    COMMAND ${CMAKE_COMMAND}
        "-DEXPECTED_BUILD_MODULEDIR=${CMAKE_BINARY_DIR}/modules"
        "-DBUILD_MODULE=$<TARGET_FILE:ExternalAbiSmoke>"
        "-DPREFIX=${CMAKE_INSTALL_PREFIX}"
        "-DCONFIG=$<CONFIG>"
        "-DDEFAULT_MODULEDIR_RELEASE=${GTLAB_INSTALL_MODULEDIR_RELEASE}"
        "-DDEFAULT_MODULEDIR_DEBUG=${GTLAB_INSTALL_MODULEDIR_DEBUG}"
        "-DDEFAULT_MODULE=$<TARGET_FILE_NAME:ExternalAbiSmoke>"
        "-DOVERRIDE_MODULEDIR=consumer-modules"
        "-DOVERRIDE_MODULE=$<TARGET_FILE_NAME:ExternalAbiOverrideSmoke>"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/verify_install.cmake")
