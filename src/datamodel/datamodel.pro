#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

include( ../../settings.pri )

TARGET = GTlabDatamodel

QT += core xml
TEMPLATE = lib
CONFIG += plugin
CONFIG += silent
CONFIG += c++11

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
    property \
    ../utilities/logging \
    ../utilities/numerics \
    ../utilities/numerics/bspline \
    ../utilities/physics

DESTDIR = $${BUILD_DEST}

HEADERS += \
    gt_datamodel_exports.h \
    gt_object.h \
    gt_objectfactory.h \
    gt_datamodelinterface.h \
    gt_initmoduleinterface.h \
    gt_abstractobjectfactory.h \
    gt_objectmemento.h \
    gt_objectio.h \
    gt_objectgroup.h \
    gt_datamodelmodule.h \
    gt_moduleinterface.h \
    gt_moduleloader.h \
    gt_package.h \
    gt_spline.h \
    gt_table.h \
    gt_tablescalers.h \
    gt_tableparser.h \
    gt_tableaxis.h \
    gt_factorygroup.h \
    gt_abstractdatazone.h \
    gt_datazone.h \
    gt_datazone0d.h \
    gt_result.h \
    gt_tablevalues.h \
    property/gt_objectlinkproperty.h \
    gt_datazonetable.h \
    gt_objectmementodiff.h \
    gt_datazonetablemainaxis.h \
    gt_datazonetablelist.h \
    property/gt_abstractproperty.h \
    property/gt_boolproperty.h \
    property/gt_doubleproperty.h \
    property/gt_intproperty.h \
    property/gt_property.h \
    property/gt_stringproperty.h \
    property/gt_unit.h \
    property/gt_unitconverter.h \
    property/gt_groupproperty.h \
    property/gt_modetypeproperty.h \
    property/gt_modeproperty.h \
    property/gt_variantproperty.h \
    gt_label.h \
    gt_labeldata.h \
    property/gt_labelproperty.h \
    gt_objectmodel.h \
    property/gt_filechooserproperty.h \
    property/gt_dynamicpropertycontainer.h \
    property/gt_controlscheduleproperty.h \
    gt_controlschedule.h \
    gt_controlscheduleparser.h \
    gt_propertyfactory.h \
    property/gt_propertyconnection.h \
    property/gt_nozzlemapproperty.h \
    gt_nozzlemap.h \
    gt_propertyfactory.h \
    property/gt_openfilenameproperty.h \
    property/gt_savefilenameproperty.h \
    gt_controlschedulegroup.h \
    property/gt_existingdirectoryproperty.h \
    property/gt_objectpathproperty.h \
    gt_objectpath.h \
    property/gt_relativeobjectlinkproperty.h \
    property/gt_doublelistproperty.h \
    gt_splinecp.h \
    gt_splineutilities.h \
    gt_bleedtable.h \
    gt_tablegroup.h \
    property/gt_freestringproperty.h

SOURCES += \
    gt_object.cpp \
    gt_objectfactory.cpp \
    gt_abstractobjectfactory.cpp \
    gt_objectmemento.cpp \
    gt_objectio.cpp \
    gt_objectgroup.cpp \
    gt_datamodelmodule.cpp \
    gt_moduleloader.cpp \
    gt_package.cpp \
    gt_spline.cpp \
    gt_table.cpp \
    gt_tableparser.cpp \
    gt_tableaxis.cpp \
    gt_factorygroup.cpp \
    gt_abstractdatazone.cpp \
    gt_datazone.cpp \
    gt_datazone0d.cpp \
    gt_result.cpp \
    gt_tablevalues.cpp \
    property/gt_objectlinkproperty.cpp \
    gt_datazonetable.cpp \
    gt_objectmementodiff.cpp \
    gt_datazonetablemainaxis.cpp \
    gt_datazonetablelist.cpp \
    property/gt_abstractproperty.cpp \
    property/gt_boolproperty.cpp \
    property/gt_doubleproperty.cpp \
    property/gt_intproperty.cpp \
    property/gt_stringproperty.cpp \
    property/gt_unit.cpp \
    property/gt_unitconverter.cpp \
    property/gt_groupproperty.cpp \
    property/gt_modetypeproperty.cpp \
    property/gt_modeproperty.cpp \
    property/gt_variantproperty.cpp \
    gt_label.cpp \
    gt_labeldata.cpp \
    property/gt_labelproperty.cpp \
    gt_objectmodel.cpp \
    property/gt_filechooserproperty.cpp \
    property/gt_dynamicpropertycontainer.cpp \
    property/gt_controlscheduleproperty.cpp \
    gt_controlschedule.cpp \
    gt_controlscheduleparser.cpp \
    gt_propertyfactory.cpp \
    property/gt_propertyconnection.cpp \
    property/gt_nozzlemapproperty.cpp \
    gt_nozzlemap.cpp \
    property/gt_openfilenameproperty.cpp \
    property/gt_savefilenameproperty.cpp \
    gt_controlschedulegroup.cpp \
    property/gt_existingdirectoryproperty.cpp \
    property/gt_objectpathproperty.cpp \
    gt_objectpath.cpp \
    property/gt_relativeobjectlinkproperty.cpp \
    property/gt_doublelistproperty.cpp \
    gt_splinecp.cpp \
    gt_splineutilities.cpp \
    gt_bleedtable.cpp \
    gt_tablegroup.cpp \
    property/gt_freestringproperty.cpp

unix {
    QMAKE_CXXFLAGS += -std=c++11
}

LIBS += -L$${BUILD_DEST} -lGTlabNumerics -lGTlabPhysics -lGTlabLogging
LIBS += -lSplineLib

copyHeaders($$HEADERS)
unix:   copyToEnvironmentPath($${DESTDIR}/$${TARGET}.so*)
win32:  copyToEnvironmentPath($${DESTDIR}/$${TARGET}.dll)
######################################################################
######################################################################
