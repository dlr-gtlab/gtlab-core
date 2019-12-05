#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __
#          / /_/ / / / / / /_/ / /_/ /
#         ____/ /_/ /_/\__,_/_.___/

include_directories(
    .
    ${CMAKE_CURRENT_SOURCE_DIR}/../datamodel
    ${CMAKE_CURRENT_SOURCE_DIR}/../calculators
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../mdi
    ${CMAKE_CURRENT_SOURCE_DIR}/../utilities/logging
    ${CMAKE_CURRENT_SOURCE_DIR}/../utilities/numerics
    ${CMAKE_CURRENT_SOURCE_DIR}/../utilities/numerics/bspline
    ${CMAKE_CURRENT_SOURCE_DIR}/../datamodel/property
)

set(headers
    batchremote.h
)

set(sources
    batch.cpp
    batchremote.cpp
)

if (WIN32)
    set(sources ${sources} batch.rc)
endif(WIN32)

# add search paths to shared libraries
# unix: QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN:\$$ORIGIN/modules\''

add_executable(GTlabConsole  ${sources} ${headers})
target_link_libraries(GTlabConsole
    Qt5::Xml
    Qt5::Widgets
    Qt5::Network
    GTlabNumerics
    GTlabPhysics
    GTlabLogging
    GTlabDatamodel
    GTlabNetwork
    GTlabCalculators
    GTlabVersionControl
    GTlabCore
    GTlabMdi
    ${QWT_LIBRARY}
)

install(TARGETS GTlabConsole DESTINATION ${CMAKE_INSTALL_BINDIR})

######################################################################
