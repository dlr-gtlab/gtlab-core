/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 31.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_explorerdock.h"

#include "gt_explorerview.h"
#include "gt_searchwidget.h"
#include "gt_project.h"
#include "gt_objectui.h"
#include "gt_mdilauncher.h"
#include "gt_application.h"
#include "gt_command.h"
#include "gt_settings.h"
#include "gt_logging.h"
#include "gt_textfilterdelegate.h"
#include "gt_deleteitemmessagebox.h"
#include "gt_stylesheets.h"
#include "gt_projectui.h"

#include "gt_datamodel.h"
#include "gt_styledmodel.h"
#include "gt_explorermodel.h"
#include "gt_icons.h"
#include "gt_guiutilities.h"


#include <QVBoxLayout>
#include <QFrame>
#include <QCheckBox>
#include <QMenu>
#include <QKeyEvent>

#include <algorithm>
#include <memory>

GtExplorerDock::GtExplorerDock() :
    m_model(nullptr),
    m_styledModel(nullptr)
{
    setObjectName(tr("Explorer"));

    QWidget* widget = new QWidget(this);
    setWidget(widget);

    QFrame* frame = new QFrame;
    QVBoxLayout* frameLayout = new QVBoxLayout;
    frameLayout->setSpacing(0);
    frameLayout->setContentsMargins(0, 0, 0, 0);

    m_view = new GtExplorerView(this);

    /// this line is used for optimization of the widget
    /// it should only be set to true, if it is guaranteed, that all
    /// rows are f the same size
    m_view->setUniformRowHeights(true);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    frameLayout->addWidget(m_view);
//    frameLayout->addWidget(m_resultView);

    frame->setLayout(frameLayout);
    frame->setAutoFillBackground(true);

    layout->addWidget(frame);

    QHBoxLayout* toolbarLayout = new QHBoxLayout;
    toolbarLayout->setContentsMargins(0, 0, 0, 0);
    frameLayout->addLayout(toolbarLayout);

    m_searchWidget = new GtSearchWidget;
    toolbarLayout->addWidget(m_searchWidget);

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
    connect(m_view, &GtExplorerView::deleteRequest, this, [this](){
        QList<QModelIndex> indexes = m_view->selectionModel()->selectedIndexes();
        deleteElements(indexes);
    });
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(customContextMenuDataView(QPoint)));
    connect(this, SIGNAL(selectedObjectChanged(GtObject*)),
            gtApp, SIGNAL(objectSelected(GtObject*)));
    connect(m_view, SIGNAL(clicked(QModelIndex)),
            SLOT(onClicked(QModelIndex)));
    connect(m_view, SIGNAL(mdiItemRequested(QModelIndex)),
            SLOT(onMdiItemRequested(QModelIndex)));
    connect(gtApp, SIGNAL(sessionChanged(QString)),
            SLOT(onSessionChanged()));
    connect(gtApp, SIGNAL(selectionByUuidRequested(QString)),
            SLOT(selectObjectByUuid(QString)));
    connect(gtDataModel, SIGNAL(triggerBeginResetDataModelView()),
            SLOT(beginResetView()), Qt::DirectConnection);
    connect(gtDataModel, SIGNAL(triggerEndResetDataModelView()),
            SLOT(endResetView()), Qt::DirectConnection);

    widget->setLayout(layout);


    m_model = new GtExplorerModel(this);
    m_styledModel = new GtStyledModel(m_model);
    m_styledModel->setSourceModel(gtDataModel);
    m_model->setSourceModel(m_styledModel);
    m_view->setModel(m_model);

    connect(m_view->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(onCurrentChanged(QModelIndex,QModelIndex)),
            Qt::UniqueConnection);

    onSessionChanged();
    //switchViewMode(m_viewModeBox->currentIndex());
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

        if (proj)
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
    if (!obj) return;

    QMenu menu(this);

    if (gt::gui::makeObjectContextMenu(menu, *obj, index, m_view))
    {
        menu.exec(QCursor::pos());
    }
}

