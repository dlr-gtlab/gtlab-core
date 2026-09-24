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
#include "gt_exportermetadata.h"
#include "gt_exporthandler.h"
#include "gt_exportmenu.h"
#include "gt_importhandler.h"
#include "gt_importmenu.h"
#include "gt_mdilauncher.h"
#include "gt_objectui.h"
#include "gt_project.h"
#include "gt_icons.h"

#include "gt_application.h"
#include "internal/gt_openwithmenu.h"

#include <QTreeView>
#include <QMenu>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QMenuBar>

namespace
{

/// helper method to get parent QWidget by skipping QMenu objects
inline QWidget*
findParentWidget(QWidget& menu)
{
    auto* parent = qobject_cast<QWidget*>(menu.parent());

    // skip if its a menu
    if (auto* parentMenu = qobject_cast<QMenu*>(parent))
    {
        return findParentWidget(*parentMenu);
    }
    if (auto* parentMenu = qobject_cast<QMenuBar*>(parent))
    {
        return findParentWidget(*parentMenu);
    }

    return parent;
}

inline int
getOrder(QAction const* action)
{
    if (!action) return 0;

    bool ok = true;
    int priority = action->data().toInt(&ok);
    return ok ? priority : 0;
}

/// Helper method to find the action, a new action needs to be inserted after
/// according to the order priority
QAction*
actionBefore(QMenu& menu, int priority)
{
    // append after all actions
    if (priority == gt::gui::OrderPriority::Last) return nullptr;

    QList<QAction*> const actions = menu.actions();
    if (actions.empty()) return nullptr;

    // prepend action
    if (priority == gt::gui::OrderPriority::First) return actions.first();

    for (QAction* action : actions)
    {
        if (getOrder(action) > priority) return action;
    }

    // all others have higher priority
    return nullptr;
}

/// helper method to add a ui-action to a menu. Visibility and status are
/// updated each time the menu is opened
QAction*
addActionBefore(QMenu& menu,
                QAction* before,
                const GtObjectUIAction& uiAction,
                GtObject* targetObj = nullptr,
                QObject* parentObj = nullptr)
{
    // separator
    if (uiAction.isSeparator())
    {
        QAction* separator = menu.insertSeparator(before);
        gt::gui::setOrderPriority(*separator, uiAction.orderPriority());
        return separator;
    }

    if (!uiAction.method()) return nullptr;

    QAction* action = new QAction(uiAction.text(), &menu);
    gt::gui::setOrderPriority(*action, uiAction.orderPriority());

    // insert at correct position
    menu.insertAction(before, action);

    if (uiAction.visibilityMethod() || uiAction.verificationMethod())
    {
        // -> use only one slot-call for both
        QObject::connect(&menu, &QMenu::aboutToShow,
                         action, [isVisible = uiAction.visibilityMethod(),
                                  isEnabled = uiAction.verificationMethod(),
                                  action,
                                  parentObj,
                                  targetObj](){
            // visibility
            if (isVisible) action->setVisible(isVisible(parentObj, targetObj));
            // verification
            if (isEnabled) action->setEnabled(isEnabled(parentObj, targetObj));
        });
    }

    // icon
    if (!uiAction.icon().isNull())
    {
        action->setIcon(uiAction.icon());
    }

    // shortcut
    if (!uiAction.shortCut().isEmpty())
    {
        action->setShortcut(uiAction.shortCut());
        action->setShortcutContext(Qt::ApplicationShortcut);

        if (QWidget* parent = findParentWidget(menu))
        {
            parent->addAction(action);
        }
    }

    // method
    QObject::connect(action, &QAction::triggered,
                     &menu, [method = uiAction.method(),
                             parentObj,
                             targetObj](){
        method(parentObj, targetObj);
    });
    return action;
}

/// helper method to add a ui-action to a menu. Inserts action at correct spot
/// according to its order priority
QAction*
addAction(QMenu& menu,
          const GtObjectUIAction& uiAction,
          GtObject* targetObj = nullptr,
          QObject* parentObj = nullptr)
{
    QAction* before = actionBefore(menu, uiAction.orderPriority());
    return addActionBefore(menu, before, uiAction, targetObj, parentObj);
}

//void
//addMenuBefore(QMenu& menu)
//{
//    QAction* before = actionBefore(menu, uiAction.orderPriority());
//    return addActionBefore(before, uiAction, menu, targetObj, parentObj);
//}

/// counts the visible actions (not separators)
inline int
countVisibleActions(QList<QAction*> const& actions)
{
    return std::count_if(std::begin(actions), std::end(actions),
                         [](QAction const* a){
        return a && !a->isSeparator() && a->isVisible();
    });
}

} // namespace

