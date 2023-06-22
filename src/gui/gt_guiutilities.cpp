/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 1.3.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_guiutilities.h"
#include "gt_application.h"
#include "gt_datamodel.h"
#include "gt_customactionmenu.h"
#include "gt_exportermetadata.h"
#include "gt_exporthandler.h"
#include "gt_exportmenu.h"
#include "gt_importhandler.h"
#include "gt_importmenu.h"
#include "gt_mdilauncher.h"
#include "gt_objectui.h"
#include "gt_project.h"
#include "gt_icons.h"

#include "gt_projectui.h"
#include "gt_application.h"
#include "internal/gt_openwithmenu.h"

#include <QTreeView>
#include <QMenu>
#include <QKeyEvent>
#include <QAbstractItemView>

/// counts the visible actions (not separators)
inline int
countVisibleActions(QList<QAction*> const& actions)
{
    return std::count_if(std::begin(actions), std::end(actions),
                         [](QAction const* a){
        return !a->isSeparator() && a->isVisible();
    });
}

/// adds the actions to the menu
inline void
addToMenu(const QList<GtObjectUIAction>& actions, QMenu& menu,
          GtObject& obj, QObject* parent = {})
{
    // menu will take ownership
    new GtCustomActionMenu(actions, &obj, parent, &menu);
}

/// adds the action to the menu
inline void
addToMenu(GtObjectUIAction const& action, QMenu& menu,
          GtObject& obj, QObject* parent = {})
{
    addToMenu(QList<GtObjectUIAction>{action}, menu, obj, parent);
}

/// adds the "open with" actions to the menu
inline void
addOpenWithActions(QMenu& menu, GtObject& obj)
{
    QList<GtObjectUI*> const ouis = gtApp->objectUI(&obj);

    QStringList openWithList;
    for (auto* oui : ouis)
    {
        openWithList << oui->openWith(&obj);
    }

    // open with
    if (!openWithList.isEmpty())
    {
        GtObjectUIAction openAction(QObject::tr("Open"),
                    [name = openWithList.first(), o = &obj](GtObject* target){
            gtMdiLauncher->open(name, o);
        });

        openAction.setIcon(gt::gui::icon::open());

        addToMenu(openAction, menu, obj);

        if (openWithList.size() > 1)
        {
            menu.addMenu(new GtOpenWithMenu(openWithList, &obj, &menu));
        }

        menu.addSeparator();
    }
}

/// adds the custom actions from the object uis to the menu
inline void
addCustomActions(QMenu& menu, GtObject& obj)
{
    struct ActionData
    {
        GtObjectUI* ui{};
        QList<GtObjectUIAction> actions{};
    };

    struct GroupActionData
    {
        GtObjectUI* ui{};
        QList<GtObjectUIActionGroup> groups{};
    };

    auto const actionsBefore = menu.actions();

    QList<GtObjectUI*> const ouis = gtApp->objectUI(&obj);

    QVector<GroupActionData> actionGroups;
    QVector<ActionData> singleActions;

    for (auto* oui : ouis)
    {
        if (oui->hasActionGroups())
        {
            actionGroups.push_back({ oui, oui->actionGroups() });
        }

        if (oui->hasActions())
        {
            singleActions.push_back({ oui, oui->actions() });
        }
    }

    // custom menu
    for (auto const& data : actionGroups)
    {
        for (auto const& group : data.groups)
        {
            QMenu* submenu = menu.addMenu(group.name());
            submenu->setIcon(group.icon());

            addToMenu(group.actions(), *submenu, obj, data.ui);
        }
    }

    for (auto const& data : singleActions)
    {
        addToMenu(data.actions, menu, obj, data.ui);
    }

    auto actionsAfter = menu.actions();

    for (auto* action : actionsBefore)
    {
        actionsAfter.removeOne(action);
    }

    if (countVisibleActions(actionsAfter) > 0)
    {
        menu.addSeparator();
    }
}

/// adds the import menu actions to the menu
inline void
addImportMenu(QMenu& menu, GtObject& obj)
{
    QList<GtImporterMetaData> importerList =
            gtImportHandler->importerMetaData(obj.metaObject()->className());

    if (!importerList.isEmpty())
    {
        GtImportMenu* imenu = new GtImportMenu(&obj, &menu);

        menu.addMenu(imenu);

        menu.addSeparator();
    }
}

