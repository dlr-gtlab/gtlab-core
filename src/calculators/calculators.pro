#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

include( $${PWD}/../../settings.pri )

TARGET = GTlabCalculators

QT += core xml
TEMPLATE = lib
CONFIG += plugin
CONFIG += silent
CONFIG += c++11

DEFINES += GT_CALCULATORS_DLL

CONFIG(debug, debug|release){
    DESTDIR = $${BUILD_DEST}/debug-calculators
    OBJECTS_DIR = $${BUILD_DEST}/debug-calculators/obj
    MOC_DIR = $${BUILD_DEST}/debug-calculators/moc
    RCC_DIR = $${BUILD_DEST}/debug-calculators/rcc
    UI_DIR = $${BUILD_DEST}/debug-calculators/ui
} else {
    DESTDIR = $${BUILD_DEST}/release-calculators
    OBJECTS_DIR = $${BUILD_DEST}/release-calculators/obj
    MOC_DIR = $${BUILD_DEST}/release-calculators/moc
    RCC_DIR = $${BUILD_DEST}/release-calculators/rcc
    UI_DIR = $${BUILD_DEST}/release-calculators/ui
}
INCLUDEPATH += .\
    ../datamodel \
    ../datamodel/property \
    ../network \
    ../utilities/logging

DESTDIR = $${BUILD_DEST}

HEADERS += \
    gt_calculators_exports.h \
    gt_processfactory.h \
    gt_task.h \
    gt_calculator.h \
    gt_processdata.h \
    gt_processinterface.h \
    gt_processmoduleloader.h \
    gt_calculatorfactory.h \
    gt_taskfactory.h \
    gt_tasklink.h \
    gt_abstractrunnable.h \
    gt_processcomponent.h \
    gt_abstractcalculatorexecutor.h \
    gt_calculatorexecinterface.h \
    gt_calculatorexecutorlist.h \
    gt_calculatorprovider.h \
    gt_taskprovider.h \
    gt_abstractprocessprovider.h \
    gt_abstractprocessdata.h \
    gt_calculatordata.h \
    gt_evaluator.h \
    gt_taskrunner.h \
    gt_loop.h \
    gt_doublemonitoringproperty.h \
    gt_monitoringproperty.h \
    gt_intmonitoringproperty.h \
    gt_monitoringdata.h \
    gt_monitoringdatatable.h \
    gt_monitoringdataset.h \
    gt_environment.h \
    gt_taskdata.h \
    gt_residualloop.h \
    gt_parameterloop.h \
    gt_calculatorhelperfactory.h

SOURCES += \
    gt_processfactory.cpp \
    gt_task.cpp \
    gt_calculator.cpp \
    gt_processdata.cpp \
    gt_processmoduleloader.cpp \
    gt_calculatorfactory.cpp \
    gt_taskfactory.cpp \
    gt_tasklink.cpp \
    gt_abstractrunnable.cpp \
    gt_processcomponent.cpp \
    gt_abstractcalculatorexecutor.cpp \
    gt_calculatorexecutorlist.cpp \
    gt_calculatorprovider.cpp \
    gt_taskprovider.cpp \
    gt_abstractprocessprovider.cpp \
    gt_abstractprocessdata.cpp \
    gt_calculatordata.cpp \
    gt_evaluator.cpp \
    gt_taskrunner.cpp \
    gt_loop.cpp \
    gt_doublemonitoringproperty.cpp \
    gt_monitoringproperty.cpp \
    gt_intmonitoringproperty.cpp \
    gt_monitoringdata.cpp \
    gt_monitoringdatatable.cpp \
    gt_monitoringdataset.cpp \
    gt_environment.cpp \
    gt_taskdata.cpp \
    gt_residualloop.cpp \
    gt_parameterloop.cpp \
    gt_calculatorhelperfactory.cpp

LIBS += -L$${BUILD_DEST} -lGTlabNumerics -lGTlabLogging -lGTlabDatamodel -lGTlabNetwork


######################################################################
