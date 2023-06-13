# This function set some variables that are common
# to the gtlab build system.
# It makes sure, that libraries are put into the root build folder,
# sets the debug postfix "-d" and sets the "binDebug" folder in
# Debug mode
#
# usage:
#   gtlab_standard_setup()
macro(gtlab_standard_setup)
    message("Setting up GTlab standards configuration")
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    # store all dlls inside build directory
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR})


    if(NOT DEFINED CMAKE_INSTALL_BINDIR)
        set(CMAKE_INSTALL_BINDIR "$<$<CONFIG:DEBUG>:binDebug>$<$<NOT:$<CONFIG:DEBUG>>:bin>")
    endif(NOT DEFINED CMAKE_INSTALL_BINDIR)

    if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
        set(CMAKE_INSTALL_LIBDIR "lib")
    endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)

    if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
        set(CMAKE_DEBUG_POSTFIX "-d")
    endif()

    # Instruct CMake to run moc automatically when needed.
    set(CMAKE_AUTOMOC ON)
endmacro()

# This function adds the packages of GTlab's devtools
# to the cmake prefix path. It adds the GTLAB_DEVTOOLS_DIR
# cache variable, to set the path to GTlab's devtools
#
# Usage:
#   enable_gtlab_devtools()
macro(enable_gtlab_devtools)
    # prefix 3rdparty stuff from devtools
    set(GTLAB_DEVTOOLS_DIR "" CACHE PATH "Path of gtlab devtools")

    if (EXISTS ${GTLAB_DEVTOOLS_DIR})
        message("GTlab DevTools enabled at " ${GTLAB_DEVTOOLS_DIR})

        set (CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}
            ${GTLAB_DEVTOOLS_DIR}
            ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/BladeGenInterface
            ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/GoogleTest
            ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/hdf5
            ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/LibXML
            ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/minpack
            ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/NLopt
            ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/Qwt
            ${GTLAB_DEVTOOLS_DIR}/ThirdPartyLibraries/SplineLib
        )
    endif()
	
endmacro()

# Function to add a gtlab module
#
# This makes sure to add a module id and deploy to the correct
# folders
#
# Usage:
#   add_gtlab_module(mymodule
#     SOURCES mod.cpp mod2.cpp mod.h
#     MODULE_ID "my mod"
#   )
function(add_gtlab_module GTLAB_ADD_MODULE_TARGET)
  cmake_parse_arguments(
    PARSE_ARGV 1 GTLAB_ADD_MODULE "" "MODULE_ID"
    "SOURCES")

  if (NOT DEFINED GTLAB_ADD_MODULE_MODULE_ID)
      message(FATAL_ERROR "In add_gtlab_module: Missing argument MODULE_ID for target ${GTLAB_ADD_MODULE_TARGET}")
  endif()

  add_library(${GTLAB_ADD_MODULE_TARGET} SHARED ${GTLAB_ADD_MODULE_SOURCES})

  # add module id
  target_compile_definitions(${GTLAB_ADD_MODULE_TARGET}
    PRIVATE -DGT_MODULE_ID="${GTLAB_ADD_MODULE_MODULE_ID}")

  # set rpath for linux
  if (UNIX)
      set_target_properties(${GTLAB_ADD_MODULE_TARGET}
          PROPERTIES INSTALL_RPATH "$ORIGIN:$ORIGIN/.."
                     INSTALL_RPATH_USE_LINK_PATH FALSE)
  endif (UNIX)

  if (IS_DIRECTORY ${CMAKE_INSTALL_PREFIX}/build)
    message(STATUS "Deploying into GTlab build dir")

    if (UNIX)
        install (TARGETS ${GTLAB_ADD_MODULE_TARGET}
            LIBRARY DESTINATION build/modules
        )

    else(UNIX)
        install (TARGETS ${GTLAB_ADD_MODULE_TARGET}
            RUNTIME DESTINATION build/modules
        )
    endif(UNIX)
  else ()
    if (UNIX)
        install (TARGETS ${GTLAB_ADD_MODULE_TARGET}
            LIBRARY DESTINATION
            $<IF:$<CONFIG:Debug>,binDebug/modules,bin/modules>
        )
    else(UNIX)
        install (TARGETS ${GTLAB_ADD_MODULE_TARGET}
            RUNTIME DESTINATION
            $<IF:$<CONFIG:Debug>,binDebug/modules,bin/modules>
        )
    endif(UNIX)
  endif ()
endfunction()