/// adds the export menu actions to the menu
inline void
addExportMenu(QMenu& menu, GtObject& obj)
{
    QList<GtExporterMetaData> exporterList =
            gtExportHandler->exporterMetaData(obj.metaObject()->className());

    if (!exporterList.isEmpty())
    {
        GtExportMenu* emenu = new GtExportMenu(&obj, &menu);

        menu.addMenu(emenu);

        menu.addSeparator();
    }
}

/// adds the actions for removing and renaming an object to the menu
inline void
addObjectActions(QMenu& menu,
                 GtObject& obj,
                 QModelIndex const& idx,
                 QAbstractItemView* view)
{
    // rename object action
    if (obj.isRenamable() && idx.isValid() && view)
    {
        GtObjectUIAction renameAction(QObject::tr("Rename"),
                                      [=](GtObject* target){
            assert(idx.isValid());
            assert(view);
            view->edit(idx);
        });

        renameAction.setIcon(gt::gui::icon::rename())
                    .setShortCut(gtApp->getShortCutSequence("rename"));

        new GtCustomActionMenu({renameAction}, &obj, nullptr, &menu);

        menu.addSeparator();
    }

    // delete object action
    if (obj.isDeletable())
    {
        GtObjectUIAction deleteAction(QObject::tr("Delete"),
                                      [](GtObject* target){
            if (GtProject* project = target->findRoot<GtProject*>())
            {
                gtDataModel->deleteFromModel(target);
            }
        });

        deleteAction.setIcon(gt::gui::icon::delete_())
                .setShortCut(gtApp->getShortCutSequence("delete"));

        new GtCustomActionMenu({deleteAction}, &obj, nullptr, &menu);

        menu.addSeparator();
    }
}

int
gt::gui::makeObjectContextMenu(QMenu& menu,
                               GtObject& obj,
                               QModelIndex const& idx,
                               QAbstractItemView* view)
{
    addOpenWithActions(menu, obj);

    addCustomActions(menu, obj);

    addImportMenu(menu, obj);

    addExportMenu(menu, obj);

    addObjectActions(menu, obj, idx, view);

    return countVisibleActions(menu.actions());
}

void
gt::gui::handleObjectDoubleClick(GtObject& obj)
{
    if (GtObjectUI* oui = gtApp->defaultObjectUI(&obj))
    {
        oui->doubleClicked(&obj);
    }
}

/// opens the context menu in the view for obj
inline void
openContextMenu(QKeyEvent& event,
                GtObject& obj,
                QModelIndex const& idx,
                QAbstractItemView* view)
{
    if (idx.isValid() && view)
    {
        emit view->customContextMenuRequested(view->visualRect(idx).center());
        event.accept();
    }
}

/// logic for renaming obj in the view
inline bool
renameObject(QKeyEvent& event, GtObject& obj, QModelIndex const& idx, QAbstractItemView* view)
{
    if (obj.isRenamable() && idx.isValid() && view)
    {
        view->edit(idx);
        return true;
    }
    return false;
}

/// checks the actions registered from the object uis and invokes the actions
/// that match the shortcut
inline void
shortcutAction(QKeyEvent& event, GtObject& obj)
{
    /// General approach to read Shortcut from ui
    for (auto* oui : gtApp->objectUI(&obj))
    {
        assert(oui);

        // only add single actions
        for (auto const& a : oui->actions())
        {
            QKeySequence k = a.shortCut();

            if (gtApp->compareKeyEvent(&event, k))
            {
                a.method()(oui, &obj);
            }
        }
    }
}

void
gt::gui::handleObjectKeyEvent(QKeyEvent& event,
                              GtObject& obj,
                              const QModelIndex& idx,
                              QAbstractItemView* view)
{
    if (gtApp->compareKeyEvent(&event, "openContextMenu"))
    {
        return openContextMenu(event, obj, idx, view);
    }

    if (gtApp->compareKeyEvent(&event, "rename"))
    {
        // object may have custom rename action registered
        if (renameObject(event, obj, idx, view)) return;
    }

    /// sanme as double click
    if (gtApp->compareKeyEvent(&event, QKeySequence(Qt::Key_Return)))
    {
        return handleObjectDoubleClick(obj);
    }

    shortcutAction(event, obj);
}
