# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

set(GTLAB_INSTALL_MODULEDIR "consumer-modules")
add_gtlab_module(ExternalAbiOverrideSmoke
    MODULE_ID "External ABI Override Smoke Test"
    SOURCES "${CMAKE_CURRENT_LIST_DIR}/../external_abi_smoke.cpp"
)
target_link_libraries(ExternalAbiOverrideSmoke PRIVATE GTlab::Core)
target_compile_definitions(ExternalAbiOverrideSmoke
    PRIVATE GTLAB_ABI_TEST_NO_ADJACENT_DEPENDENCY)
