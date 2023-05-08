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