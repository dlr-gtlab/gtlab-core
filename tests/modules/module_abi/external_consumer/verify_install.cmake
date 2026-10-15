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