void
GtExplorerDock::objectContextMenu(const QList<GtObject*>& objs)
{
    if (objs.isEmpty()) return;

    QList<GtProject*> projectsList;
    /// handle the special case of only projects selected
    bool allProjects = true;

    for (GtObject* obj : objs)
    {
        auto* proj = qobject_cast<GtProject*>(obj);
        if (!qobject_cast<GtProject*>(obj))
        {
            allProjects = false;
            break;
        }

        projectsList.append(proj);
    }

    bool oneDeletable = std::any_of(std::begin(objs), std::end(objs),
                                    [](const GtObject* obj) {
        return obj->isDeletable();
    });

    if (!oneDeletable && !allProjects)
    {
        return;
    }

    QMenu menu(this);

    QAction* actionDelete = nullptr;
    QAction* actionRemoveProjects = nullptr;

    if (oneDeletable)
    {
        actionDelete = menu.addAction(gt::gui::icon::delete_(),
                                      tr("Delete"));
    }

    if (allProjects)
    {
       actionRemoveProjects = menu.addAction(gt::gui::icon::projectRemove(),
                                             tr("Delete from session"));
    }

    QAction* a = menu.exec(QCursor::pos());

    if (a == actionDelete)
    {
        GtProject* project = objs.first()->findRoot<GtProject*>();

        if (project)
        {
            deleteElements(m_view->selectionModel()->selectedIndexes());
        }
    }
    else if (a == actionRemoveProjects)
    {
        gtInfo() << tr("Removing several projects...");

        if (std::any_of(std::begin(projectsList), std::end(projectsList), [](const GtProject* p) {
                return p->isOpen();
            }))
        {
            gtWarning() << tr("Open project cannot be removed from session");
            return;
        }

        for (GtProject* p : qAsConst(projectsList))
        {
            gtDataModel->deleteProject(p);
        }
    }

}

QModelIndex
GtExplorerDock::mapToSource(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return {};
    }

    QModelIndex tmp = index;

    if (!mapToSourceHelper(tmp, m_model))
    {
        return {};
    }

    if (!mapToSourceHelper(tmp, m_styledModel))
    {
        return {};
    }

    assert(tmp.model() == gtDataModel);

    return tmp;
}

GtObject*
GtExplorerDock::mapToObject(const QModelIndex& index)
{
    QModelIndex source = mapToSource(index);

    return gtDataModel->objectFromIndex(source);
}

QModelIndex
GtExplorerDock::mapFromSource(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return {};
    }

    QModelIndex tmp = index;

    if (!mapFromSourceHelper(tmp, gtDataModel, m_styledModel))
    {
        return {};
    }

    if (!mapFromSourceHelper(tmp, m_styledModel, m_model))
    {
        return {};
    }

    assert(tmp.model() == m_model);

    return  tmp;
}

