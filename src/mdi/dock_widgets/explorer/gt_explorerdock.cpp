/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 31.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QFrame>
#include <QCheckBox>
#include <QComboBox>
#include <QMenu>
#include <QDebug>

#include "gt_explorerdock.h"
#include "gt_explorerview.h"
#include "gt_searchwidget.h"
#include "gt_project.h"
#include "gt_objectui.h"
#include "gt_openwithmenu.h"
#include "gt_customactionmenu.h"
#include "gt_mdilauncher.h"
#include "gt_application.h"
#include "gt_coreprocessexecutor.h"
#include "gt_importhandler.h"
#include "gt_importmenu.h"
#include "gt_exporthandler.h"
#include "gt_exportmenu.h"
#include "gt_command.h"
#include "gt_settings.h"
#include "gt_logging.h"
#include "gt_textfilterdelegate.h"
#include "gt_deleteitemmessagebox.h"
#include "gt_stylesheets.h"

#include "gt_datamodel.h"
#include "gt_styledmodel.h"
#include "gt_explorermodel.h"
#include "gt_resultsmodel.h"

GtExplorerDock::GtExplorerDock() :
    m_viewMode(DATA_VIEW),
    m_model(Q_NULLPTR),
    m_styledModel(Q_NULLPTR),
    m_resultsModel(Q_NULLPTR)
{
    setObjectName(tr("Explorer"));

    QWidget* widget = new QWidget(this);
    setWidget(widget);

    QFrame* frame = new QFrame;
    QVBoxLayout* frameLayout = new QVBoxLayout;

    frameLayout->setContentsMargins(0, 0, 0, 0);

    m_view = new GtExplorerView(this);

    /// this line is used for optimization of the widget
    /// it should only be set to true, if it is guaranteed, that all
    /// rows are f the same size
    m_view->setUniformRowHeights(true);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    frameLayout->addWidget(m_view);
//    frameLayout->addWidget(m_resultView);

    frame->setLayout(frameLayout);
    QPalette palette = frame->palette();
    palette.setColor(backgroundRole(), QColor(255, 255, 255));
    frame->setPalette(palette);
    frame->setAutoFillBackground(true);
    frame->setFrameShape(m_view->frameShape());
    frame->setFrameStyle(m_view->frameStyle());
    frame->setFrameShadow(m_view->frameShadow());

    layout->addWidget(frame);

    QHBoxLayout* toolbarLayout = new QHBoxLayout;
    toolbarLayout->setContentsMargins(0, 0, 0, 0);
    frameLayout->addLayout(toolbarLayout);

    m_searchWidget = new GtSearchWidget;
    toolbarLayout->addWidget(m_searchWidget);

    m_viewModeBox = new QComboBox;
    m_viewModeBox->setStyleSheet(GtStyleSheets::selectionComboBox("85", "110"));

    m_viewModeBox->addItem(gtApp->icon(QStringLiteral("emptyIcon_16.png")),
                           QStringLiteral("Data View"));
    m_viewModeBox->addItem(gtApp->icon(QStringLiteral("dataIcon_16.png")),
                           QStringLiteral("Results View"));

    QFont font = m_viewModeBox->font();
    font.setPointSize(8);
    m_viewModeBox->setFont(font);
    m_viewModeBox->setFrame(true);
    m_viewModeBox->setLayoutDirection(Qt::LeftToRight);
    m_viewModeBox->setMinimumWidth(85);
    m_viewModeBox->setMaximumHeight(18);
    m_viewModeBox->setIconSize(QSize(12, 12));

    toolbarLayout->addWidget(m_viewModeBox);

    m_view->setFrameStyle(QTreeView::NoFrame);
    m_view->setEditTriggers(QTreeView::SelectedClicked);
    m_view->setIconSize(QSize(16, 16));

    GtTextFilterDelegate* delegate = new GtTextFilterDelegate(this);

    m_view->setItemDelegate(delegate);

//    if (gtApp->devMode())
//    {
//        QCheckBox* fullModeCheck = new QCheckBox("Show Full Session");
//        fullModeCheck->setChecked(false);
//        layout->addWidget(fullModeCheck);

//        connect(fullModeCheck, SIGNAL(toggled(bool)), m_view,
//                SLOT(setShowFullData(bool)));
//    }

    connect(m_searchWidget, SIGNAL(textEdited(QString)),
            SLOT(filterData(QString)));
    connect(m_searchWidget, SIGNAL(textChanged(QString)),
            SLOT(filterData(QString)));
    connect(m_searchWidget, SIGNAL(searchEnabled()),
            SLOT(onSearchEnabled()));
    connect(m_view, SIGNAL(searchRequest()), m_searchWidget,
            SLOT(enableSearch()));
    connect(m_view, &GtExplorerView::deleteElementsRequest,
            this, &GtExplorerDock::deleteElements);
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(customContextMenuDataView(QPoint)));
    connect(m_viewModeBox, SIGNAL(currentIndexChanged(int)),
            SLOT(switchViewMode(int)));
    connect(this, SIGNAL(selectedObjectChanged(GtObject*)),
            gtApp, SIGNAL(objectSelected(GtObject*)));
    connect(m_view, SIGNAL(clicked(QModelIndex)),
            SLOT(onClicked(QModelIndex)));
    connect(m_view, SIGNAL(mdiItemRequested(QModelIndex)),
            SLOT(onMdiItemRequested(QModelIndex)));
    connect(gtApp, SIGNAL(sessionChanged(QString)),
            SLOT(onSessionChanged()));
    connect(gtDataModel, SIGNAL(triggerBeginResetDataModelView()),
            SLOT(beginResetView()));
    connect(gtDataModel, SIGNAL(triggerEndResetDataModelView()),
            SLOT(endResetView()));

    widget->setLayout(layout);


    m_model = new GtExplorerModel(this);
    m_resultsModel = new GtResultsModel(m_model);
    m_styledModel = new GtStyledModel(m_model);
    m_styledModel->setSourceModel(gtDataModel);
    m_model->setSourceModel(m_styledModel);
    m_resultsModel->setSourceModel(m_model);
    m_view->setModel(m_model);

    connect(m_view->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(onCurrentChanged(QModelIndex,QModelIndex)),
            Qt::UniqueConnection);

    onSessionChanged();
    switchViewMode(m_viewModeBox->currentIndex());
}

