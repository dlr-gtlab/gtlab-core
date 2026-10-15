# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

get_filename_component(_build_module_dir "${BUILD_MODULE}" DIRECTORY)
get_filename_component(_expected_build_module_dir
    "${EXPECTED_BUILD_MODULEDIR}" ABSOLUTE)
if (NOT _build_module_dir STREQUAL _expected_build_module_dir)
    message(FATAL_ERROR
        "Module output directory '${_build_module_dir}' does not match '${_expected_build_module_dir}'")
endif()

if (CONFIG STREQUAL "Debug")
    set(_default_moduledir "${DEFAULT_MODULEDIR_DEBUG}")
else()
    set(_default_moduledir "${DEFAULT_MODULEDIR_RELEASE}")
endif()

foreach(_module_spec
        "${_default_moduledir}|${DEFAULT_MODULE}"
        "${OVERRIDE_MODULEDIR}|${OVERRIDE_MODULE}")
    string(REPLACE "|" ";" _module_parts "${_module_spec}")
    list(GET _module_parts 0 _module_dir)
    list(GET _module_parts 1 _module_name)
    if (NOT EXISTS "${PREFIX}/${_module_dir}/${_module_name}")
        message(FATAL_ERROR
            "Expected installed module '${PREFIX}/${_module_dir}/${_module_name}'")
    endif()
endforeach()

get_filename_component(_installed_module_dir "${INSTALLED_MODULE}" DIRECTORY)
set(_adjacent_dependency "${_installed_module_dir}/${ADJACENT_DEPENDENCY}")
if (NOT EXISTS "${_adjacent_dependency}")
    message(FATAL_ERROR "Expected adjacent module dependency '${_adjacent_dependency}'")
endif()

file(REMOVE "${LOAD_MARKER}")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "QT_QPA_PLATFORM=offscreen"
        "GTLAB_ABI_TEST_MARKER=${LOAD_MARKER}"
        "${CONSOLE}" load_module "${INSTALLED_MODULE}"
    RESULT_VARIABLE _load_result
    OUTPUT_VARIABLE _load_stdout
    ERROR_VARIABLE _load_stderr
)
if (NOT _load_result EQUAL 0)
    message(FATAL_ERROR
        "Could not load installed module '${INSTALLED_MODULE}' with its adjacent dependency.\n"
        "stdout: ${_load_stdout}\nstderr: ${_load_stderr}")
endif()
if (NOT EXISTS "${LOAD_MARKER}")
    message(FATAL_ERROR "The installed module did not execute its load marker")
endif()
file(READ "${LOAD_MARKER}" _load_marker_contents)
if (NOT _load_marker_contents STREQUAL "1621")
    message(FATAL_ERROR
        "The installed module did not call its adjacent dependency: ${_load_marker_contents}")
endif()
