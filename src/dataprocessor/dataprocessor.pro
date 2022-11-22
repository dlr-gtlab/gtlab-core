#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

GT_MODULE_ID="DataProcessor"

include( ../../settings.pri )

CONFIG(debug, debug|release){
    TARGET = GTlabDataProcessor-d
} else {
    TARGET = GTlabDataProcessor
}

QT += core xml
TEMPLATE = lib
CONFIG += plugin
CONFIG += silent
CONFIG += c++14

DEFINES += GT_DATAMODEL_DLL

# qstring restrictions
#DEFINES += QT_NO_CAST_FROM_ASCII
#DEFINES += QT_NO_CAST_TO_ASCII
#DEFINES += QT_NO_CAST_FROM_BYTEARRAY
#DEFINES += QT_NO_URL_CAST_FROM_STRING

CONFIG(debug, debug|release){
    DESTDIR = $${BUILD_DEST}/debug-datamodel
    OBJECTS_DIR = $${BUILD_DEST}/debug-datamodel/obj
    MOC_DIR = $${BUILD_DEST}/debug-datamodel/moc
    RCC_DIR = $${BUILD_DEST}/debug-datamodel/rcc
    UI_DIR = $${BUILD_DEST}/debug-datamodel/ui
} else {
    DESTDIR = $${BUILD_DEST}/release-datamodel
    OBJECTS_DIR = $${BUILD_DEST}/release-datamodel/obj
    MOC_DIR = $${BUILD_DEST}/release-datamodel/moc
    RCC_DIR = $${BUILD_DEST}/release-datamodel/rcc
    UI_DIR = $${BUILD_DEST}/release-datamodel/ui
}

INCLUDEPATH += .\
    property

DESTDIR = $${BUILD_DEST}

HEADERS += \
    gt_abstractexternalizationinterface.h \
    gt_exceptions.h \
    gt_datamodel_exports.h \
    gt_eventloop.h \
    gt_externalizationmanager.h \
    gt_externalizedobject.h \
    gt_externalizedobjectfetcher.h \
    gt_h5externalizationinterface.h \
    gt_h5externalizehelper.h \
    gt_object.h \
    gt_objectfactory.h \
    gt_abstractobjectfactory.h \
    gt_objectmemento.h \
    gt_objectio.h \
    gt_objectgroup.h \
    gt_package.h \
    gt_platform.h \
    #gt_table.h \
    #gt_tablescalers.h \
    #gt_tableparser.h \
    #gt_tableaxis.h \
    gt_factorygroup.h \
    #gt_tablevalues.h \
    gt_typetraits.h \
    gt_utilities.h \
    gt_variantconvert.h \
    gt_xmlexpr.h \
    gt_xmlutilities.h \
    property/gt_enumproperty.h \
    property/gt_objectlinkproperty.h \
    gt_objectmementodiff.h \
    property/gt_abstractproperty.h \
    property/gt_boolproperty.h \
    property/gt_doubleproperty.h \
    property/gt_intproperty.h \
    property/gt_polyvector.h \
    property/gt_property.h \
    property/gt_propertyreference.h \
    property/gt_stringproperty.h \
    property/gt_unit.h \
    property/gt_unitconverter.h \
    property/gt_groupproperty.h \
    property/gt_modetypeproperty.h \
    property/gt_modeproperty.h \
    property/gt_variantproperty.h \
    property/gt_structproperty.h\
    property/gt_propertystructcontainer.h \
    gt_label.h \
    gt_labeldata.h \
    property/gt_labelproperty.h \
    gt_objectmodel.h \
    property/gt_filechooserproperty.h \
    gt_propertyfactory.h \
    property/gt_propertyconnection.h \
    gt_propertyfactory.h \
    property/gt_openfilenameproperty.h \
    property/gt_savefilenameproperty.h \
    property/gt_existingdirectoryproperty.h \
    property/gt_objectpathproperty.h \
    gt_objectpath.h \
    property/gt_relativeobjectlinkproperty.h \
    property/gt_doublelistproperty.h \
    #gt_tablegroup.h \
    gt_regexp.h \
    gt_algorithms.h \
    gt_mpl.h \
    gt_versionnumber.h

SOURCES += \
    gt_abstractexternalizationinterface.cpp \
    gt_exceptions.cpp \
    gt_eventloop.cpp \
    gt_externalizationmanager.cpp \
    gt_externalizedobject.cpp \
    gt_h5externalizationinterface.cpp \
    gt_h5externalizehelper.cpp \
    gt_init_dp.cpp \
    gt_object.cpp \
    gt_objectfactory.cpp \
    gt_abstractobjectfactory.cpp \
    gt_objectmemento.cpp \
    gt_objectio.cpp \
    gt_objectgroup.cpp \
    gt_package.cpp \
    #gt_table.cpp \
    #gt_tableparser.cpp \
    #gt_tableaxis.cpp \
    gt_factorygroup.cpp \
    #gt_tablevalues.cpp \
    gt_xmlutilities.cpp \
    property/gt_objectlinkproperty.cpp \
    gt_objectmementodiff.cpp \
    property/gt_abstractproperty.cpp \
    property/gt_boolproperty.cpp \
    property/gt_doubleproperty.cpp \
    property/gt_intproperty.cpp \
    property/gt_propertyreference.cpp \
    property/gt_stringproperty.cpp \
    property/gt_unit.cpp \
    property/gt_groupproperty.cpp \
    property/gt_modetypeproperty.cpp \
    property/gt_modeproperty.cpp \
    property/gt_variantproperty.cpp \
    property/gt_structproperty.cpp \
    property/gt_propertystructcontainer.cpp \
    gt_label.cpp \
    gt_labeldata.cpp \
    property/gt_labelproperty.cpp \
    gt_objectmodel.cpp \
    property/gt_filechooserproperty.cpp \
    gt_propertyfactory.cpp \
    property/gt_propertyconnection.cpp \
    property/gt_openfilenameproperty.cpp \
    property/gt_savefilenameproperty.cpp \
    property/gt_existingdirectoryproperty.cpp \
    property/gt_objectpathproperty.cpp \
    gt_objectpath.cpp \
    property/gt_relativeobjectlinkproperty.cpp \
    property/gt_doublelistproperty.cpp \
    #gt_tablegroup.cpp \
    gt_versionnumber.cpp \
    gt_regexp.cpp

unix {
    QMAKE_CXXFLAGS += -std=c++14
}

LIBS += -L$${BUILD_DEST}

CONFIG(debug, debug|release) {
    LIBS += -lGTlabNumerics-d -lGTlabLogging-d
} else {
    LIBS += -lGTlabNumerics -lGTlabLogging
}

contains(USE_HDF5, true) {
    message("USE_HDF5 = true")

    #DEFINES += GENH5_NO_DEPRECATED_SYMBOLS
    DEFINES += GT_H5

    CONFIG(debug, debug|release) {
        win32 {
            message("WIN32")
            LIBS += -lhdf5_D -lhdf5_cpp_D
        }
        unix {
            message("UNIX")
            LIBS += -lhdf5 -lhdf5_cpp
        }
        LIBS += -lGenH5-d
    } else {
        LIBS += -lhdf5 -lhdf5_cpp
        LIBS += -lGenH5
    }
}

unix:{
    # suppress the default RPATH if you wish
    QMAKE_LFLAGS_RPATH=
    # add your own with quoting gyrations to make sure $ORIGIN gets to the command line unexpanded
    QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''
}

######################################################################

contains(BUILD_DEPLOY, true) {

    copyHeaders($$HEADERS)
    copyToEnvironmentPath()
}

######################################################################
