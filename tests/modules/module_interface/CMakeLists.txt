# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

add_library(TestModuleInterface SHARED
    test_module_interface.h
    test_preferencepage.h
    test_module_interface.cpp
    test_preferencepage.cpp
)

target_compile_definitions(TestModuleInterface PRIVATE GT_MODULE_ID="Test Module Interface")

target_link_libraries(TestModuleInterface 
    PRIVATE
    GTlab::Logging
    GTlab::Gui
)