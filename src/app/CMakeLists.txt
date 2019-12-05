#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __
#          / /_/ / / / / / /_/ / /_/ /
#         ____/ /_/ /_/\__,_/_.___/

include_directories(
    .
    ${CMAKE_CURRENT_SOURCE_DIR}/../utilities/logging
    ${CMAKE_CURRENT_SOURCE_DIR}/../datamodel
    ${CMAKE_CURRENT_SOURCE_DIR}/../calculators
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../mdi
    ${CMAKE_CURRENT_SOURCE_DIR}/../gui
)

set(headers
)

set(sources
    app.cpp
)

if (WIN32)
    set(sources ${sources} app.rc)
    set(TYPE WIN32)
endif(WIN32)


# add search paths to shared libraries
# unix: QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN:\$$ORIGIN/modules\''

add_executable(GTlab ${TYPE} ${sources} ${headers})
target_link_libraries(GTlab
    Qt5::Widgets
    GTlabNumerics
    GTlabPhysics
    GTlabLogging
    GTlabDatamodel
    GTlabNetwork
    GTlabCalculators
    GTlabVersionControl
    GTlabCore
    GTlabMdi
    GTlabGui
    ${QWT_LIBRARY}
)

install(TARGETS GTlab DESTINATION ${CMAKE_INSTALL_BINDIR})

include(fixup-install)

######################################################################
