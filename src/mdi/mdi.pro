#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

include( $${PWD}/../../settings.pri )

CONFIG(debug, debug|release){
    TARGET = GTlabMdi-d
} else {
    TARGET = GTlabMdi
}

QT += core widgets xml printsupport network qml
TEMPLATE = lib
CONFIG += plugin
CONFIG += silent
CONFIG += c++11

DEFINES += GT_MDI_DLL

#### use qml widget instead of qwidgets
#DEFINES += GT_QML_WIDGETS
#QT += quickwidgets

CONFIG(debug, debug|release){
    DESTDIR = $${BUILD_DEST}/debug-mdi
    OBJECTS_DIR = $${BUILD_DEST}/debug-mdi/obj
    MOC_DIR = $${BUILD_DEST}/debug-mdi/moc
    RCC_DIR = $${BUILD_DEST}/debug-mdi/rcc
    UI_DIR = $${BUILD_DEST}/debug-mdi/ui
} else {
    DESTDIR = $${BUILD_DEST}/release-mdi
    OBJECTS_DIR = $${BUILD_DEST}/release-mdi/obj
    MOC_DIR = $${BUILD_DEST}/release-mdi/moc
    RCC_DIR = $${BUILD_DEST}/release-mdi/rcc
    UI_DIR = $${BUILD_DEST}/release-mdi/ui
}

INCLUDEPATH += post \
    mdi_items \
    dock_widgets \
    tools \
    dialogs \
    tools/graphic_items \
    mdi_items/memento_viewer \
    mdi_items/process_env \
    mdi_items/process_env/entities \
    mdi_items/label_usages \
    mdi_items/session_viewer \
    mdi_items/state_viewer \
    mdi_items/result_viewer \
    mdi_items/collection_env \
    mdi_items/examples \
    dock_widgets/output \
    dock_widgets/explorer \
    dock_widgets/process \
    dock_widgets/process/pages \
    dock_widgets/properties \
    dock_widgets/properties/delegates \
    dock_widgets/properties/editors \
    dock_widgets/properties/items \
    dock_widgets/labels \
    dock_widgets/post \
    dock_widgets/calculators \
    object_ui \
    undoredo \
    importer \
    importer/processImporter \
    exporter \
    exporter/processExporter \
    exporter/dataZoneExporter \
    collection \
    ../datamodel \
    ../network \
    ../calculators \
    ../core \
    ../core/provider \
    ../core/settings \
    ../core/collection \
    ../datamodel/property \
    ../versioncontrol


DESTDIR = $${BUILD_DEST}

