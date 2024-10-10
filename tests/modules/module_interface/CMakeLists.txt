# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

add_gtlab_module(TestModuleInterface MODULE_ID "Test Module Interface"
  SOURCES
    test_module_interface.h
    test_preferencepage.h
    test_module_interface.cpp
    test_preferencepage.cpp
)

target_link_libraries(TestModuleInterface 
    PRIVATE
    GTlab::Logging
    GTlab::Gui
)