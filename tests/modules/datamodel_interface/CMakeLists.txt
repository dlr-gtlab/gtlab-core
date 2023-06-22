# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

add_library(TestDatamodelInterface SHARED
    data/test_dmi_data.h
    data/test_dmi_externalobject.h
    test_datamodel_interface_exports.h 
    test_datamodel_interface.h 
    data/test_dmi_package.h

    data/test_dmi_data.cpp
    data/test_dmi_externalobject.cpp
    test_datamodel_interface.cpp 
    data/test_dmi_package.cpp
)

target_include_directories(TestDatamodelInterface PUBLIC data .)

target_compile_definitions(TestDatamodelInterface 
    PRIVATE 
    GT_MODULE_ID="Test Datamodel Interface"
    TEST_DATAMODEL_INTERFACE_DLL
)

target_link_libraries(TestDatamodelInterface 
    PRIVATE
    GTlab::Logging
    GTlab::Core
)