HEADERS += \
    dialogs/gt_environmentdialog.h \
    gt_application.h \
    gt_mdi_exports.h \
    gt_mdiinterface.h \
    gt_mdiitem.h \
    gt_mdilauncher.h \
    tools/gt_projectanalyzerdialog.h \
    tools/gt_ruler.h \
    tools/gt_grid.h \
    tools/gt_graphicsscene.h \
    tools/gt_graphicsview.h \
    gt_guimoduleloader.h \
    gt_dockwidget.h \
    dock_widgets/output/gt_outputdock.h \
    gt_perspective.h \
    dock_widgets/explorer/gt_explorerdock.h \
    dock_widgets/explorer/gt_explorerview.h \
    tools/gt_xmlhighlighter.h \
    mdi_items/memento_viewer/gt_mementoviewer.h \
    tools/gt_linenumberarea.h \
    tools/gt_codeeditor.h \
    dock_widgets/process/gt_processdock.h \
    dock_widgets/process/gt_processview.h \
    gt_objectui.h \
    object_ui/gt_projectui.h \
    gt_styledmodel.h \
    object_ui/gt_processui.h \
    dock_widgets/output/gt_styledlogmodel.h \
    dock_widgets/output/gt_filteredlogmodel.h \
    dock_widgets/output/gt_outputtester.h \
    tools/gt_lineedit.h \
    tools/gt_listview.h \
    object_ui/gt_packageui.h \
    object_ui/gt_objectgroupui.h \
    tools/gt_searchwidget.h \
    tools/gt_treeview.h \
    dock_widgets/properties/gt_propertiesdock.h \
    dock_widgets/properties/gt_propertymodel.h \
    dock_widgets/properties/delegates/gt_propertyiddelegate.h \
    tools/gt_tableview.h \
    dock_widgets/properties/delegates/gt_propertyunitdelegate.h \
    dock_widgets/properties/items/gt_propertyitem.h \
    dock_widgets/properties/gt_propertytreeview.h \
    dock_widgets/properties/delegates/gt_propertyvaluedelegate.h \
    dock_widgets/properties/gt_abstractpropertyitem.h \
    dock_widgets/properties/items/gt_propertycategoryitem.h \
    dock_widgets/properties/items/gt_propertymodeitem.h \
    tools/graphic_items/gt_graphicsanimatedpathitem.h \
    tools/graphic_effects/gt_graphicsfadeouteffect.h \
    tools/gt_contour.h \
    object_ui/gt_calculatorui.h \
    gt_queuedmdievent.h \
    gt_projectchangedevent.h \
    gt_objectchangedevent.h \
    dock_widgets/explorer/gt_openwithmenu.h \
    mdi_items/process_env/gt_processeditor.h \
    mdi_items/process_env/entities/gt_taskentity.h \
    mdi_items/process_env/entities/gt_calculatorentity.h \
    mdi_items/process_env/entities/gt_calculatorpropertyentity.h \
    mdi_items/process_env/entities/gt_propertyportentity.h \
    mdi_items/process_env/entities/gt_taskarrowentity.h \
    mdi_items/process_env/entities/gt_taskentityshadow.h \
    mdi_items/process_env/entities/gt_datamodelportentity.h \
    mdi_items/process_env/gt_processeditorview.h \
    mdi_items/process_env/entities/gt_addtaskglobalbutton.h \
    mdi_items/process_env/entities/gt_taskconnectionentity.h \
    gt_objectuiaction.h \
    dock_widgets/explorer/gt_customactionmenu.h \
    undoredo/gt_propertychangecommand.h \
    object_ui/gt_resultui.h \
    object_ui/gt_operatingpointui.h \
    mdi_items/process_env/entities/gt_taskpipeentity.h \
    mdi_items/process_env/entities/gt_tasklinkentity.h \
    object_ui/gt_processlinkui.h \
    mdi_items/process_env/entities/gt_taskarrowlabelentity.h \
    mdi_items/process_env/gt_processeditorscene.h \
    post/gt_posttemplate.h \
    post/gt_posttemplateitem.h \
    post/gt_templateviewer.h \
    post/gt_posttemplatefactory.h \
    post/gt_abstractitemui.h \
    dock_widgets/properties/items/gt_propertyobjectlinkitem.h \
    dock_widgets/properties/editors/gt_propertyobjectlinkeditor.h \
    post/gt_abstractpostwidget.h \
    post/gt_postaction.h \
    post/gt_abstractdataconfigwizard.h \
    post/gt_abstractchartprovider.h \
    post/gt_dummyresults.h \
    dock_widgets/process/gt_processcomponentmodel.h \
    post/gt_templateviewerlistwidget.h \
    post/gt_stringmapcontainer.h \
    post/gt_abstractwizard.h \
    tools/gt_filedialog.h \
    dock_widgets/labels/gt_labelsdock.h \
    dock_widgets/properties/items/gt_propertylabelitem.h \
    object_ui/gt_labelui.h \
    post/gt_stringcontainer.h \
    mdi_items/label_usages/gt_labelusageviewer.h \
    mdi_items/label_usages/gt_labelcontainer.h \
    mdi_items/label_usages/gt_labelusagemodel.h \
    dock_widgets/labels/gt_labeldelegate.h \
    mdi_items/label_usages/gt_labelusagedelegate.h \
    dock_widgets/post/gt_postdock.h \
    dock_widgets/post/gt_postmodel.h \
    post/gt_posttemplatepath.h \
    gt_state.h \
    gt_statehandler.h \
    gt_statecontainer.h \
    mdi_items/session_viewer/gt_sessionviewer.h \
    mdi_items/state_viewer/gt_stateviewer.h \
    mdi_items/state_viewer/gt_statemodel.h \
    gt_stategroup.h \
    dock_widgets/calculators/gt_calculatorsdock.h \
    dock_widgets/process/gt_processwizard.h \
    dock_widgets/process/pages/gt_calculatoroverviewpage.h \
    dock_widgets/process/pages/gt_calculatoroverviewmodel.h \
    dock_widgets/process/pages/gt_abstractprocessitem.h \
    dock_widgets/process/pages/gt_processcategoryitem.h \
    dock_widgets/process/pages/gt_calculatorsettingspage.h \
    gt_extendedcalculatordata.h \
    dock_widgets/process/pages/gt_processwizardpage.h \
    gt_customprocesswizard.h \
    tools/gt_objectselectiondialog.h \
    dock_widgets/post/gt_postdelegate.h \
    dock_widgets/properties/editors/gt_propertyfilechoosereditor.h \
    dock_widgets/properties/items/gt_propertyfilechooseritem.h \
    tools/gt_jshighlighter.h \
    tools/gt_pyhighlighter.h \
    dock_widgets/properties/gt_propertyitemfactory.h \
    dock_widgets/process/gt_processfiltermodel.h \
    mdi_items/memento_viewer/gt_mementoeditor.h \
    mdi_items/result_viewer/gt_resultviewer.h \
    mdi_items/result_viewer/gt_resulttableview.h \
    mdi_items/result_viewer/gt_datazonemodel.h \
    undoredo/gt_mementochangecommand.h \
    gt_processexecutor.h \
    gt_datamodel.h \
    object_ui/gt_loopui.h \
    mdi_items/process_env/gt_processconnectioneditor.h \
    mdi_items/process_env/gt_processconnectionmodel.h \
    mdi_items/process_env/entities/gt_processconnectionview.h \
    mdi_items/process_env/gt_processconnectiongraphicsview.h \
    mdi_items/process_env/entities/gt_processpropertyportentity.h \
    mdi_items/process_env/gt_processconnectionitem.h \
    mdi_items/process_env/entities/gt_processpropertyconnectionentity.h \
    mdi_items/process_env/gt_processconnectionscene.h \
    tools/gt_projectspecwidget.h \
    dock_widgets/properties/items/gt_propertynozzlemapitem.h \
    dock_widgets/properties/editors/gt_propertynozzlemapeditor.h \
    importer/gt_importerinterface.h \
    importer/gt_abstractimporter.h \
    importer/gt_importhandler.h \
    importer/gt_importermetadata.h \
    importer/gt_importmenu.h \
    importer/gt_abstractimporterpage.h \
    importer/gt_selectfiletoimportpage.h \
    exporter/gt_exporterinterface.h \
    exporter/gt_abstractexporter.h \
    exporter/gt_abstractexporterpage.h \
    exporter/gt_selectfiletoexportpage.h \
    exporter/gt_exportermetadata.h \
    exporter/gt_exporthandler.h \
    exporter/gt_exportmenu.h \
    dock_widgets/properties/items/gt_propertyaccessselectionitem.h \
    gt_propertyinterface.h \
    mdi_items/collection_env/gt_collectioneditor.h \
    mdi_items/collection_env/gt_abstractcollectionsettings.h \
    mdi_items/collection_env/gt_collectionsettings.h \
    mdi_items/collection_env/gt_localcollectionwidget.h \
    mdi_items/collection_env/gt_browserwidget.h \
    gt_collectioninterface.h \
    mdi_items/collection_env/gt_localcollectionmodel.h \
    mdi_items/collection_env/gt_collectionbrowsermodel.h \
    collection/gt_collectionitem.h \
    collection/gt_collectionloader.h \
    mdi_items/collection_env/gt_collectionitemwidget.h \
    collection/gt_collectionreply.h \
    collection/gt_collectionhelper.h \
    collection/gt_collectionnetworkitem.h \
    collection/gt_collectionhelperdata.h \
    mdi_items/collection_env/gt_collectionbrowsermodelitem.h \
    mdi_items/collection_env/gt_abstractbrowserwidget.h \
    tools/gt_downloaddialog.h \
    mdi_items/collection_env/gt_collectionloadingwidget.h \
    post/gt_scrollarea.h \
    tools/gt_environmentmodel.h \
    gt_applicationprivate.h \
    object_ui/gt_datazoneui.h \
    gt_objectuiactiongroup.h \
    dock_widgets/process/gt_processcomponentsettingsbutton.h \
    dock_widgets/process/pages/gt_taskoverviewmodel.h \
    dock_widgets/process/pages/gt_processoverviewmodel.h \
    dock_widgets/process/pages/gt_processcalculatoritem.h \
    dock_widgets/process/pages/gt_processtaskitem.h \
    dock_widgets/process/pages/gt_processoverviewtree.h \
    gt_extendedtaskdata.h \
    dock_widgets/process/pages/gt_taskoverviewpage.h \
    tools/gt_confirmdeleteprojectdialog.h \
    mdi_items/examples/gt_examplesmdiwidget.h \
    mdi_items/examples/gt_examplesentry.h \
    mdi_items/examples/gt_examplegraphicalitem.h \
    mdi_items/examples/gt_examplestabwidget.h \
    gt_textfilterdelegate.h \
    gt_regexp.h \
    tools/gt_saveprojectmessagebox.h \
    tools/gt_deleteitemmessagebox.h \
    dialogs/gt_inputdialog.h \
    dock_widgets/properties/items/gt_propertydoublelistitem.h \
    post/gt_scaleproperty.h \
    gt_stylesheets.h \
    dock_widgets/output/gt_taskhistorymodel.h \
    exporter/dataZoneExporter/gt_datazonecsvexporter.h \
    exporter/dataZoneExporter/gt_datazonedatexporter.h \
    tools/gt_contourrunnable.h \
    dock_widgets/properties/items/gt_propertyfreestringitem.h \
    gt_finishedprocessloadinghelper.h \
    exporter/processExporter/gt_processexporter.h \
    importer/processImporter/gt_processimporter.h