void
GtExplorerDock::saveExpandStates()
{
    m_expandStates.clear();

    QModelIndexList list;

    list = m_model->getPersistentIndexList();

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
    if (!gtApp->currentProject())
    {
        return;
    }

    QStringList expStates;

    QModelIndexList list;

    list = m_model->getPersistentIndexList();

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
    QAbstractItemModel* model = nullptr;

    model = m_model;

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

QModelIndex
GtExplorerDock::firstSelectedIndex() const
{
    if (!m_view)
    {
        return {};
    }
    if (!m_view->selectionModel())
    {
        return {};
    }

    QModelIndexList indexlist = m_view->selectionModel()->selectedIndexes();

    if (indexlist.isEmpty())
    {
        return {};
    }

    return indexlist.first();
}


QModelIndexList
GtExplorerDock::listElements(QModelIndex const& parent) const
{
    QModelIndexList retVal;

    if (!parent.isValid())
    {
        return retVal;
    }

    int count = m_model->rowCount(parent);

    for (int i = 0; i < count; i++)
    {
        QModelIndex childIndex = m_model->index(i, 0, parent);

        if (childIndex.isValid() && !retVal.contains(childIndex))
        {
            retVal.append(childIndex);
            retVal += listElements(childIndex);
        }
    }

    return retVal;
}

void
GtExplorerDock::selectObjectByUuid(const QString& uuid)
{
    if (!gtApp->currentProject())
    {
        return;
    }

    QModelIndexList list;

    for(int i = 0; i < m_model->rowCount(); i++)
    {
        QModelIndex index = m_model->index(i, 0);

        for (const QModelIndex& i2 : listElements(index))
        {
            if (!list.contains(i2))
            {
                list.append(i2);
            }
        }
    }

    auto iter = std::find_if(std::begin(list), std::end(list),
                             [&uuid](const QModelIndex& index)
    {
        if (!index.isValid())
        {
            return false;
        }

        return index.data(GtCoreDatamodel::UuidRole).toString() == uuid;
    });

    if (iter != std::end(list))
    {
        if (GtObject* obj = mapToObject(*iter))
        {
            m_view->setCurrentIndex(*iter);
            m_view->scrollTo(*iter);
            return emit selectedObjectChanged(obj);
        }

        gtWarning() << tr("Cannot select the requested object");
    }
}

void
GtExplorerDock::keyPressEvent(QKeyEvent* event)
{
    assert(m_view);
    assert(m_view->selectionModel());

    auto first = firstSelectedIndex();

    GtObject* obj = mapToObject(first);
    if (!obj)
    {
        return;
    }

    gt::gui::handleObjectKeyEvent(*event, *obj, first, m_view);
}

void
GtExplorerDock::onSessionChanged()
{
    if (!gtApp->session())
    {
        return;
    }

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

    if (!index.isValid()) return;

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
    if (GtObject* item = mapToObject(index))
    {
        gt::gui::handleObjectDoubleClick(*item);
    }
}

void
GtExplorerDock::filterData(const QString& val)
{
    m_model->filterData(val);

    restoreExpandStates(m_expandStates);
}

void
GtExplorerDock::onSearchEnabled()
{
    saveExpandStates();
}

void
GtExplorerDock::customContextMenuDataView(const QModelIndex& indexOrigin)
{
    QModelIndexList const indexlist =
            m_view->selectionModel()->selectedIndexes();

    if (indexlist.empty()) return;

    if (indexlist.size() < 4)
    {
        if (GtObject* item = mapToObject(indexOrigin))
        {
            objectContextMenu(item, indexOrigin);
        }
        return;
    }

    GtObjectList selectedObjects;

    for (const QModelIndex& index : indexlist)
    {
        if (index.column() == 0)
        {
            if (auto* item = mapToObject(index))
            {
                selectedObjects << item;
            }
        }
    }

    objectContextMenu(selectedObjects);
}

void
GtExplorerDock::customContextMenuDataView(const QPoint& pos)
{
    QModelIndex indexUnderMouse = m_view->indexAt(pos);

    customContextMenuDataView(indexUnderMouse);
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

    for (QModelIndex const& index : indexList)
    {
        GtObject* obj = mapToObject(index);

        if (!obj) continue;

        if (!objects.contains(obj))
        {
            objects.append(obj);
        }
    }

    gtDataModel->reduceToParents(objects);

    QList<GtObject*> deletables;
    for (GtObject* obj : qAsConst(objects))
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

    if (deletables.size() != objects.size())
    {
        for (GtObject* obj : qAsConst(objects))
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
            break;
        }

        case QMessageBox::Cancel:
        {
            break;
        }

        default:
            break;
    }
}

/**
 * @brief Make sure that the project is expanded and selected
 */
void
GtExplorerDock::projectChangedEvent(GtProject* project)
{
    if (!project || !m_view) return;

    auto srcIdx = gtDataModel->indexFromObject(project);

    if (!srcIdx.isValid()) return;

    auto idx = mapFromSource(srcIdx);

    if (!idx.isValid()) return;

    m_view->expand(idx);

    if (m_view->selectionModel())
    {
        m_view->selectionModel()->select(
            idx, QItemSelectionModel::ClearAndSelect);
        m_view->scrollTo(idx);
    }
}
