#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __
#          / /_/ / / / / / /_/ / /_/ /
#         ____/ /_/ /_/\__,_/_.___/

set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)

add_definitions (-DGT_GUI_DLL)

include_directories(
    .
    widgets
    preferences
    preferences/pages
    project
    project/pages
    dialogs
    tools
    ${CMAKE_CURRENT_SOURCE_DIR}/../datamodel
    ${CMAKE_CURRENT_SOURCE_DIR}/../calculators
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/settings
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/provider
    ${CMAKE_CURRENT_SOURCE_DIR}/../mdi
    ${CMAKE_CURRENT_SOURCE_DIR}/../mdi/tools
    ${CMAKE_CURRENT_SOURCE_DIR}/../mdi/object_ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../mdi/mdi_items/map_editor
    ${CMAKE_CURRENT_SOURCE_DIR}/../mdi/mdi_items/memento_viewer
    ${CMAKE_CURRENT_SOURCE_DIR}/../utilities/logging
    ${CMAKE_CURRENT_SOURCE_DIR}/../datamodel/property
    ${CMAKE_CURRENT_SOURCE_DIR}/../network
    ${CMAKE_CURRENT_SOURCE_DIR}/../mdi/mdi_items/collection_env
    ${CMAKE_CURRENT_BINARY_DIR}/
)

file (GLOB_RECURSE sources
    "*.h"
    "*.cpp"
)

set (resources
    ${CMAKE_CURRENT_SOURCE_DIR}/../resources/icons/icons.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/../resources/pixmaps/pixmaps.qrc
)

file (GLOB_RECURSE forms
    "*.ui"
)
qt5_wrap_ui(ui_src ${forms})

add_library(GTlabGui SHARED  ${sources} ${resources} ${ui_src})
target_link_libraries(GTlabGui
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

install(TARGETS GTlabGui
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

######################################################################
