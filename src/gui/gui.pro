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
    TARGET = GTlabGui-d
} else {
    TARGET = GTlabGui
}

QT += core widgets network xml printsupport qml
TEMPLATE = lib
CONFIG += plugin
CONFIG += silent
CONFIG += c++14

DEFINES += GT_GUI_DLL
DEFINES += GT_MODULE_ID=Gui

QTPLUGIN += gif

CONFIG(debug, debug|release){
    DESTDIR = $${BUILD_DEST}/debug-gui
    OBJECTS_DIR = $${BUILD_DEST}/debug-gui/obj
    MOC_DIR = $${BUILD_DEST}/debug-gui/moc
    RCC_DIR = $${BUILD_DEST}/debug-gui/rcc
    UI_DIR = $${BUILD_DEST}/debug-gui/ui
} else {
    DESTDIR = $${BUILD_DEST}/release-gui
    OBJECTS_DIR = $${BUILD_DEST}/release-gui/obj
    MOC_DIR = $${BUILD_DEST}/release-gui/moc
    RCC_DIR = $${BUILD_DEST}/release-gui/rcc
    UI_DIR = $${BUILD_DEST}/release-gui/ui
}
INCLUDEPATH += .\
    post \
    mdi_items \
    dock_widgets \
    tools \
    dialogs \
    dialogs/project_settings \
    dialogs/project_upgrader \
    tools/graphic_items \
    mdi_items/collection_env \
    dock_widgets/process \
    dock_widgets/process/pages \
    dock_widgets/properties \
    dock_widgets/properties/delegates \
    dock_widgets/properties/editors \
    dock_widgets/properties/items \
    dock_widgets/post \
    object_ui \
    undoredo \
    importer \
    importer/processImporter \
    exporter \
    exporter/processExporter \
    exporter/dataZoneExporter \
    collection \
    widgets \
    ../dataprocessor \
    ../dataprocessor/property \
    ../core \
    ../core/settings \
    ../core/network \
    ../core/process_management \
    ../core/provider \
    ../core/states

DESTDIR = $${BUILD_DEST}

