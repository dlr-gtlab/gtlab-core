# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

if (NOT TARGET mpark::variant)
    add_library(mparkvariant INTERFACE)
    add_library(mpark::variant ALIAS mparkvariant)
    target_include_directories(mparkvariant INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty>
        $<INSTALL_INTERFACE:include/core/thirdparty>)

    install(TARGETS mparkvariant EXPORT GTlabTargets)
endif()

if (NOT TARGET tl::optional)
    add_library(tloptional INTERFACE)
    add_library(tl::optional ALIAS tloptional)
    target_include_directories(tloptional INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty>
        $<INSTALL_INTERFACE:include/core/thirdparty>)

    install(TARGETS tloptional EXPORT GTlabTargets)
endif()


set(HEADERS_INTERNAL
    internal/gt_externalizedobjectprivate.h
    internal/varianthasher.h
)

set(HEADERS
    gt_abstractexternalizationinterface.h
    gt_exceptions.h
    gt_datamodel_exports.h
    gt_eventloop.h
    gt_externalizationmanager.h
    gt_externalizedobject.h
    gt_externalizedobjectfetcher.h
    gt_h5externalizationinterface.h
    gt_h5externalizehelper.h
    gt_object.h
    gt_objectfactory.h
    gt_abstractobjectfactory.h
    gt_objectmemento.h
    gt_objectio.h
    gt_objectgroup.h
    gt_package.h
    gt_platform.h
    gt_factorygroup.h
    gt_finally.h
    gt_qtutilities.h
    gt_typetraits.h
    gt_utilities.h
    gt_variantconvert.h
    gt_version.h
    gt_xmlexpr.h
    gt_xmlutilities.h
    property/gt_enumproperty.h
    property/gt_objectlinkproperty.h
    gt_objectmementodiff.h
    property/gt_abstractproperty.h
    property/gt_boolproperty.h
    property/gt_doubleproperty.h
    property/gt_intproperty.h
    property/gt_polyvector.h
    property/gt_property.h
    property/gt_propertyreference.h
    property/gt_stringproperty.h
    property/gt_unit.h
    property/gt_unitconverter.h
    property/gt_groupproperty.h
    property/gt_modetypeproperty.h
    property/gt_modeproperty.h
    property/gt_variantproperty.h
    property/gt_structproperty.h
    property/gt_propertystructcontainer.h
    property/gt_colorproperty.h
    gt_label.h
    gt_labeldata.h
    property/gt_labelproperty.h
    gt_objectmodel.h
    property/gt_filechooserproperty.h
    gt_propertyfactory.h
    property/gt_propertyconnection.h
    gt_propertyfactory.h
    property/gt_openfilenameproperty.h
    property/gt_savefilenameproperty.h
    property/gt_existingdirectoryproperty.h
    property/gt_objectpathproperty.h
    gt_objectpath.h
    property/gt_relativeobjectlinkproperty.h
    property/gt_doublelistproperty.h
    gt_regexp.h
    gt_algorithms.h
    gt_mpl.h
    gt_versionnumber.h
)

set(SOURCES
    gt_abstractexternalizationinterface.cpp
    gt_exceptions.cpp
    gt_eventloop.cpp
    gt_externalizationmanager.cpp
    gt_externalizedobject.cpp
    gt_h5externalizationinterface.cpp
    gt_h5externalizehelper.cpp
    gt_init_dp.cpp
    gt_object.cpp
    gt_objectfactory.cpp
    gt_abstractobjectfactory.cpp
    gt_objectmemento.cpp
    gt_objectio.cpp
    gt_objectgroup.cpp
    gt_package.cpp
    gt_factorygroup.cpp
    gt_xmlutilities.cpp
    internal/varianthasher.cpp
    property/gt_objectlinkproperty.cpp
    gt_objectmementodiff.cpp
    property/gt_abstractproperty.cpp
    property/gt_boolproperty.cpp
    property/gt_doubleproperty.cpp
    property/gt_intproperty.cpp
    property/gt_propertyreference.cpp
    property/gt_stringproperty.cpp
    property/gt_unit.cpp
    property/gt_groupproperty.cpp
    property/gt_modetypeproperty.cpp
    property/gt_modeproperty.cpp
    property/gt_variantproperty.cpp
    property/gt_structproperty.cpp
    property/gt_propertystructcontainer.cpp
    property/gt_colorproperty.h
    gt_label.cpp
    gt_labeldata.cpp
    property/gt_labelproperty.cpp
    gt_objectmodel.cpp
    property/gt_filechooserproperty.cpp
    gt_propertyfactory.cpp
    property/gt_propertyconnection.cpp
    property/gt_openfilenameproperty.cpp
    property/gt_savefilenameproperty.cpp
    property/gt_existingdirectoryproperty.cpp
    property/gt_objectpathproperty.cpp
    gt_objectpath.cpp
    property/gt_relativeobjectlinkproperty.cpp
    property/gt_doublelistproperty.cpp
    gt_versionnumber.cpp
    gt_regexp.cpp
)

add_library(GTlabDataProcessor ${SOURCES} ${HEADERS} ${HEADERS_INTERNAL})
add_library(GTlab::DataProcessor ALIAS GTlabDataProcessor)

target_compile_definitions(GTlabDataProcessor PUBLIC GT_LOG_USE_QT_BINDINGS)

if (BUILD_SHARED_LIBS)
  target_compile_definitions(GTlabDataProcessor PRIVATE GT_DATAMODEL_DLL)
else(BUILD_SHARED_LIBS)
  target_compile_definitions(GTlabDataProcessor PUBLIC GT_DATAMODEL_LIB)
endif(BUILD_SHARED_LIBS)

target_include_directories(GTlabDataProcessor
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/property>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/core>
)

target_link_libraries(GTlabDataProcessor
    PUBLIC
      Qt5::Core
      Qt5::Xml
      GTlab::Logging
    PRIVATE
      mpark::variant
      tl::optional
)

if (BUILD_WITH_HDF5)
    target_compile_definitions(GTlabDataProcessor PUBLIC GT_H5)
    target_link_libraries(GTlabDataProcessor
        PUBLIC
        GenH5
    )
endif(BUILD_WITH_HDF5)

set_target_properties(GTlabDataProcessor PROPERTIES 
  # set rpath to lib directory, since it is installed to lib/core
  INSTALL_RPATH "\$ORIGIN:\$ORIGIN/..") 

install(TARGETS GTlabDataProcessor
    EXPORT GTlabTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/core
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/core
)

install(FILES ${HEADERS} DESTINATION include/core)
install(FILES ${HEADERS_INTERNAL} DESTINATION include/core/internal)
install(DIRECTORY thirdparty DESTINATION include/core)
######################################################################
