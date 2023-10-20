# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

add_library(TestMdiInterface SHARED
    data/test_mdi_data.h
    data/test_mdi_package.h
    mdi/test_mdi_dockwidget.h
    mdi/test_mdi_packageui.h
    mdi/test_mdi_viewer.h
    test_mdi_interface_exports.h
    test_mdi_interface.h

    data/test_mdi_data.cpp
    data/test_mdi_package.cpp
    mdi/test_mdi_dockwidget.cpp
    mdi/test_mdi_packageui.cpp
    mdi/test_mdi_viewer.cpp
    test_mdi_interface.cpp
)

target_include_directories(TestMdiInterface PUBLIC data mdi .)

target_compile_definitions(TestMdiInterface 
    PRIVATE 
    GT_MODULE_ID="Test Mdi Interface"
    TEST_MDI_INTERFACE_DLL
)

target_link_libraries(TestMdiInterface 
    PRIVATE
    GTlab::Logging
    GTlab::Gui
)