void
gt::gui::addToMenu(std::initializer_list<GtObjectUIAction> actions,
                   QMenu& menu,
                   GtObject* obj,
                   QObject* parent)
{
    for (GtObjectUIAction const& action : actions)
    {
        addAction(menu, action, obj, parent);
    }
}

void
gt::gui::addToMenu(const QList<GtObjectUIAction>& actions,
                   QMenu& menu,
                   GtObject* obj,
                   QObject* parent)
{
    for (GtObjectUIAction const& action : actions)
    {
        addAction(menu, action, obj, parent);
    }
}

void
gt::gui::addToMenu(GtObjectUIAction const& action,
                   QMenu& menu,
                   GtObject* obj,
                   QObject* parent)
{
    addAction(menu, action, obj, parent);
}

void
gt::gui::setOrderPriority(QAction& action, int priority)
{
    // use setData which is intended for "user" data
    action.setData(priority);
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
        // building section from last to first action using "insert before" mechanism
        QAction* before = actionBefore(menu, gt::gui::OrderPriority::AfterOpenWithAction);

        before = addActionBefore(menu, before, makeSeparator(gt::gui::OrderPriority::AfterOpenWithAction));

        if (openWithList.size() > 1)
        {
            auto* submenu = new GtOpenWithMenu(openWithList, &obj, &menu);
            assert(submenu->parent());

            QAction* openWithAction = menu.insertMenu(before, submenu);
            gt::gui::setOrderPriority(*openWithAction, gt::gui::OrderPriority::OpenWithAction);
            before = openWithAction;
        }

        auto lambda = [name = openWithList.first(), o = &obj](GtObject* target){
            gtMdiLauncher->open(name, o);
        };
        auto openAction = gt::gui::makeAction(QObject::tr("Open"), lambda)
                              .setIcon(gt::gui::icon::open())
                              .setOrderPriority(gt::gui::OrderPriority::OpenWithAction);

        before = addActionBefore(menu, before, openAction, &obj);

        addActionBefore(menu, before, makeSeparator(gt::gui::OrderPriority::BeforeOpenWithAction));
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
            QAction* before = actionBefore(menu, group.orderPriority());

            QMenu* submenu = new QMenu(group.name(), &menu);
            submenu->setIcon(group.icon());
            gt::gui::addToMenu(group.actions(), *submenu, &obj, data.ui);

            QAction* submenuAction = menu.insertMenu(before, submenu);
            gt::gui::setOrderPriority(*submenuAction, group.orderPriority());
        }
    }

    for (auto const& data : singleActions)
    {
        gt::gui::addToMenu(data.actions, menu, &obj, data.ui);
    }
}

QMenu*
gt::gui::addImportMenu(QMenu& menu, GtObject& obj)
{
    QList<GtImporterMetaData> importerList =
            gtImportHandler->importerMetaData(obj.metaObject()->className());

    if (!importerList.isEmpty())
    {
        // building section from last to first action using "insert before" mechanism
        QAction* before = actionBefore(menu, OrderPriority::AfterImportAction);

        before = addActionBefore(menu, before, makeSeparator(OrderPriority::AfterImportAction));

        GtImportMenu* submenu = new GtImportMenu(&obj, &menu);
        assert(submenu->parent());

        QAction* importAction = menu.insertMenu(before, submenu);
        setOrderPriority(*importAction, gt::gui::OrderPriority::ImportAction);
        before = importAction;

        addActionBefore(menu, before, makeSeparator(gt::gui::OrderPriority::BeforeImportAction));

        return submenu;
    }

    return nullptr;
}

