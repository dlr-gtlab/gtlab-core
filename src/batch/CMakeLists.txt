# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __
#          / /_/ / / / / / /_/ / /_/ /
#         ____/ /_/ /_/\__,_/_.___/

set(headers
    batchremote.h
    gt_consoleparser.h
    gt_remoteprocessrunner.h
    gt_remoteprocessrunnerstates.h
    gt_consolerunprocess.h
    gt_consoleupgradeproject.h
)

set(sources
    batch.cpp
    batchremote.cpp
    gt_consoleparser.cpp
    gt_remoteprocessrunner.cpp
    gt_remoteprocessrunnerstates.cpp
    gt_consolerunprocess.cpp
    gt_consoleupgradeproject.cpp
)

if (WIN32)
    set(sources ${sources} batch.rc)
endif(WIN32)



add_executable(GTlabConsole  ${sources} ${headers})
target_compile_definitions(GTlabConsole
    PRIVATE GT_MODULE_ID="Batch" GT_EXEC_ID="Exec"
    PRIVATE QT_DEPRECATED_WARNINGS
)

target_link_libraries(GTlabConsole
    PRIVATE
    GTlabCore
    GTlabDataProcessor
    GTlabGui
    Qt5::Widgets
    Qt5::Xml
)

install(TARGETS GTlabConsole DESTINATION ${CMAKE_INSTALL_BINDIR})

######################################################################
