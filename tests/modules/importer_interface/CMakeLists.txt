# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

add_gtlab_module(TestImporterInterface MODULE_ID "Test Importer Interface"
  SOURCES
    importer/test_memento_append_importer.h
    importer/test_memento_append_importer_page.h
    importer/test_memento_restore_importer.h
    importer/test_memento_restore_importer_page.h
    importer_interface_exports.h
    importer_interface.h

    importer/test_memento_append_importer.cpp
    importer/test_memento_append_importer_page.cpp
    importer/test_memento_restore_importer.cpp
    importer/test_memento_restore_importer_page.cpp
    importer_interface.cpp
)

target_include_directories(TestImporterInterface PUBLIC importer .)

target_compile_definitions(TestImporterInterface
    PRIVATE
    TEST_IMPORTER_INTERFACE_DLL
)

target_link_libraries(TestImporterInterface
    PRIVATE
    GTlab::Logging
    GTlab::Gui
    TestDatamodelInterface
)