GtExplorerDock::~GtExplorerDock()
{
    if (gtApp->settings()->openLastProject())
    {
       saveExpandStatesToSettings();
    }
}

Qt::DockWidgetArea GtExplorerDock::getDockWidgetArea()
{
    return Qt::LeftDockWidgetArea;
}

void
GtExplorerDock::initAfterStartup()
{
    if (gtApp->settings()->openLastProject())
    {
        GtProject* proj = gtApp->findProject(gtApp->settings()->lastProject());

        if (proj != Q_NULLPTR)
        {
            gtDataModel->openProject(proj);

            // restore expand states
            QStringList expandStates =
                    gtApp->settings()->explorerExpandStates();

            if (!expandStates.isEmpty())
            {
                // update root uuid
                int idx = expandStates.indexOf(QStringLiteral("root"));

                if (idx >= 0)
                {
                    expandStates[idx] = proj->uuid();
                }

                restoreExpandStates(expandStates);
            }

            QModelIndex srcIdx = gtDataModel->indexFromObject(proj);

            QModelIndex idx = mapFromSource(srcIdx);

            if (idx.isValid())
            {
                m_view->scrollTo(idx, QTreeView::PositionAtTop);
            }
        }
    }
}

void
GtExplorerDock::objectContextMenu(GtObject* obj, const QModelIndex& index)
{
    if (obj == NULL)
    {
        return;
    }

    QMenu menu(this);

    QAction* actionOpen = menu.addAction(tr("Open"));
    actionOpen->setIcon(gtApp->icon("openIcon_16.png"));

    actionOpen->setVisible(false);

    GtObjectUI* oui = gtApp->objectUI(obj);
    QString openStr;

    if (oui)
    {
        // open with
        QStringList openList = oui->openWith(obj);

        if (!openList.isEmpty())
        {
            actionOpen->setVisible(true);
            openStr = openList.first();
            if (openList.size() > 1)
            {
                GtOpenWithMenu* openWith = new GtOpenWithMenu(openList,
                                                              obj,
                                                              &menu);
                menu.addMenu(openWith);
            }
            menu.addSeparator();
        }

        // custom menu
        if (oui->hasActionGroups())
        {
            foreach(GtObjectUIActionGroup actGroup, oui->actionGroups())
            {
                QMenu* submenu = new QMenu(actGroup.name());
                GtCustomActionMenu* cmenu =
                        new GtCustomActionMenu(actGroup.actions(), obj, oui,
                                               submenu);

                Q_UNUSED(cmenu)
                submenu->setIcon(gtApp->icon(actGroup.icon()));
                menu.addMenu(submenu);
            }
        }

        if (oui->hasActions())
        {
            GtCustomActionMenu* cmenu = new GtCustomActionMenu(oui->actions(),
                                                               obj, oui,
                                                               &menu);

            Q_UNUSED(cmenu)

        }

        menu.addSeparator();
    }

    // importer menu
    QList<GtImporterMetaData> importerList =
            gtImportHandler->importerMetaData(obj->metaObject()->className());

    if (!importerList.isEmpty())
    {
        GtImportMenu* imenu = new GtImportMenu(obj, &menu);

        menu.addMenu(imenu);

        menu.addSeparator();
    }

    // exporter menu
    QList<GtExporterMetaData> exporterList =
            gtExportHandler->exporterMetaData(obj->metaObject()->className());

    if (!exporterList.isEmpty())
    {
        GtExportMenu* emenu = new GtExportMenu(obj, &menu);

        menu.addMenu(emenu);

        menu.addSeparator();
    }

//    QAction* actionDelete =
//            menu.addAction(gtApp->icon("closeIcon_16.png"),
//                           tr("Delete From Session"));

//    menu.addSeparator();

    // rename object action
    QAction* actrename = menu.addAction("Rename");
    actrename->setIcon(gtApp->icon("inputIcon_16.png"));

    if (obj->isRenamable())
    {
        menu.addSeparator();
    }
    else
    {
        actrename->setVisible(false);
    }

    QAction* actionDelete = menu.addAction(
                                gtApp->icon(QStringLiteral("closeIcon_16.png")),
                                tr("Delete"));

    // delete object action
    if (obj->isDeletable())
    {
        menu.addSeparator();
    }
    else
    {
        actionDelete->setVisible(false);
    }

    //QAction* actionDetails =
    //        menu.addAction(gtApp->icon(QStringLiteral("infoIcon_16.png")),
    //                       tr("Details"));
    //
    //actionDetails->setEnabled(false);
    bool anyVisible = false;

    foreach (QAction* a, menu.actions())
    {
        if (!a->isSeparator())
        {
            if (a->isVisible())
            {
                anyVisible = true;
                //gtDebug() << "Visible:" << a->objectName();
                break;
            }
        }
    }

    if (!anyVisible)
    {
        menu.clear();
        return;
    }

    // catch action
    QAction* a = menu.exec(QCursor::pos());

    if (a == actionOpen)
    {
        gtMdiLauncher->open(openStr, obj);
    }
    else if (a == actionDelete)
    {
        GtProject* project = obj->findRoot<GtProject*>();

        if (project != Q_NULLPTR)
        {
            gtDataModel->deleteFromModel(obj);
        }
    }
    else if (a == actrename)
    {
        QModelIndex idx = mapFromSource(index);

        if (idx.isValid())
        {
            m_view->edit(idx);
        }
    }
}

