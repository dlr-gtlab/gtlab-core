/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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
     * @brief drawRow
     * @param painter
     * @param option
     * @param index
     */
    void drawRow(QPainter* painter,
                 const QStyleOptionViewItem& option,
                 const QModelIndex& index) const override;

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

    /// Value delegate
    GtPropertyValueDelegate* m_valDelegate;

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

};

#endif // GTPROPERTYTREEVIEW_H