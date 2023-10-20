# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

cmake_minimum_required(VERSION 3.12)
project(GTlab-Conan-TestPackage)

find_package(GTlab REQUIRED)

add_executable(dataprocessor_test test_dataprocessor.cpp)
target_compile_definitions(dataprocessor_test PRIVATE GT_MODULE_ID="testDP")
target_link_libraries(dataprocessor_test PRIVATE GTlab::DataProcessor)

add_executable(core_test test_core.cpp)
target_compile_definitions(core_test PRIVATE GT_MODULE_ID="testCore")
target_link_libraries(core_test PRIVATE GTlab::Core)

add_executable(gui_test test_gui.cpp)
target_compile_definitions(gui_test PRIVATE GT_MODULE_ID="testGui")
target_link_libraries(gui_test PRIVATE GTlab::Gui)