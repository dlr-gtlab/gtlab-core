/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef GT_GUIFACTORY_H
#define GT_GUIFACTORY_H

#include "gt_gui_exports.h"

#include <gt_platform.h>
#include <gt_globals.h>

#include <memory>

#include <QMetaObject>

class QWidget;

class GtDockWidget;
class GtMdiItem;

class GT_GUI_EXPORT GtGuiFactory final
{
public:

    ~GtGuiFactory() noexcept;

    /**
     * @brief Returns the current instance of this factory
     * @return Self
     */
    GT_NO_DISCARD
    static GtGuiFactory& instance() noexcept;

    /**
     * @brief Attempts to instantiate a new dock widget of the corresponding
     * class name. Updates `lastError`
     * @param className Class name of the dock widget to instantiate
     * @return New widget. May be null.
     */
    GT_NO_DISCARD
    std::unique_ptr<GtDockWidget> makeDockWidget(QString const& className) const;

    /**
     * @brief Attempts to instantiate a new gui item of the corresponding
     * class name. Updates `lastError`
     * @param className Class name of the gui item to instantiate
     * @return New widget. May be null.
     */
    GT_NO_DISCARD
    std::unique_ptr<GtMdiItem> makeGuiItem(QString const& className) const;

    /**
     * @brief Registers a dock widget using the meta object provided.
     * The dock widget must be default invokable. Updates `lastError`
     * @param metaObject Meta object
     * @param moduleId Module Id this dock widget is associated with
     * @return Success
     */
    bool registerDockWidget(QMetaObject metaObject, QString const& moduleId);

    /**
     * @brief Registers a gui item using the meta object provided.
     * The gui item must be default invokable. Updates `lastError`
     * @param metaObject Meta object
     * @param moduleId Module Id this gui item is associated with
     * @return Success
     */
    bool registerGuiItem(QMetaObject metaObject, QString const& moduleId);

    /**
     * @brief Meta object id of the associated class
     * @param className Class name to query
     * @return Meta object. May be null if the class is unknown to this factory.
     */
    GT_NO_DISCARD
    QMetaObject* metaObject(QString const& anyClassName) const noexcept;

    /**
     * @brief Module id of the associated class
     * @param className Class name to query
     * @return Module id. May be empty if no module id was registered or
     * if the class is unknown to this factory.
     */
    GT_NO_DISCARD
    QString moduleId(QString const& className) noexcept;

    /**
     * @brief Display name of the associated class
     * @param className Class name to query
     * @return Display name. May be empty if no display name was registered or
     * if the class is unknown to this factory.
     */
    GT_NO_DISCARD
    QString displayName(QString const& className) noexcept;

    /**
     * @brief Returns whether the class name is a known dock widget
     * @param className Class name to check
     * @return Whether the class name is a known dock widget
     */
    GT_NO_DISCARD
    bool isDockWidget(QString const& className) noexcept;

    /**
     * @brief Returns whether the class name is a known gui item
     * @param className Class name to check
     * @return Whether the class name is a known gui item
     */
    GT_NO_DISCARD
    bool isGuiItem(QString const& className) noexcept;

    /**
     * @brief Returns whether the class name is known to this factory
     * @param className Class name to check
     * @return Whether the class name is known
     */
    GT_NO_DISCARD
    bool isKnownClass(QString const& className) noexcept;

    /**
     * @brief Returns a list of class names of all known dock widgets
     * @return List of class names of dock widgets
     */
    GT_NO_DISCARD
    QStringList knownDockWidgets() const;

    /**
     * @brief Returns a list of class names of all known gui items
     * @return List of class names of gui items
     */
    GT_NO_DISCARD
    QStringList knownGuiItems() const;

    /**
     * @brief Returns the last errorl. Only updated by a register call or when
     * attempting to instantiate a class.
     * @return
     */
    GT_NO_DISCARD
    QString const& lastError() const noexcept;

private:
    GtGuiFactory();

    struct Impl;
    std::unique_ptr<Impl> impl;

    /// wrapper for impl for const-correct access
    GT_NO_DISCARD
    Impl* pimpl() noexcept;
    /// wrapper for impl for const-correct access
    GT_NO_DISCARD
    Impl const* pimpl() const noexcept;
};

#endif // GT_GUIFACTORY_H
