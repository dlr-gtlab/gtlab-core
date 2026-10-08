# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

set(ABI_TEST_MODULE_SOURCES
    abi_test_module.cpp
    abi_test_module.h
)
set(ABI_TEST_PLUGIN_DIRECTORY
    "${PROJECT_BINARY_DIR}/tests/modules/module_abi/plugins")

add_library(AbiAdjacentDependency SHARED adjacent_dependency.cpp)
set_target_properties(AbiAdjacentDependency PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON)

function(set_abi_test_output_directory target)
    set_target_properties(${target} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${ABI_TEST_PLUGIN_DIRECTORY}"
        RUNTIME_OUTPUT_DIRECTORY "${ABI_TEST_PLUGIN_DIRECTORY}"
    )
    foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
        set_target_properties(${target} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY_${config} "${ABI_TEST_PLUGIN_DIRECTORY}"
            RUNTIME_OUTPUT_DIRECTORY_${config} "${ABI_TEST_PLUGIN_DIRECTORY}"
        )
    endforeach()
endfunction()

set_abi_test_output_directory(AbiAdjacentDependency)

add_gtlab_module(AbiTestModule
    MODULE_ID "ABI Test Module"
    SOURCES ${ABI_TEST_MODULE_SOURCES}
)
target_link_libraries(AbiTestModule PRIVATE GTlab::Core AbiAdjacentDependency)
set_target_properties(AbiTestModule PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "$ORIGIN")
set_abi_test_output_directory(AbiTestModule)

function(add_abi_test_variant target filename_abi metadata_abi)
    add_gtlab_module(${target}
        MODULE_ID "ABI Test Module"
        SOURCES ${ABI_TEST_MODULE_SOURCES}
    )
    target_link_libraries(${target} PRIVATE GTlab::Core AbiAdjacentDependency)
    set_abi_test_output_directory(${target})
    set_target_properties(${target} PROPERTIES
        OUTPUT_NAME "${target}.gtm.${filename_abi}"
    )
    set_property(TARGET ${target} PROPERTY AUTOMOC_MOC_OPTIONS
        "-Mgtlab_core_version=${GTLAB_CORE_VERSION}"
        "-Mgtlab_module_abi=${metadata_abi}"
    )
endfunction()

add_abi_test_variant(AbiTestModuleIncompatible "2.0" "2.0")

add_library(AbiTestModuleMissing SHARED ${ABI_TEST_MODULE_SOURCES})
target_compile_definitions(AbiTestModuleMissing PRIVATE
    GT_MODULE_ID="ABI Test Module")
target_link_libraries(AbiTestModuleMissing PRIVATE
    GTlab::Core AbiAdjacentDependency)
set_target_properties(AbiTestModuleMissing PROPERTIES
    AUTOMOC ON
    PREFIX ""
    DEBUG_POSTFIX ""
    OUTPUT_NAME "AbiTestModuleMissing.gtm.${GTLAB_MODULE_ABI}"
)
set_abi_test_output_directory(AbiTestModuleMissing)

add_executable(ModuleMetadataInspector metadata_inspector.cpp)
target_link_libraries(ModuleMetadataInspector PRIVATE
    Qt${QT_VERSION_MAJOR}::Core)
