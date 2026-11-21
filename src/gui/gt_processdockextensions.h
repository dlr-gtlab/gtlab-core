/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTPROCESSDOCKEXTENSIONS_H
#define GTPROCESSDOCKEXTENSIONS_H

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
    /** Virtual destructor. */
    ~GtProcessDockPage() override = default;

    /**
     * @brief Returns the unique identifier for this page.
     *
     * The Process Dock stores this value in the page selector item data and
     * uses it to reject duplicate page registrations. It must be non-empty,
     * unique within the dock, and must not be "processes".
     */
    virtual QString pageId() const = 0;

    /** @brief Returns the user-visible title displayed in the page selector. */
    virtual QString title() const = 0;

    /**
     * @brief Returns the icon shown next to the page title.
     * @return The page icon, or an empty icon when no icon is needed.
     */
    virtual QIcon icon() const
    {
        return {};
    }

signals:
    /** Emit this signal to make the Process Dock select this page. */
    void activationRequested();
};

namespace gt::gui
{

using ProcessDockPageFactory = std::function<GtProcessDockPage*()>;

/** Registers a factory for a Process Dock page.
     * @param owner QObject that owns the registration, usually the module
     * interface. Contributions are ignored after the owner is destroyed.
     */
GT_GUI_EXPORT void registerProcessDockPage(QObject* owner,
                                           ProcessDockPageFactory factory);

/** Registers an action in the Process Dock's Add menu.
     * @param owner QObject used as the parent for action callbacks, usually
     * the module interface. Contributions are ignored after it is destroyed.
     */
GT_GUI_EXPORT void registerProcessAddAction(QObject* owner,
                                            GtObjectUIAction action);

/** Registers an action in a process component's context menu.
     * @param owner QObject used as the parent for action callbacks, usually
     * the module interface. Contributions are ignored after it is destroyed.
     */
GT_GUI_EXPORT void registerProcessContextAction(QObject* owner,
                                                GtObjectUIAction action);

} // namespace gt::gui

#endif // GTPROCESSDOCKEXTENSIONS_H
