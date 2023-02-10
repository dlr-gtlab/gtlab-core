#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

GT_MODULE_ID="Core"

include( $${PWD}/../../settings.pri )

CONFIG(debug, debug|release){
    TARGET = GTlabCore-d
} else {
    TARGET = GTlabCore
}

QT += core xml network
TEMPLATE = lib
CONFIG += plugin
CONFIG += silent
CONFIG += c++14

DEFINES += GT_CORE_DLL

# qstring restrictions
#DEFINES += QT_NO_CAST_FROM_ASCII
#DEFINES += QT_NO_CAST_TO_ASCII
#DEFINES += QT_NO_CAST_FROM_BYTEARRAY
#DEFINES += QT_NO_URL_CAST_FROM_STRING

CONFIG(debug, debug|release){
    DESTDIR = $${BUILD_DEST}/debug-core
    OBJECTS_DIR = $${BUILD_DEST}/debug-core/obj
    MOC_DIR = $${BUILD_DEST}/debug-core/moc
    RCC_DIR = $${BUILD_DEST}/debug-core/rcc
    UI_DIR = $${BUILD_DEST}/debug-core/ui
} else {
    DESTDIR = $${BUILD_DEST}/release-core
    OBJECTS_DIR = $${BUILD_DEST}/release-core/obj
    MOC_DIR = $${BUILD_DEST}/release-core/moc
    RCC_DIR = $${BUILD_DEST}/release-core/rcc
    UI_DIR = $${BUILD_DEST}/release-core/ui
}
INCLUDEPATH += .\
    settings \
    provider \
    process_management \
    process_management/process_runner/ \
    process_management/process_runner/commands \
    network \
    states \
    thirdparty \
    ../dataprocessor \
    ../dataprocessor/property

DESTDIR = $${BUILD_DEST}

HEADERS += \
    gt_commandlinefunction.h \
    gt_commandlineparser.h \
    gt_filesystem.h \
    gt_logmodel.h \
    gt_functionalinterface.h \
    gt_processexecutormanager.h \
    gt_propertyconnectionfunctions.h \
    gt_sharedfunction.h \
    internal/gt_commandlinefunctionhandler.h \
    internal/gt_coreupgraderoutines.h \
    internal/gt_platformspecifics.h \
    internal/gt_sharedfunctionhandler.h \
    network/gt_accessdata.h \
    network/gt_accessmanager.h \
    network/gt_accessgroup.h \
    network/gt_hostinfo.h \
    network/gt_networkinterface.h \
    network/gt_accessdataconnection.h \
    network/gt_abstractaccessdataconnection.h \
    network/gt_accessselectionproperty.h \
    network/gt_downloaditem.h \
    network/gt_downloader.h \
    network/gt_tcpsocket.h \
    process_management/gt_processfactory.h \
    process_management/gt_task.h \
    process_management/gt_calculator.h \
    process_management/gt_processdata.h \
    process_management/gt_processinterface.h \
    process_management/gt_processmoduleloader.h \
    process_management/gt_calculatorfactory.h \
    process_management/gt_taskfactory.h \
    process_management/gt_taskgroup.h \
    process_management/gt_tasklink.h \
    process_management/gt_abstractrunnable.h \
    process_management/gt_processcomponent.h \
    process_management/gt_abstractcalculatorexecutor.h \
    process_management/gt_calculatorexecinterface.h \
    process_management/gt_calculatorexecutorlist.h \
    process_management/gt_calculatorprovider.h \
    process_management/gt_taskprovider.h \
    process_management/gt_abstractprocessprovider.h \
    process_management/gt_abstractprocessdata.h \
    process_management/gt_calculatordata.h \
    process_management/gt_taskrunner.h \
    process_management/gt_loop.h \
    process_management/gt_doublemonitoringproperty.h \
    process_management/gt_monitoringproperty.h \
    process_management/gt_intmonitoringproperty.h \
    process_management/gt_monitoringdata.h \
    process_management/gt_monitoringdatatable.h \
    process_management/gt_monitoringdataset.h \
    process_management/gt_environment.h \
    process_management/gt_taskdata.h \
    process_management/gt_residualloop.h \
    process_management/gt_parameterloop.h \
    process_management/gt_calculatorhelperfactory.h \
    process_management/gt_stringmonitoringproperty.h \
    process_management/process_runner/commands/gt_processrunnercommand.h \
    process_management/process_runner/commands/gt_processrunnernotification.h \
    process_management/process_runner/commands/gt_processrunnerresponse.h \
    process_management/process_runner/gt_processrunneraccessdataconnection.h \
    process_management/process_runner/gt_processrunnercommandfactory.h \
    process_management/process_runner/gt_processrunnerconnectionstrategy.h \
    process_management/process_runner/gt_processrunnerglobals.h \
    process_management/process_runner/gt_processrunnertcpconnection.h \
    settings/gt_shortcutsettingsdata.h \
    states/gt_state.h \
    states/gt_stategroup.h \
    states/gt_statehandler.h \
    states/gt_statecontainer.h \
    gt_coreapplication.h \
    gt_core_exports.h \
    gt_footprint.h \
    gt_globals.h \
    gt_projectanalyzer.h \
    gt_saveprojecthelper.h \
    gt_session.h \
    gt_project.h \
    settings/gt_settingsitem.h \
    settings/gt_abstractsettings.h \
    settings/gt_settings.h \
    gt_coreprocessexecutor.h \
    gt_explorermodel.h \
    provider/gt_projectprovider.h \
    gt_treefiltermodel.h \
    gt_checkablestringlistmodel.h \
    gt_coremoduleloader.h \
    gt_runnable.h \
    gt_objectfiltermodel.h \
    gt_projectdiffpatch.h \
    gt_coredatamodel.h \
    gt_loadprojecthelper.h \
    gt_updatechecker.h \
    gt_abstractloadinghelper.h \
    gt_moduleinterface.h \
    gt_moduleloader.h \
    gt_datamodelinterface.h \
    gt_command.h \
    internal/gt_moduleupgrader.h

