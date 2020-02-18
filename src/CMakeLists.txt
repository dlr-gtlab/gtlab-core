if (GTLAB_BUILD_CADKERNEL)

    set (OCC_LIBS TKSTEP TKSTEP209 TKSTEPAttr TKSTEPBase
        TKOpenGl TKVRML TKShapeSchema TKPShape TKCDF PTKernel
        TKV3d TKMesh TKHLR TKService
        TKOffset
        TKBool TKXSBase TKShHealing
        TKBO TKPrim TKTopAlgo
        TKGeomAlgo TKBRep TKGeomBase
        TKG3d TKG2d TKMath TKernel
    )

    # we need opencascade for the 3d widget
    find_package(OCE 0.17 REQUIRED COMPONENTS ${OCC_LIBS})

  add_subdirectory(cadkernel/widget)
endif(GTLAB_BUILD_CADKERNEL)

include_directories(${CMINPACK_INCLUDE_DIR})

if (GTLAB_BUILD_UTILITIES)
  add_subdirectory(utilities)
endif (GTLAB_BUILD_UTILITIES)

if (GTLAB_BUILD_DATAMODEL)
  add_subdirectory(datamodel)
endif (GTLAB_BUILD_DATAMODEL)

if (GTLAB_BUILD_VERSIONCONTROL)
    add_subdirectory(versioncontrol)
endif (GTLAB_BUILD_VERSIONCONTROL)

if (GTLAB_BUILD_CONSTRAINTSOLVER)
    add_subdirectory(constraintsolver)
endif (GTLAB_BUILD_CONSTRAINTSOLVER)

if (GTLAB_BUILD_NETWORK)
    add_subdirectory(network)
endif (GTLAB_BUILD_NETWORK)

if (GTLAB_BUILD_CORE)
    add_subdirectory(core)
endif (GTLAB_BUILD_CORE)

if (GTLAB_BUILD_MDI)
    add_subdirectory(mdi)
endif (GTLAB_BUILD_MDI)

if (GTLAB_BUILD_GUI)
    add_subdirectory(gui)
endif (GTLAB_BUILD_GUI)

if (GTLAB_BUILD_CALCULATORS_INTERFACE)
    add_subdirectory(calculators)
endif (GTLAB_BUILD_CALCULATORS_INTERFACE)

if (GTLAB_BUILD_BATCH)
    add_subdirectory(batch)
endif(GTLAB_BUILD_BATCH)

add_subdirectory(modules)

if (GTLAB_BUILD_APP)
    add_subdirectory(app)
endif(GTLAB_BUILD_APP)