HEADERS += \
    dialogs/project_upgrader/gt_projectupgradedialog.h \
    dialogs/project_upgrader/gt_projectupgradeoverviewpage.h \
    dialogs/project_upgrader/gt_projectupgradesettingspage.h \
    dialogs/project_upgrader/gt_projectupgradestartpage.h \
    gt_colors.h \
    gt_gui_exports.h \
    gt_application.h \
    gt_icons.h \
    gt_mdiinterface.h \
    gt_mdiitem.h \
    gt_mdilauncher.h \
    gt_palette.h \
    gt_shortcut.h \
    gt_shortcuts.h \
    tools/gt_switchprojectmessagebox.h \
    widgets/gt_shortcutedit.h \
    gt_processexecutor.h \
    gt_datamodel.h \
    gt_guimoduleloader.h \
    gt_dockwidget.h \
    gt_perspective.h \
    gt_styledmodel.h \
    gt_queuedmdievent.h \
    gt_projectchangedevent.h \
    gt_objectchangedevent.h \
    gt_wizard.h \
    tools/gt_projectanalyzerdialog.h \
    tools/gt_ruler.h \
    tools/gt_grid.h \
    tools/gt_graphicsscene.h \
    tools/gt_graphicsview.h \
    tools/gt_xmlhighlighter.h \
    tools/gt_linenumberarea.h \
    tools/gt_codeeditor.h \
    tools/gt_lineedit.h \
    tools/gt_listview.h \
    tools/gt_searchwidget.h \
    tools/gt_treeview.h \
    tools/gt_tableview.h \
    tools/graphic_items/gt_graphicsanimatedpathitem.h \
    tools/graphic_effects/gt_graphicsfadeouteffect.h \
    tools/gt_contour.h \
    tools/gt_filedialog.h \
    tools/gt_objectselectiondialog.h \
    tools/gt_jshighlighter.h \
    tools/gt_pyhighlighter.h \
    tools/gt_projectspecwidget.h \
    tools/gt_confirmdeleteprojectdialog.h \
    tools/gt_saveprojectmessagebox.h \
    tools/gt_deleteitemmessagebox.h \
    tools/gt_contourrunnable.h \
    tools/gt_downloaddialog.h \
    tools/gt_environmentmodel.h \
    gt_objectui.h \
    object_ui/gt_projectui.h \
    object_ui/gt_processui.h \
    object_ui/gt_packageui.h \
    object_ui/gt_objectgroupui.h \
    object_ui/gt_calculatorui.h \
    object_ui/gt_resultui.h \
    object_ui/gt_processlinkui.h \
    object_ui/gt_labelui.h \
    object_ui/gt_loopui.h \
    object_ui/gt_datazoneui.h \
    dock_widgets/process/gt_processwizard.h \
    dock_widgets/process/gt_processfiltermodel.h \
    dock_widgets/process/gt_processcomponentmodel.h \
    dock_widgets/process/pages/gt_calculatoroverviewpage.h \
    dock_widgets/process/pages/gt_calculatoroverviewmodel.h \
    dock_widgets/process/pages/gt_abstractprocessitem.h \
    dock_widgets/process/pages/gt_processcategoryitem.h \
    dock_widgets/process/pages/gt_calculatorsettingspage.h \
    dock_widgets/process/pages/gt_processwizardpage.h \
    dock_widgets/process/pages/gt_taskoverviewmodel.h \
    dock_widgets/process/pages/gt_processoverviewmodel.h \
    dock_widgets/process/pages/gt_processcalculatoritem.h \
    dock_widgets/process/pages/gt_processtaskitem.h \
    dock_widgets/process/pages/gt_processoverviewtree.h \
    dock_widgets/process/pages/gt_taskoverviewpage.h \
    dock_widgets/properties/gt_propertymodel.h \
    dock_widgets/properties/gt_propertytreeview.h \
    dock_widgets/properties/gt_abstractpropertyitem.h \
    dock_widgets/properties/gt_propertyitemfactory.h \
    dock_widgets/properties/delegates/gt_propertyiddelegate.h \
    dock_widgets/properties/delegates/gt_propertyunitdelegate.h \
    dock_widgets/properties/delegates/gt_propertyvaluedelegate.h \
    dock_widgets/properties/items/gt_propertycategoryitem.h \
    dock_widgets/properties/items/gt_propertymodeitem.h \
    dock_widgets/properties/items/gt_propertyobjectlinkitem.h \
    dock_widgets/properties/items/gt_propertyfilechooseritem.h \
    dock_widgets/properties/items/gt_propertyaccessselectionitem.h \
    dock_widgets/properties/items/gt_propertydoublelistitem.h \
    dock_widgets/properties/items/gt_propertylabelitem.h \
    dock_widgets/properties/items/gt_propertyitem.h \
    dock_widgets/properties/editors/gt_propertyfilechoosereditor.h \
    dock_widgets/properties/editors/gt_propertyobjectlinkeditor.h \
    dock_widgets/post/gt_postmodel.h \
    dock_widgets/gt_customactionmenu.h \
    gt_objectuiaction.h \
    undoredo/gt_propertychangecommand.h \
    undoredo/gt_mementochangecommand.h \
    post/gt_posttemplate.h \
    post/gt_posttemplateitem.h \
    post/gt_templateviewer.h \
    post/gt_posttemplatefactory.h \
    post/gt_abstractitemui.h \
    post/gt_abstractpostwidget.h \
    post/gt_postaction.h \
    post/gt_abstractdataconfigwizard.h \
    post/gt_abstractchartprovider.h \
    post/gt_templateviewerlistwidget.h \
    post/gt_stringmapcontainer.h \
    post/gt_abstractwizard.h \
    post/gt_stringcontainer.h \
    post/gt_posttemplatepath.h \
    gt_customprocesswizard.h \
    gt_extendedcalculatordata.h \
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
    exporter/dataZoneExporter/gt_datazonecsvexporter.h \
    exporter/dataZoneExporter/gt_datazonedatexporter.h \
    exporter/processExporter/gt_processexporter.h \
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
    mdi_items/collection_env/gt_collectionloadingwidget.h \
    post/gt_scrollarea.h \
    gt_applicationprivate.h \
    gt_objectuiactiongroup.h \
    gt_extendedtaskdata.h \
    dialogs/gt_inputdialog.h \
    dialogs/project_settings/gt_projectsettingsdialog.h \
    dialogs/project_settings/gt_projectsettingsexternalizationtab.h \
    dialogs/project_settings/gt_projectsettingstab.h \
    post/gt_scaleproperty.h \
    gt_stylesheets.h \
    gt_textfilterdelegate.h \
    gt_finishedprocessloadinghelper.h \
    importer/processImporter/gt_processimporter.h

