# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

cmake_minimum_required(VERSION 3.12)
project(GTlabExternalAbiConsumer LANGUAGES CXX)

find_package(GTlab CONFIG REQUIRED)

if (NOT GTLAB_CORE_VERSION OR NOT GTLAB_MODULE_ABI)
    message(FATAL_ERROR "The installed GTlab package must export version and ABI")
endif()

add_gtlab_module(ExternalAbiSmoke
    MODULE_ID "External ABI Smoke Test"
    SOURCES external_abi_smoke.cpp
)
target_link_libraries(ExternalAbiSmoke PRIVATE GTlab::Core)
