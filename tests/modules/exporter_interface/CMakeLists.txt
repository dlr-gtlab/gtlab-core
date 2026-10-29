# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

add_gtlab_module(TestExporterInterface MODULE_ID "Test Exporter Interface"
  SOURCES
    exporter/test_memento_exporter.h
    exporter/test_memento_exporter_page.h
    exporter_interface_exports.h
    exporter_interface.h

    exporter/test_memento_exporter.cpp
    exporter/test_memento_exporter_page.cpp
    exporter_interface.cpp
)

target_include_directories(TestExporterInterface PUBLIC exporter .)

target_compile_definitions(TestExporterInterface
    PRIVATE
    TEST_EXPORTER_INTERFACE_DLL
)

target_link_libraries(TestExporterInterface
    PRIVATE
    GTlab::Logging
    GTlab::Gui
    TestDatamodelInterface
)