SOURCES += \
    dialogs/project_upgrader/gt_projectupgradedialog.cpp \
    dialogs/project_upgrader/gt_projectupgradeoverviewpage.cpp \
    dialogs/project_upgrader/gt_projectupgradesettingspage.cpp \
    dialogs/project_upgrader/gt_projectupgradestartpage.cpp \
    gt_application.cpp \
    gt_colors.cpp \
    gt_icons.cpp \
    gt_mdiitem.cpp \
    gt_mdilauncher.cpp \
    gt_palette.cpp \
    gt_shortcut.cpp \
    gt_shortcuts.cpp \
    tools/gt_switchprojectmessagebox.cpp \
    widgets/gt_shortcutedit.cpp \
    gt_guimoduleloader.cpp \
    gt_dockwidget.cpp \
    gt_perspective.cpp \
    gt_styledmodel.cpp \
    gt_projectchangedevent.cpp \
    gt_objectchangedevent.cpp \
    gt_wizard.cpp \
    tools/gt_projectanalyzerdialog.cpp \
    tools/gt_ruler.cpp \
    tools/gt_grid.cpp \
    tools/gt_graphicsscene.cpp \
    tools/gt_graphicsview.cpp \
    tools/gt_xmlhighlighter.cpp \
    tools/gt_linenumberarea.cpp \
    tools/gt_codeeditor.cpp \
    tools/gt_searchwidget.cpp \
    tools/gt_treeview.cpp \
    tools/gt_lineedit.cpp \
    tools/gt_listview.cpp \
    tools/gt_tableview.cpp \
    tools/graphic_items/gt_graphicsanimatedpathitem.cpp \
    tools/graphic_effects/gt_graphicsfadeouteffect.cpp \
    tools/gt_contour.cpp \
    tools/gt_objectselectiondialog.cpp \
    tools/gt_jshighlighter.cpp \
    tools/gt_pyhighlighter.cpp \
    tools/gt_projectspecwidget.cpp \
    tools/gt_filedialog.cpp \
    tools/gt_confirmdeleteprojectdialog.cpp \
    tools/gt_contourrunnable.cpp \
    tools/gt_saveprojectmessagebox.cpp \
    tools/gt_deleteitemmessagebox.cpp \
    tools/gt_downloaddialog.cpp \
    tools/gt_environmentmodel.cpp \
    gt_objectui.cpp \
    object_ui/gt_projectui.cpp \
    object_ui/gt_processui.cpp \
    object_ui/gt_packageui.cpp \
    object_ui/gt_objectgroupui.cpp \
    object_ui/gt_calculatorui.cpp \
    object_ui/gt_resultui.cpp \
    object_ui/gt_processlinkui.cpp \
    object_ui/gt_labelui.cpp \
    object_ui/gt_loopui.cpp \
    object_ui/gt_datazoneui.cpp \
    dock_widgets/process/gt_processwizard.cpp \
    dock_widgets/process/gt_processfiltermodel.cpp \
    dock_widgets/process/pages/gt_calculatoroverviewpage.cpp \
    dock_widgets/process/pages/gt_calculatoroverviewmodel.cpp \
    dock_widgets/process/pages/gt_processcategoryitem.cpp \
    dock_widgets/process/pages/gt_calculatorsettingspage.cpp \
    dock_widgets/process/pages/gt_processwizardpage.cpp \
    dock_widgets/process/pages/gt_taskoverviewmodel.cpp \
    dock_widgets/process/pages/gt_processoverviewmodel.cpp \
    dock_widgets/process/pages/gt_processcalculatoritem.cpp \
    dock_widgets/process/pages/gt_processtaskitem.cpp \
    dock_widgets/process/pages/gt_processoverviewtree.cpp \
    dock_widgets/process/pages/gt_taskoverviewpage.cpp \
    dock_widgets/process/gt_processcomponentmodel.cpp \
    dock_widgets/properties/gt_propertymodel.cpp \
    dock_widgets/properties/delegates/gt_propertyiddelegate.cpp \
    dock_widgets/properties/delegates/gt_propertyunitdelegate.cpp \
    dock_widgets/properties/gt_propertytreeview.cpp \
    dock_widgets/properties/delegates/gt_propertyvaluedelegate.cpp \
    dock_widgets/properties/gt_abstractpropertyitem.cpp \
    dock_widgets/properties/items/gt_propertyitem.cpp \
    dock_widgets/properties/items/gt_propertycategoryitem.cpp \
    dock_widgets/properties/items/gt_propertymodeitem.cpp \
    dock_widgets/properties/items/gt_propertyobjectlinkitem.cpp \
    dock_widgets/properties/items/gt_propertyfilechooseritem.cpp \
    dock_widgets/properties/items/gt_propertylabelitem.cpp \
    dock_widgets/properties/items/gt_propertyaccessselectionitem.cpp \
    dock_widgets/properties/items/gt_propertydoublelistitem.cpp \
    dock_widgets/properties/editors/gt_propertyobjectlinkeditor.cpp \
    dock_widgets/properties/editors/gt_propertyfilechoosereditor.cpp \
    dock_widgets/properties/gt_propertyitemfactory.cpp \
    dock_widgets/post/gt_postmodel.cpp \
    dock_widgets/gt_customactionmenu.cpp \
    gt_objectuiaction.cpp \
    undoredo/gt_propertychangecommand.cpp \
    undoredo/gt_mementochangecommand.cpp \
    post/gt_posttemplate.cpp \
    post/gt_posttemplateitem.cpp \
    post/gt_templateviewer.cpp \
    post/gt_posttemplatefactory.cpp \
    post/gt_abstractitemui.cpp \
    post/gt_abstractpostwidget.cpp \
    post/gt_postaction.cpp \
    post/gt_abstractdataconfigwizard.cpp \
    post/gt_abstractchartprovider.cpp \
    post/gt_templateviewerlistwidget.cpp \
    post/gt_stringmapcontainer.cpp \
    post/gt_stringcontainer.cpp \
    post/gt_posttemplatepath.cpp \
    post/gt_abstractwizard.cpp \
    gt_extendedcalculatordata.cpp \
    gt_customprocesswizard.cpp \
    gt_processexecutor.cpp \
    gt_datamodel.cpp \
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
    exporter/dataZoneExporter/gt_datazonecsvexporter.cpp \
    exporter/dataZoneExporter/gt_datazonedatexporter.cpp \
    exporter/processExporter/gt_processexporter.cpp \
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
    mdi_items/collection_env/gt_collectionloadingwidget.cpp \
    post/gt_scrollarea.cpp \
    gt_applicationprivate.cpp \
    gt_objectuiactiongroup.cpp \
    gt_extendedtaskdata.cpp \
    dialogs/gt_inputdialog.cpp \
    dialogs/project_settings/gt_projectsettingsdialog.cpp \
    dialogs/project_settings/gt_projectsettingsexternalizationtab.cpp \
    dialogs/project_settings/gt_projectsettingstab.cpp \
    post/gt_scaleproperty.cpp \
    gt_stylesheets.cpp \
    gt_textfilterdelegate.cpp \
    gt_finishedprocessloadinghelper.cpp \
    importer/processImporter/gt_processimporter.cpp

RESOURCES += ../resources/icons/icons.qrc \
    ../resources/pixmaps/pixmaps.qrc \
    ../resources/qml/qml.qrc

unix {
    QMAKE_CXXFLAGS += -std=c++11
}

LIBS += -L$${BUILD_DEST}

CONFIG(debug, debug|release){
    # GTlab Utilities
    LIBS += -lGTlabNumerics-d -lGTlabLogging-d
    #GTlab Core
    LIBS += -lGTlabDataProcessor-d
    LIBS += -lGTlabCore-d
} else {
    # GTlab Utilities
    LIBS += -lGTlabNumerics -lGTlabLogging
    # GTLab Core
    LIBS += -lGTlabDataProcessor
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
