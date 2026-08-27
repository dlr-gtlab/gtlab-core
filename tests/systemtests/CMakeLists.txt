# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

find_package(Python3 COMPONENTS Interpreter QUIET)

if (NOT Python3_Interpreter_FOUND)
    message(WARNING "Pytest system tests are disabled: Python 3 was not found.")
else()
    execute_process(
        COMMAND ${Python3_EXECUTABLE} -c "import pytest"
        RESULT_VARIABLE PYTEST_IMPORT_RESULT
        OUTPUT_QUIET
        ERROR_QUIET
    )

    if (PYTEST_IMPORT_RESULT EQUAL 0)
        add_test(
            NAME SystemTest.RunTaskFromMemento.Test1
            COMMAND ${Python3_EXECUTABLE} -m pytest
                    ${CMAKE_CURRENT_SOURCE_DIR}/run_task_from_memento_tests/test_1/test_run_task_from_memento.py
        )

        set_tests_properties(SystemTest.RunTaskFromMemento.Test1 PROPERTIES
            ENVIRONMENT "GTLAB_CONSOLE=$<TARGET_FILE:GTlabConsole>;QT_QPA_PLATFORM=offscreen"
            LABELS "SystemTest;Batch"
        )
    else()
        message(WARNING
            "Pytest system tests are disabled: install dependencies from "
            "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt."
        )
    endif()
endif()

add_custom_target(systemtests
    DEPENDS
        GTlabConsole
        TestDatamodelInterface
        TestMdiInterface
        TestProcessInterface
)