void
GtExplorerDock::objectContextMenu(const QList<GtObject*>& objs)
{
    if (objs.isEmpty())
    {
        gtError() << "object list is empty!";
        return;
    }

    bool oneDeletable = false;

    foreach (GtObject* obj, objs)
    {
        if (obj->isDeletable())
        {
            oneDeletable = true;
            break;
        }
    }

    if (!oneDeletable)
    {
        return;
    }

    QMenu menu(this);

    QAction* actionDelete = menu.addAction(
                                gtApp->icon(QStringLiteral("closeIcon_16.png")),
                                tr("Delete"));

    QAction* a = menu.exec(QCursor::pos());

    if (a == actionDelete)
    {
        GtProject* project = objs.first()->findRoot<GtProject*>();

        if (project != Q_NULLPTR)
        {
            deleteElements(m_view->selectionModel()->selectedIndexes());
        }
    }

}

QModelIndex
GtExplorerDock::mapToSource(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    QModelIndex tmp = index;

    // view mode
    if (m_viewMode == RESULTS_VIEW)
    {
        if (!mapToSourceHelper(tmp, m_resultsModel))
        {
            return QModelIndex();
        }
    }

    if (!mapToSourceHelper(tmp, m_model))
    {
        return QModelIndex();
    }

    if (!mapToSourceHelper(tmp, m_styledModel))
    {
        return QModelIndex();
    }

    return tmp;
}

