# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+


add_gtlab_module(TestMdiInterfaceExt MODULE_ID "Test Mdi Interface Ext"
  SOURCES
    mdi/test_mdi_ext_externalobjectui.h
    mdi/test_mdi_ext_externalobjectviewer.h
    mdi/test_mdi_ext_packageui.h
    data/test_mdi_ext_package.h
    mdi/test_mdi_ext_viewer.h
    mdi/test_gui_item_multiple.h
    mdi/test_gui_item_reopen.h
    mdi/test_gui_item_bound.h
    mdi/test_gui_item_customid.h
    mdi/test_gui_item_nodata.h
    mdi/test_gui_flags_dock.h
    test_mdi_interface_ext.h

    mdi/test_mdi_ext_externalobjectui.cpp
    mdi/test_mdi_ext_externalobjectviewer.cpp
    mdi/test_mdi_ext_packageui.cpp
    data/test_mdi_ext_package.cpp
    mdi/test_mdi_ext_viewer.cpp
    mdi/test_gui_item_multiple.cpp
    mdi/test_gui_item_reopen.cpp
    mdi/test_gui_item_bound.cpp
    mdi/test_gui_item_customid.cpp
    mdi/test_gui_item_nodata.cpp
    mdi/test_gui_flags_dock.cpp
    test_mdi_interface_ext.cpp
)

target_include_directories(TestMdiInterfaceExt PRIVATE data mdi .)

target_link_libraries(TestMdiInterfaceExt 
    PRIVATE
    GTlab::Logging
    GTlab::Gui
    TestMdiInterface
    TestDatamodelInterface
)