# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

# Versions of the bundled libraries
# If you like to upgrade, just change the number
set(GTLAB_LOGGING_VERSION "master")
set(GTLAB_GENH5_VERSION "132-prepare-qt6-compatibility")

if (EXISTS ${GTLAB_DEVTOOLS_DIR})
    return()
endif()


option(GTLAB_USE_BUNDLED_LOGGING "Build logging library bundled with GTlab" ON)
option(GTLAB_USE_BUNDLED_GENH5 "Build genh5 (hdf5 c++ wrapper) bundled with GTlab" ON)

include(CPM)

cmake_policy(SET CMP0097 NEW) # empty submodules list -> don't download these

if (BUILD_WITH_HDF5 AND GTLAB_USE_BUNDLED_GENH5)
    CPMAddPackage(
      NAME genh5
      GIT_REPOSITORY "https://github.com/dlr-gtlab/genh5.git"
      GIT_TAG ${GTLAB_GENH5_VERSION}
    )

    set_target_properties(GenH5
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}")
endif()


if (GTLAB_USE_BUNDLED_LOGGING)
    CPMAddPackage(
      NAME logging
      GIT_REPOSITORY "https://github.com/dlr-gtlab/gt-logging.git"
      GIT_TAG ${GTLAB_LOGGING_VERSION}
      OPTIONS "BUILD_UNITTESTS OFF"
    )

    set_target_properties(GTlabLogging
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}")
endif()