QModelIndex
GtExplorerDock::mapFromSource(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    QModelIndex tmp = index;

    if (!mapFromSourceHelper(tmp, gtDataModel, m_styledModel))
    {
        return QModelIndex();
    }

    if (!mapFromSourceHelper(tmp, m_styledModel, m_model))
    {
        return QModelIndex();
    }

    // view mode
    if (m_viewMode == RESULTS_VIEW)
    {
        if (!mapFromSourceHelper(tmp, m_model, m_resultsModel))
        {
            return QModelIndex();
        }
    }

    return  tmp;
}

void
GtExplorerDock::saveExpandStates()
{
    m_expandStates.clear();

    QModelIndexList list;

    if (m_viewMode == DATA_VIEW)
    {
        list = m_model->getPersistentIndexList();
    }
    else
    {
        list = m_resultsModel->getPersistentIndexList();
    }

    foreach (QModelIndex index, list)
    {
        if (m_view->isExpanded(index))
        {
            m_expandStates << index.data(GtCoreDatamodel::UuidRole).toString();
        }
    }
}

void
GtExplorerDock::saveExpandStatesToSettings()
{
    if (gtApp->currentProject() == Q_NULLPTR)
    {
        return;
    }

    QStringList expStates;

    QModelIndexList list;

    if (m_viewMode == DATA_VIEW)
    {
        list = m_model->getPersistentIndexList();
    }
    else
    {
        list = m_resultsModel->getPersistentIndexList();
    }

    foreach (QModelIndex index, list)
    {
        if (m_view->isExpanded(index))
        {
            QString uuid = index.data(GtCoreDatamodel::UuidRole).toString();

            if (uuid == gtApp->currentProject()->uuid())
            {
                uuid = QStringLiteral("root");
            }

            expStates << uuid;
        }
    }

    gtApp->settings()->setExplorerExpandStates(expStates);
}

void
GtExplorerDock::restoreExpandStates(const QStringList& list)
{
    QAbstractItemModel* model = Q_NULLPTR;

    if (m_viewMode == DATA_VIEW)
    {
        model = m_model;
    }
    else
    {
        model = m_resultsModel;
    }

    m_view->setUpdatesEnabled(false);

    restoreExpandStatesHelper(list, model, model->index(0, 0, QModelIndex()));

    m_view->setUpdatesEnabled(true);
}

void
GtExplorerDock::restoreExpandStatesHelper(const QStringList& expandedItems,
                                          QAbstractItemModel* model,
                                          QModelIndex startIndex)
{
    foreach (const QString& item, expandedItems)
    {
        QModelIndexList matches = model->match(startIndex,
                                               GtCoreDatamodel::UuidRole,
                                               QVariant::fromValue(item));

        foreach (QModelIndex index, matches)
        {
            m_view->setExpanded(index, true);
            restoreExpandStatesHelper(expandedItems, model,
                                      model->index(0, 0, index));
        }
    }
}

void
GtExplorerDock::switchViewMode(int mode)
{
    QStringList lastExpandState = m_expandStates;
    saveExpandStates();

    if (mode == 0)
    {
        m_viewMode = DATA_VIEW;
        m_view->setModel(m_model);
    }
    else
    {
        m_viewMode = RESULTS_VIEW;
        m_view->setModel(m_resultsModel);
    }

    if (lastExpandState.isEmpty())
    {
        restoreExpandStates(m_expandStates);
    }
    else
    {
        restoreExpandStates(lastExpandState);
    }

    filterData(m_searchWidget->text());

    connect(m_view->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(onCurrentChanged(QModelIndex,QModelIndex)),
            Qt::UniqueConnection);
}

void
GtExplorerDock::onSessionChanged()
{
    if (gtApp->session() == Q_NULLPTR)
    {
        return;
    }

//    qDebug() << "#### GtExplorerDock::onSessionChanged()";

    // connect data changed signals
    connect(gtApp->session(), SIGNAL(dataChanged(GtObject*)),
            gtDataModel, SLOT(onObjectDataChanged(GtObject*)));
    connect(gtApp->session(),
            SIGNAL(dataChanged(GtObject*,GtAbstractProperty*)),
            gtDataModel,
            SLOT(onObjectDataChanged(GtObject*,GtAbstractProperty*)));

    m_styledModel->setSourceModel(gtDataModel);
}

void
GtExplorerDock::onCurrentChanged(const QModelIndex& current,
                                 const QModelIndex& /*previous*/)
{
    QModelIndex index = mapToSource(current);

    if (!index.isValid())
    {
        return;
    }

    GtObject* obj = gtDataModel->objectFromIndex(index);

    emit selectedObjectChanged(obj);
}

void
GtExplorerDock::onClicked(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    onCurrentChanged(index);
}

