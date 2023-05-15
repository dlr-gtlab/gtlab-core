project(GTlab-Unittests)

cmake_minimum_required(VERSION 3.12)

if (EXISTS ${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
    cmake_policy(SET CMP0091 NEW)
    include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
endif()

project(GTlabUnittests)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if (NOT TARGET Qt5::Core)
    find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
endif()

if (NOT TARGET GTlab::Core)
    find_package(GTlab REQUIRED)
endif()

if (NOT TARGET gtest)
    include(AddGoogleTest)
endif()

FILE (GLOB_RECURSE DM_SRCS datamodel/*.cpp datamodel/*.h datamodel/data/data.qrc)
FILE (GLOB_RECURSE CAL_SRCS calculators/*.cpp calculators/*.h)
FILE (GLOB_RECURSE CORE_SRCS core/*.cpp core/*.h)
FILE (GLOB_RECURSE TP_SRCS thirdparty/*.cpp thirdparty/*.h)
FILE (GLOB_RECURSE EXP_SRCS exporter/*.cpp exporter/*.h)
FILE (GLOB_RECURSE MDI_SRCS mdi/*.cpp mdi/*.h)
FILE (GLOB_RECURSE GUI_SRCS gui/*.cpp gui/*.h)

set(SOURCES 
    ${DM_SRCS}
    ${CAL_SRCS}
    ${CORE_SRCS}
    ${TP_SRCS}
    ${EXP_SRCS}
    ${MDI_SRCS}
    ${GUI_SRCS}
)

set (SOURCES 
    ${SOURCES}
    main.cpp 
    gt_testhelper.cpp 
    slotadaptor.cpp
)

add_executable(GTlabUnitTest ${SOURCES})
set_target_properties(GTlabUnitTest PROPERTIES AUTOMOC ON AUTORCC ON)

target_compile_definitions(GTlabUnitTest PRIVATE GT_MODULE_ID="Unittests" "GT_LOG_USE_QT_BINDINGS")
target_include_directories(GTlabUnitTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GTlabUnitTest PRIVATE
    GTlab::Core
    GTlab::Gui
    GTlab::DataProcessor
    gtest
    mpark::variant
    tl::optional
)
