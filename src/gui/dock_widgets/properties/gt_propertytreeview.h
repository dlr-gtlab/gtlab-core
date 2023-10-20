/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 28.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYTREEVIEW_H
#define GTPROPERTYTREEVIEW_H

#include "gt_treeview.h"

#include <QPointer>

class GtPropertyModel;
class GtTreeFilterModel;
class GtObject;
class GtProject;
class GtPropertyValueDelegate;
class GtPropertyStructContainer;

/**
 * @brief The GtPropertyTreeView class
 */
class GT_GUI_EXPORT GtPropertyTreeView : public GtTreeView
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent Parent widget.
     */
    explicit GtPropertyTreeView(GtObject* scope, QWidget* parent = nullptr);

    /**
     * @brief Resizes columns to contents.
     */
    void resizeColumns();

    /**
     * @brief filterModel
     * @return
     */
    GtTreeFilterModel* filterModel();

    GtPropertyModel* propertyModel();

    /**
     * @brief mapToSource
     * @param index
     * @return
     */
    QModelIndex mapToSource(const QModelIndex& index);

    /**
     * @brief mapFromSource
     * @param index
     * @return
     */
    QModelIndex mapFromSource(const QModelIndex& index);

    /**
     * @brief setObject
     * @param obj
     */
    void setObject(GtObject* obj, bool processEvents = true);

    /**
     * @brief setObject
     * @param obj
     */
    void setObject(GtObject* obj, GtPropertyStructContainer& container,
                   bool processEvents = true);

    /**
     * @brief setScope
     * @param project
     */
    void setScope(GtObject* project);

    /**
     * @brief setCategoryFilter
     * @param filter
     */
    void setCategoryFilter(const QStringList& filter);

protected:
    /**
     * @brief resizeEvent
     * @param event
     */
    void resizeEvent(QResizeEvent* event) override;

    /**
     * @brief mousePressEvent
     * @param event
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief moveCursor
     * @param cursorAction
     * @param modifiers
     * @return
     */
    QModelIndex moveCursor(CursorAction cursorAction,
                           Qt::KeyboardModifiers modifiers) override;

private:
    /// property model
    GtPropertyModel* m_model;

    /// filter model
    GtTreeFilterModel* m_filterModel;

private slots:
    /**
     * @brief onCollapsed
     * @param index
     */
    void onCollapsed(const QModelIndex& index);

    /**
     * @brief onExpanded
     * @param index
     */
    void onExpanded(const QModelIndex& index);

    /**
     * @brief Sets the first colum spanned for all roots.
     */
    void setRootsSpanned();

    /**
     * @brief Processes delete request for given index.
     * @param idx
     */
    void onDeleteRequested(const QModelIndex& idx);

};

#endif // GTPROPERTYTREEVIEW_H