void
GtExplorerDock::onMdiItemRequested(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    QModelIndex srcIndex = mapToSource(index);

    GtObject* item = gtDataModel->objectFromIndex(srcIndex);

    if (item == Q_NULLPTR)
    {
        return;
    }

    GtObjectUI* oui = gtApp->objectUI(item);

    if (oui == Q_NULLPTR)
    {
        return;
    }

    oui->doubleClicked(item);

    GtProject* project = qobject_cast<GtProject*>(item);

    if (project != Q_NULLPTR)
    {
        m_view->expand(index);
    }
}

void
GtExplorerDock::filterData(const QString& val)
{
    if (m_viewMode == DATA_VIEW)
    {
        m_resultsModel->setFilterFixedString(QString());
        m_model->filterData(val);
    }
    else
    {
        m_model->filterData(QString());
        m_resultsModel->setFilterRegExp(val);
    }

    //    if (m_model)
//    {
//        m_model->filterData(val);
//    }
    restoreExpandStates(m_expandStates);
}

void
GtExplorerDock::onSearchEnabled()
{
    saveExpandStates();
}

void
GtExplorerDock::customContextMenuDataView(const QPoint& pos)
{
    QModelIndexList indexlist = m_view->selectionModel()->selectedIndexes();

    if (indexlist.size() > 0 && indexlist.size() < 4)
    {
        QModelIndex indexUnderMouse = m_view->indexAt(pos);

        indexUnderMouse = mapToSource(indexUnderMouse);

        if (!indexUnderMouse.isValid())
        {
            qDebug() << "indexUnderMouse invalid!";
            return;
        }

        GtObject* item = gtDataModel->objectFromIndex(indexUnderMouse);

        if (item != Q_NULLPTR)
        {
            objectContextMenu(item, indexUnderMouse);
        }
    }
    else if (indexlist.size() > 3)
    {
        GtObjectList selectedObjects;

        foreach (const QModelIndex& index, indexlist)
        {
            if (index.column() == 0)
            {
                QModelIndex mappedIndex = mapToSource(index);
                GtObject* item = gtDataModel->objectFromIndex(mappedIndex);

                if (item != Q_NULLPTR)
                {
                    selectedObjects << item;
                }
            }
        }

        objectContextMenu(selectedObjects);
    }
}

void
GtExplorerDock::beginResetView()
{
    saveExpandStates();
}

void
GtExplorerDock::endResetView()
{
    restoreExpandStates(m_expandStates);

    connect(m_view->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(onCurrentChanged(QModelIndex,QModelIndex)),
            Qt::UniqueConnection);
}

void
GtExplorerDock::deleteElements(const QList<QModelIndex>& indexList)
{
    if (indexList.isEmpty())
    {
        return;
    }

    QList<GtObject*> objects;

    foreach(QModelIndex index, indexList)
    {
        if (!index.isValid())
        {
            continue;
        }

        QModelIndex srcIndex = mapToSource(index);

        if (!srcIndex.isValid())
        {
            continue;
        }

        if (srcIndex.model() != gtDataModel)
        {
            continue;
        }

        GtObject* obj = gtDataModel->objectFromIndex(srcIndex);

        if (obj == Q_NULLPTR)
        {
            continue;
        }

        if (!objects.contains(obj))
        {
            objects.append(obj);
        }
    }

    gtDataModel->reduceToParents(objects);

    QList<GtObject*> deletables;
    foreach(GtObject* obj, objects)
    {
        if (obj->isDeletable())
        {
            deletables.append(obj);
        }
    }

    if (deletables.isEmpty())
    {
        gtInfo() << "No deletable objects selected!";
        return;
    }
    else if (deletables.size() != objects.size())
    {
        foreach(GtObject* obj, objects)
        {
            if (!deletables.contains(obj))
            {
                gtInfo() << "Item is not deletable:" << obj->objectName();
            }
        }
    }

    QString str1;
    QString str2;

    if (deletables.size() == 1)
    {
        str1 = tr("Delete Item");
        str2 = deletables.first()->objectName();
    }
    else
    {
        str1 = tr("Delete Items");
        str2 = tr("Selected Objects");
    }

    GtDeleteItemMessageBox mb(str1, str2);

    int ret = mb.exec();

    switch (ret)
    {
        case QMessageBox::Yes:
        {
            gtDataModel->deleteFromModel(deletables);
        }

        case QMessageBox::Cancel:
        {
            break;
        }

        default:
            break;
    }
}