QMenu*
gt::gui::addExportMenu(QMenu& menu, GtObject& obj)
{
    QList<GtExporterMetaData> exporterList =
            gtExportHandler->exporterMetaData(obj.metaObject()->className());

    if (!exporterList.isEmpty())
    {
        // building section from last to first action using "insert before" mechanism
        QAction* before = actionBefore(menu, OrderPriority::AfterExportAction);

        before = addActionBefore(menu, before, makeSeparator(OrderPriority::AfterExportAction));

        GtExportMenu* submenu = new GtExportMenu(&obj, &menu);
        assert(submenu->parent());

        QAction* exportAction = menu.insertMenu(before, submenu);
        setOrderPriority(*exportAction, gt::gui::OrderPriority::ExportAction);
        before = exportAction;

        addActionBefore(menu, before, makeSeparator(gt::gui::OrderPriority::BeforeExportAction));

        return submenu;
    }

    return nullptr;
}

GtObjectUIAction
gt::gui::makeDeleteAction(GtObject& obj)
{
    // delete object action
    if (obj.isDeletable())
    {
        auto lambda = [=](GtObject* target){
            if (GtProject* project = target->findRoot<GtProject*>())
            {
                gtDataModel->deleteFromModel(target);
            }
        };
        return makeAction(QObject::tr("Delete"), lambda)
            .setIcon(gt::gui::icon::delete_())
            .setShortCut(gtApp->getShortCutSequence("delete"))
            .setOrderPriority(gt::gui::OrderPriority::DeleteAction);
    }

    return {};
}

GtObjectUIAction
gt::gui::makeRenameAction(GtObject& obj,
                          const QModelIndex& idx,
                          QAbstractItemView& view)
{
    // rename object action
    if (obj.isRenamable() && idx.isValid())
    {
        auto lambda = [idx, v = &view](GtObject* target){
            v->edit(idx);
        };
        return gt::gui::makeAction(QObject::tr("Rename"), lambda)
            .setIcon(gt::gui::icon::rename())
            .setShortCut(gtApp->getShortCutSequence("rename"))
            .setOrderPriority(gt::gui::OrderPriority::RenameAction);
    }

    return {};
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

    if (view)
    {
        auto rename = makeRenameAction(obj, idx, *view);
        if (!rename.isEmpty())
        {
            QAction* before = addAction(menu, makeSeparator(gt::gui::OrderPriority::AfterRenameAction));
            before = addActionBefore(menu, before, rename, &obj);
            addActionBefore(menu, before, makeSeparator(gt::gui::OrderPriority::BeforeRenameAction));
        }
    }

    auto delete_ = makeDeleteAction(obj);
    if (!delete_.isEmpty())
    {
        QAction* before = addAction(menu, makeSeparator(gt::gui::OrderPriority::AfterDeleteAction));
        before = addActionBefore(menu, before, delete_, &obj);
        addActionBefore(menu, before, makeSeparator(gt::gui::OrderPriority::BeforeDeleteAction));
    }

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
renameObject(QKeyEvent& event, const GtObject& obj,
             QModelIndex const& idx, QAbstractItemView* view)
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

gt::gui::ApplicationTheme
gt::gui::theme()
{
    if (!gtApp) return ApplicationTheme::Bright;

    if (gtApp->inDarkMode()) return ApplicationTheme::Dark;

    return ApplicationTheme::Bright;
}

bool
gt::gui::isApplicationDarkTheme()
{
    return theme() == ApplicationTheme::Dark;
}
