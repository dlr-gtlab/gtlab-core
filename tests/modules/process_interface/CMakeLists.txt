# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

add_gtlab_module(TestProcessInterface MODULE_ID "Test Process Interface"
  SOURCES
    operation/test_operation.h
    calculator/test_calculator.h
    calculator/test_crashonexecutecalculator.h
    calculator/test_mementoprojectcalculator.h
    calculator/test_processstatescalculator.h
    calculator/test_progresscalculator.h
    calculator/test_sleepcalculator.h
    task/test_consoleoverridetask.h
    test_process_interface.h

    operation/test_operation.cpp
    calculator/test_calculator.cpp
    calculator/test_crashonexecutecalculator.cpp
    calculator/test_mementoprojectcalculator.cpp
    calculator/test_processstatescalculator.cpp
    calculator/test_progresscalculator.cpp
    calculator/test_sleepcalculator.cpp
    task/test_consoleoverridetask.cpp
    test_process_interface.cpp
)

target_include_directories(TestProcessInterface PUBLIC calculator .)

target_link_libraries(TestProcessInterface 
    PRIVATE
    GTlab::Logging
    GTlab::Core
    TestDatamodelInterface
    TestMdiInterface
)