SOURCES += \
    gt_commandlinefunction.cpp \
    gt_commandlineparser.cpp \
    gt_filesystem.cpp \
    gt_logmodel.cpp \
    gt_processexecutormanager.cpp \
    gt_propertyconnectionfunctions.cpp \
    gt_sharedfunction.cpp \
    internal/gt_commandlinefunctionhandler.cpp \
    internal/gt_coreupgraderoutines.cpp \
    internal/gt_platformspecifics.cpp \
    internal/gt_sharedfunctionhandler.cpp \
    network/gt_accessdata.cpp \
    network/gt_accessmanager.cpp \
    network/gt_accessgroup.cpp \
    network/gt_accessdataconnection.cpp \
    network/gt_abstractaccessdataconnection.cpp \
    network/gt_accessselectionproperty.cpp \
    network/gt_downloaditem.cpp \
    network/gt_downloader.cpp \
    network/gt_hostinfo.cpp \
    network/gt_tcpsocket.cpp \
    process_management/gt_processfactory.cpp \
    process_management/gt_task.cpp \
    process_management/gt_calculator.cpp \
    process_management/gt_processdata.cpp \
    process_management/gt_processmoduleloader.cpp \
    process_management/gt_calculatorfactory.cpp \
    process_management/gt_taskfactory.cpp \
    process_management/gt_taskgroup.cpp \
    process_management/gt_tasklink.cpp \
    process_management/gt_abstractrunnable.cpp \
    process_management/gt_processcomponent.cpp \
    process_management/gt_abstractcalculatorexecutor.cpp \
    process_management/gt_calculatorexecutorlist.cpp \
    process_management/gt_calculatorprovider.cpp \
    process_management/gt_taskprovider.cpp \
    process_management/gt_abstractprocessprovider.cpp \
    process_management/gt_abstractprocessdata.cpp \
    process_management/gt_calculatordata.cpp \
    process_management/gt_taskrunner.cpp \
    process_management/gt_loop.cpp \
    process_management/gt_doublemonitoringproperty.cpp \
    process_management/gt_monitoringproperty.cpp \
    process_management/gt_intmonitoringproperty.cpp \
    process_management/gt_monitoringdata.cpp \
    process_management/gt_monitoringdatatable.cpp \
    process_management/gt_monitoringdataset.cpp \
    process_management/gt_environment.cpp \
    process_management/gt_taskdata.cpp \
    process_management/gt_residualloop.cpp \
    process_management/gt_parameterloop.cpp \
    process_management/gt_calculatorhelperfactory.cpp \
    process_management/gt_stringmonitoringproperty.cpp \
    process_management/process_runner/commands/gt_processrunnercommand.cpp \
    process_management/process_runner/commands/gt_processrunnernotification.cpp \
    process_management/process_runner/commands/gt_processrunnerresponse.cpp \
    process_management/process_runner/gt_processrunneraccessdataconnection.cpp \
    process_management/process_runner/gt_processrunnercommandfactory.cpp \
    process_management/process_runner/gt_processrunnerconnectionstrategy.cpp \
    process_management/process_runner/gt_processrunnerglobals.cpp \
    process_management/process_runner/gt_processrunnertcpconnection.cpp \
    settings/gt_shortcutsettingsdata.cpp \
    states/gt_state.cpp \
    states/gt_stategroup.cpp \
    states/gt_statehandler.cpp \
    states/gt_statecontainer.cpp \
    gt_coreapplication.cpp \
    gt_footprint.cpp \
    gt_projectanalyzer.cpp \
    gt_saveprojecthelper.cpp \
    gt_session.cpp \
    gt_project.cpp \
    settings/gt_settingsitem.cpp \
    settings/gt_abstractsettings.cpp \
    settings/gt_settings.cpp \
    gt_coreprocessexecutor.cpp \
    gt_explorermodel.cpp \
    provider/gt_projectprovider.cpp \
    gt_treefiltermodel.cpp \
    gt_checkablestringlistmodel.cpp \
    gt_coremoduleloader.cpp \
    gt_runnable.cpp \
    gt_objectfiltermodel.cpp \
    gt_projectdiffpatch.cpp \
    gt_coredatamodel.cpp \
    gt_loadprojecthelper.cpp \
    gt_updatechecker.cpp \
    gt_abstractloadinghelper.cpp \
    gt_moduleloader.cpp \
    gt_command.cpp \
    internal/gt_moduleupgrader.cpp

RESOURCES += ../resources/templates/templates.qrc

LIBS += -L$${BUILD_DEST}

# Use of pre compiled logging header to reduce compile time
PRECOMPILED_HEADER = $${GTLAB_LOGGING_PATH}/include/logging/gt_logging.h

CONFIG(debug, debug|release){
    LIBS += -lGTlabLogging-d -lGTlabDataProcessor-d
} else {
    LIBS += -lGTlabLogging -lGTlabDataProcessor
}

unix {
    QMAKE_CXXFLAGS += -std=c++11
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
