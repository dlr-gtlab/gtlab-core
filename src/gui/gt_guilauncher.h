/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef GT_GUILAUNCHER_H
#define GT_GUILAUNCHER_H

#include "gt_gui_exports.h"

#include <gt_platform.h>
#include <gt_globals.h>

#include <QObject>
#include <QPointer>

class GtObject;
class GtMdiItem;
class GtQmlAction;
class GtGuiLayoutAdapter;

class GT_GUI_EXPORT GtGuiLauncher final : public QObject
{
public:

    ~GtGuiLauncher() noexcept override;

    /**
     * @brief Returns the current instance of this launcher
     * @return Self
     */
    GT_NO_DISCARD
    static GtGuiLauncher& instance() noexcept;

    /**
     * @brief Sets the adapter responsible for laying out the gui items
     * @param adapter Adapter
     */
    void setAdapter(std::unique_ptr<GtGuiLayoutAdapter> adapter);

    /**
     * @brief setFocus
     * @param mdiId
     */
    void focus(const QString& itemId);

    /**
     * @brief Calls print routine of current gui item.
     */
    void printCurrentWindow();

    /**
     * @brief print
     * @param widget
     */
    void print(QWidget* widget);

    /**
     * @brief Returns true if current gui item is printable.
     * @return Is printable
     */
    bool isCurrentWindowPrintable() const;

    /**
     * @brief Returns true if gui item for given subwindow is printable.
     * @param widget
     * @return Is printable
     */
    bool isPrintable(QWidget* widget) const;

    /**
     * @brief Returns the toolbar actions of the currently opened gui item,
     * which should be placed into the editor context of the main toolbar.
     * @param widget The widget of the currently opened tab (from the gui item)
     * @return The list of actions
     */
    std::vector<GtQmlAction*> toolbarActions(QWidget* widget) const;

    /**
     * @brief Closes all active Gui items.
     */
    void closeAll();

public slots:

    GtMdiItem* open(const QString& className,
                    GtObject* data = nullptr,
                    const QString& customId = {});

    GtMdiItem* open(const QString& className,
                    const QString& customId);

private:

    GtGuiLauncher();

    /// Returns whether the launcher is null
    bool isNull() const noexcept;

    /// Returns whether
    bool isNewItemAllowed(GtMdiItem const& guiItem) const;

    struct Impl;
    std::unique_ptr<Impl> impl;

    /// wrapper for impl for const-correct access
    GT_NO_DISCARD
    Impl* pimpl() noexcept;
    /// wrapper for impl for const-correct access
    GT_NO_DISCARD
    Impl const* pimpl() const noexcept;
};

#endif // GT_GUILAUNCHER_H
