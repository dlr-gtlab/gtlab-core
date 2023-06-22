# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

add_library(TestMdiInterfaceExt SHARED
    mdi/test_mdi_ext_externalobjectui.h
    mdi/test_mdi_ext_externalobjectviewer.h
    mdi/test_mdi_ext_packageui.h
    data/test_mdi_ext_package.h
    mdi/test_mdi_ext_viewer.h
    test_mdi_interface_ext.h

    mdi/test_mdi_ext_externalobjectui.cpp
    mdi/test_mdi_ext_externalobjectviewer.cpp
    mdi/test_mdi_ext_packageui.cpp
    data/test_mdi_ext_package.cpp
    mdi/test_mdi_ext_viewer.cpp
    test_mdi_interface_ext.cpp
)

target_include_directories(TestMdiInterfaceExt PRIVATE data mdi .)

target_compile_definitions(TestMdiInterfaceExt 
    PRIVATE 
    GT_MODULE_ID="Test Mdi Interface Ext"
)

target_link_libraries(TestMdiInterfaceExt 
    PRIVATE
    GTlab::Logging
    GTlab::Gui
    TestMdiInterface
    TestDatamodelInterface
)