SOURCES += \
    dialogs/gt_environmentdialog.cpp \
    gt_application.cpp \
    gt_mdiitem.cpp \
    gt_mdilauncher.cpp \
    tools/gt_projectanalyzerdialog.cpp \
    tools/gt_ruler.cpp \
    tools/gt_grid.cpp \
    tools/gt_graphicsscene.cpp \
    tools/gt_graphicsview.cpp \
    gt_guimoduleloader.cpp \
    gt_dockwidget.cpp \
    dock_widgets/output/gt_outputdock.cpp \
    gt_perspective.cpp \
    dock_widgets/explorer/gt_explorerdock.cpp \
    dock_widgets/explorer/gt_explorerview.cpp \
    tools/gt_xmlhighlighter.cpp \
    mdi_items/memento_viewer/gt_mementoviewer.cpp \
    tools/gt_linenumberarea.cpp \
    tools/gt_codeeditor.cpp \
    dock_widgets/process/gt_processdock.cpp \
    dock_widgets/process/gt_processview.cpp \
    gt_objectui.cpp \
    object_ui/gt_projectui.cpp \
    gt_styledmodel.cpp \
    object_ui/gt_processui.cpp \
    dock_widgets/output/gt_styledlogmodel.cpp \
    dock_widgets/output/gt_filteredlogmodel.cpp \
    dock_widgets/output/gt_outputtester.cpp \
    tools/gt_lineedit.cpp \
    tools/gt_listview.cpp \
    object_ui/gt_packageui.cpp \
    object_ui/gt_objectgroupui.cpp \
    tools/gt_searchwidget.cpp \
    tools/gt_treeview.cpp \
    dock_widgets/properties/gt_propertiesdock.cpp \
    dock_widgets/properties/gt_propertymodel.cpp \
    dock_widgets/properties/delegates/gt_propertyiddelegate.cpp \
    tools/gt_tableview.cpp \
    dock_widgets/properties/delegates/gt_propertyunitdelegate.cpp \
    dock_widgets/properties/items/gt_propertyitem.cpp \
    dock_widgets/properties/gt_propertytreeview.cpp \
    dock_widgets/properties/delegates/gt_propertyvaluedelegate.cpp \
    dock_widgets/properties/gt_abstractpropertyitem.cpp \
    dock_widgets/properties/items/gt_propertycategoryitem.cpp \
    dock_widgets/properties/items/gt_propertymodeitem.cpp \
    tools/graphic_items/gt_graphicsanimatedpathitem.cpp \
    tools/graphic_effects/gt_graphicsfadeouteffect.cpp \
    tools/gt_contour.cpp \
    object_ui/gt_calculatorui.cpp \
    gt_projectchangedevent.cpp \
    gt_objectchangedevent.cpp \
    dock_widgets/explorer/gt_openwithmenu.cpp \
    mdi_items/process_env/gt_processeditor.cpp \
    mdi_items/process_env/entities/gt_taskentity.cpp \
    mdi_items/process_env/entities/gt_calculatorentity.cpp \
    mdi_items/process_env/entities/gt_calculatorpropertyentity.cpp \
    mdi_items/process_env/entities/gt_propertyportentity.cpp \
    mdi_items/process_env/entities/gt_taskarrowentity.cpp \
    mdi_items/process_env/entities/gt_taskentityshadow.cpp \
    mdi_items/process_env/entities/gt_datamodelportentity.cpp \
    mdi_items/process_env/gt_processeditorview.cpp \
    mdi_items/process_env/entities/gt_addtaskglobalbutton.cpp \
    mdi_items/process_env/entities/gt_taskconnectionentity.cpp \
    gt_objectuiaction.cpp \
    dock_widgets/explorer/gt_customactionmenu.cpp \
    undoredo/gt_propertychangecommand.cpp \
    object_ui/gt_resultui.cpp \
    object_ui/gt_operatingpointui.cpp \
    mdi_items/process_env/entities/gt_taskpipeentity.cpp \
    mdi_items/process_env/entities/gt_tasklinkentity.cpp \
    object_ui/gt_processlinkui.cpp \
    mdi_items/process_env/entities/gt_taskarrowlabelentity.cpp \
    mdi_items/process_env/gt_processeditorscene.cpp \
    post/gt_posttemplate.cpp \
    post/gt_posttemplateitem.cpp \
    post/gt_templateviewer.cpp \
    post/gt_posttemplatefactory.cpp \
    post/gt_abstractitemui.cpp \
    dock_widgets/properties/items/gt_propertyobjectlinkitem.cpp \
    dock_widgets/properties/editors/gt_propertyobjectlinkeditor.cpp \
    post/gt_abstractpostwidget.cpp \
    post/gt_postaction.cpp \
    post/gt_abstractdataconfigwizard.cpp \
    post/gt_abstractchartprovider.cpp \
    post/gt_dummyresults.cpp \
    dock_widgets/process/gt_processcomponentmodel.cpp \
    post/gt_templateviewerlistwidget.cpp \
    post/gt_stringmapcontainer.cpp \
    tools/gt_filedialog.cpp \
    dock_widgets/labels/gt_labelsdock.cpp \
    dock_widgets/properties/items/gt_propertylabelitem.cpp \
    object_ui/gt_labelui.cpp \
    post/gt_stringcontainer.cpp \
    mdi_items/label_usages/gt_labelusageviewer.cpp \
    mdi_items/label_usages/gt_labelcontainer.cpp \
    mdi_items/label_usages/gt_labelusagemodel.cpp \
    dock_widgets/labels/gt_labeldelegate.cpp \
    mdi_items/label_usages/gt_labelusagedelegate.cpp \
    dock_widgets/post/gt_postdock.cpp \
    dock_widgets/post/gt_postmodel.cpp \
    gt_state.cpp \
    gt_statehandler.cpp \
    gt_statecontainer.cpp \
    mdi_items/session_viewer/gt_sessionviewer.cpp \
    mdi_items/state_viewer/gt_stateviewer.cpp \
    mdi_items/state_viewer/gt_statemodel.cpp \
    post/gt_posttemplatepath.cpp \
    post/gt_abstractwizard.cpp \
    gt_stategroup.cpp \
    dock_widgets/calculators/gt_calculatorsdock.cpp \
    dock_widgets/process/gt_processwizard.cpp \
    dock_widgets/process/pages/gt_calculatoroverviewpage.cpp \
    dock_widgets/process/pages/gt_calculatoroverviewmodel.cpp \
    dock_widgets/process/pages/gt_processcategoryitem.cpp \
    dock_widgets/process/pages/gt_calculatorsettingspage.cpp \
    gt_extendedcalculatordata.cpp \
    dock_widgets/process/pages/gt_processwizardpage.cpp \
    gt_customprocesswizard.cpp \
    tools/gt_objectselectiondialog.cpp \
    dock_widgets/post/gt_postdelegate.cpp \
    dock_widgets/properties/editors/gt_propertyfilechoosereditor.cpp \
    dock_widgets/properties/items/gt_propertyfilechooseritem.cpp \
    tools/gt_jshighlighter.cpp \
    tools/gt_pyhighlighter.cpp \
    dock_widgets/properties/gt_propertyitemfactory.cpp \
    dock_widgets/process/gt_processfiltermodel.cpp \
    mdi_items/memento_viewer/gt_mementoeditor.cpp \
    mdi_items/result_viewer/gt_resultviewer.cpp \
    mdi_items/result_viewer/gt_resulttableview.cpp \
    mdi_items/result_viewer/gt_datazonemodel.cpp \
    undoredo/gt_mementochangecommand.cpp \
    gt_processexecutor.cpp \
    gt_datamodel.cpp \
    object_ui/gt_loopui.cpp \
    mdi_items/process_env/gt_processconnectioneditor.cpp \
    mdi_items/process_env/gt_processconnectionmodel.cpp \
    mdi_items/process_env/entities/gt_processconnectionview.cpp \
    mdi_items/process_env/gt_processconnectiongraphicsview.cpp \
    mdi_items/process_env/entities/gt_processpropertyportentity.cpp \
    mdi_items/process_env/gt_processconnectionitem.cpp \
    mdi_items/process_env/entities/gt_processpropertyconnectionentity.cpp \
    mdi_items/process_env/gt_processconnectionscene.cpp \
    tools/gt_projectspecwidget.cpp \
    dock_widgets/properties/items/gt_propertynozzlemapitem.cpp \
    dock_widgets/properties/editors/gt_propertynozzlemapeditor.cpp \
    importer/gt_abstractimporter.cpp \
    importer/gt_importhandler.cpp \
    importer/gt_importermetadata.cpp \
    importer/gt_importmenu.cpp \
    importer/gt_abstractimporterpage.cpp \
    importer/gt_selectfiletoimportpage.cpp \
    exporter/gt_abstractexporter.cpp \
    exporter/gt_abstractexporterpage.cpp \
    exporter/gt_selectfiletoexportpage.cpp \
    exporter/gt_exportermetadata.cpp \
    exporter/gt_exporthandler.cpp \
    exporter/gt_exportmenu.cpp \
    dock_widgets/properties/items/gt_propertyaccessselectionitem.cpp \
    mdi_items/collection_env/gt_collectioneditor.cpp \
    mdi_items/collection_env/gt_abstractcollectionsettings.cpp \
    mdi_items/collection_env/gt_collectionsettings.cpp \
    mdi_items/collection_env/gt_localcollectionwidget.cpp \
    mdi_items/collection_env/gt_browserwidget.cpp \
    mdi_items/collection_env/gt_localcollectionmodel.cpp \
    mdi_items/collection_env/gt_collectionbrowsermodel.cpp \
    collection/gt_collectionitem.cpp \
    collection/gt_collectionloader.cpp \
    mdi_items/collection_env/gt_collectionitemwidget.cpp \
    collection/gt_collectionreply.cpp \
    collection/gt_collectionhelper.cpp \
    collection/gt_collectionnetworkitem.cpp \
    collection/gt_collectionhelperdata.cpp \
    mdi_items/collection_env/gt_collectionbrowsermodelitem.cpp \
    mdi_items/collection_env/gt_abstractbrowserwidget.cpp \
    tools/gt_downloaddialog.cpp \
    mdi_items/collection_env/gt_collectionloadingwidget.cpp \
    post/gt_scrollarea.cpp \
    tools/gt_environmentmodel.cpp \
    gt_applicationprivate.cpp \
    object_ui/gt_datazoneui.cpp \
    gt_objectuiactiongroup.cpp \
    dock_widgets/process/gt_processcomponentsettingsbutton.cpp \
    dock_widgets/process/pages/gt_taskoverviewmodel.cpp \
    dock_widgets/process/pages/gt_processoverviewmodel.cpp \
    dock_widgets/process/pages/gt_processcalculatoritem.cpp \
    dock_widgets/process/pages/gt_processtaskitem.cpp \
    dock_widgets/process/pages/gt_processoverviewtree.cpp \
    gt_extendedtaskdata.cpp \
    dock_widgets/process/pages/gt_taskoverviewpage.cpp \
    tools/gt_confirmdeleteprojectdialog.cpp \
    mdi_items/examples/gt_examplesmdiwidget.cpp \
    mdi_items/examples/gt_examplesentry.cpp \
    mdi_items/examples/gt_examplegraphicalitem.cpp \
    mdi_items/examples/gt_examplestabwidget.cpp \
    gt_textfilterdelegate.cpp \
    gt_regexp.cpp \
    tools/gt_saveprojectmessagebox.cpp \
    tools/gt_deleteitemmessagebox.cpp \
    dialogs/gt_inputdialog.cpp \
    dock_widgets/properties/items/gt_propertydoublelistitem.cpp \
    post/gt_scaleproperty.cpp \
    gt_stylesheets.cpp \
    dock_widgets/output/gt_taskhistorymodel.cpp \
    exporter/dataZoneExporter/gt_datazonecsvexporter.cpp \
    exporter/dataZoneExporter/gt_datazonedatexporter.cpp \
    tools/gt_contourrunnable.cpp \
    dock_widgets/properties/items/gt_propertyfreestringitem.cpp \
    gt_finishedprocessloadinghelper.cpp \
    exporter/processExporter/gt_processexporter.cpp \
    importer/processImporter/gt_processimporter.cpp

RESOURCES += ../resources/icons/icons.qrc \
    ../resources/pixmaps/pixmaps.qrc \
    ../resources/qml/qml.qrc

LIBS += -L$${BUILD_DEST}

CONFIG(debug, debug|release){
    # GTlab Utilities
    LIBS += -lGTlabNumerics-d -lGTlabLogging-d
    #GTlab Core
    LIBS += -lGTlabDatamodel-d -lGTlabNetwork-d -lGTlabCalculators-d
    LIBS += -lGTlabCore-d

} else {
    # GTlab Utilities
    LIBS += -lGTlabNumerics -lGTlabLogging
    #GTlab Core
    LIBS += -lGTlabDatamodel -lGTlabNetwork -lGTlabCalculators
    LIBS += -lGTlabCore
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
