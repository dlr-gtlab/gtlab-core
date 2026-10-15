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
gtlab_standard_setup()

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

add_library(ExternalAbiAdjacentDependency SHARED external_abi_dependency.cpp)
set_target_properties(ExternalAbiAdjacentDependency PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    LIBRARY_OUTPUT_DIRECTORY "${GTLAB_BUILD_MODULEDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${GTLAB_BUILD_MODULEDIR}"
)
foreach(_config IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER "${_config}" _config_upper)
    set_target_properties(ExternalAbiAdjacentDependency PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY_${_config_upper} "${GTLAB_BUILD_MODULEDIR}"
        RUNTIME_OUTPUT_DIRECTORY_${_config_upper} "${GTLAB_BUILD_MODULEDIR}"
    )
endforeach()
target_link_libraries(ExternalAbiSmoke PRIVATE
    GTlab::Core ExternalAbiAdjacentDependency)

install(TARGETS ExternalAbiAdjacentDependency
    LIBRARY DESTINATION "${GTLAB_INSTALL_MODULEDIR}"
    RUNTIME DESTINATION "${GTLAB_INSTALL_MODULEDIR}"
)

if (APPLE OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if (NOT CMAKE_SKIP_RPATH AND NOT CMAKE_SKIP_INSTALL_RPATH)
        get_target_property(_module_install_rpath ExternalAbiSmoke INSTALL_RPATH)
        if (DEFINED CMAKE_INSTALL_RPATH AND NOT "${CMAKE_INSTALL_RPATH}" STREQUAL "")
            set(_expected_module_rpath "${CMAKE_INSTALL_RPATH}")
        elseif (APPLE)
            set(_expected_module_rpath "@loader_path;@loader_path/../../${GTLAB_INSTALL_LIBDIR}")
        else()
            set(_expected_module_rpath "$ORIGIN;$ORIGIN/../../${GTLAB_INSTALL_LIBDIR}")
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
        gtlab_set_install_rpath(${target} RELEASE_DIR "bin/modules")
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
        gtlab_set_install_rpath(RpathOverrideProbe RELEASE_DIR "bin/modules")
        get_target_property(_override_rpath RpathOverrideProbe INSTALL_RPATH)
        if (NOT _override_rpath STREQUAL "$ORIGIN/packager-path")
            message(FATAL_ERROR
                "CMAKE_INSTALL_RPATH override was not preserved: ${_override_rpath}")
        endif()
    endfunction()
    assert_gtlab_rpath_override()
endif()

add_subdirectory(override)

if (WIN32)
    set(_gtlab_console_name "GTlabConsole.exe")
else()
    set(_gtlab_console_name "GTlabConsole")
endif()
set(_installed_gtlab_console
    "${CMAKE_INSTALL_PREFIX}/$<IF:$<CONFIG:Debug>,${GTLAB_INSTALL_DEBUG_BINDIR},${GTLAB_INSTALL_BINDIR}>/${_gtlab_console_name}")

set(_test_environment)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(_runtime_library_targets GTlabLogging GenH5)
    get_target_property(_genh5_link_libraries GenH5 INTERFACE_LINK_LIBRARIES)
    foreach(_dependency IN LISTS _genh5_link_libraries)
        if (TARGET "${_dependency}")
            get_target_property(_dependency_type "${_dependency}" TYPE)
            if (NOT _dependency_type STREQUAL "INTERFACE_LIBRARY")
                list(APPEND _runtime_library_targets "${_dependency}")
            endif()
        endif()
    endforeach()

    set(_runtime_library_dirs)
    foreach(_target IN LISTS _runtime_library_targets)
        list(APPEND _runtime_library_dirs "$<TARGET_FILE_DIR:${_target}>")
    endforeach()
    list(JOIN _runtime_library_dirs ":" _runtime_library_path)
    list(APPEND _test_environment
        "LD_LIBRARY_PATH=${_runtime_library_path}:$ENV{LD_LIBRARY_PATH}")
endif()

add_test(NAME ExternalModuleInstallLayout
    COMMAND ${CMAKE_COMMAND}
        -E env ${_test_environment}
        ${CMAKE_COMMAND}
        "-DEXPECTED_BUILD_MODULEDIR=${CMAKE_BINARY_DIR}/modules"
        "-DBUILD_MODULE=$<TARGET_FILE:ExternalAbiSmoke>"
        "-DPREFIX=${CMAKE_INSTALL_PREFIX}"
        "-DCONFIG=$<CONFIG>"
        "-DDEFAULT_MODULEDIR_RELEASE=${GTLAB_INSTALL_MODULEDIR_RELEASE}"
        "-DDEFAULT_MODULEDIR_DEBUG=${GTLAB_INSTALL_MODULEDIR_DEBUG}"
        "-DDEFAULT_MODULE=$<TARGET_FILE_NAME:ExternalAbiSmoke>"
        "-DOVERRIDE_MODULEDIR=consumer-modules"
        "-DOVERRIDE_MODULE=$<TARGET_FILE_NAME:ExternalAbiOverrideSmoke>"
        "-DADJACENT_DEPENDENCY=$<TARGET_FILE_NAME:ExternalAbiAdjacentDependency>"
        "-DCONSOLE=${_installed_gtlab_console}"
        "-DINSTALLED_MODULE=${CMAKE_INSTALL_PREFIX}/$<IF:$<CONFIG:Debug>,${GTLAB_INSTALL_MODULEDIR_DEBUG},${GTLAB_INSTALL_MODULEDIR_RELEASE}>/$<TARGET_FILE_NAME:ExternalAbiSmoke>"
        "-DLOAD_MARKER=${CMAKE_BINARY_DIR}/external-module-loaded.txt"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/verify_install.cmake")
