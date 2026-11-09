/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTPROCESSDOCKPAGE_H
#define GTPROCESSDOCKPAGE_H

#include "gt_gui_exports.h"
#include "gt_objectuiaction.h"

#include <QIcon>
#include <QList>
#include <QWidget>

#include <functional>

/**
 * @brief An additional view contributed to the Process Dock by a GUI module.
 *
 * The Process Dock owns instances created by registered factories.
 */
class GT_GUI_EXPORT GtProcessDockPage : public QWidget
{
    Q_OBJECT

public:
    using QWidget::QWidget;
    ~GtProcessDockPage() override = default;

    virtual QString pageId() const = 0;
    virtual QString title() const = 0;
    virtual QIcon icon() const { return {}; }

signals:
    void activationRequested();
};

namespace gt::gui
{

using ProcessDockPageFactory = std::function<GtProcessDockPage*()>;

/// Registers a factory for a page in the Process Dock.
GT_GUI_EXPORT void registerProcessDockPage(ProcessDockPageFactory factory);

/// Registers an action in the Process Dock's Add menu.
GT_GUI_EXPORT void registerProcessAddAction(GtObjectUIAction action);

/// Registers an action in a process component's context menu.
GT_GUI_EXPORT void registerProcessContextAction(GtObjectUIAction action);

/// Returns registered factories for use by the Process Dock.
GT_GUI_EXPORT const QList<ProcessDockPageFactory>& processDockPageFactories();

/// Returns registered Add actions for use by the Process Dock.
GT_GUI_EXPORT const QList<GtObjectUIAction>& processAddActions();

/// Returns registered Context actions for use by the Process Dock.
GT_GUI_EXPORT const QList<GtObjectUIAction>& processContextActions();

} // namespace gt::gui

#endif // GTPROCESSDOCKPAGE_H
