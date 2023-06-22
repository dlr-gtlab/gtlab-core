/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 27.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYIDDELEGATE_H
#define GTPROPERTYIDDELEGATE_H

#include <QStyledItemDelegate>

class GtPropertyTreeView;

/**
 * @brief The GtPropertyIdDelegate class
 */
class GtPropertyIdDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent Parent object.
     */
    explicit GtPropertyIdDelegate(GtPropertyTreeView* parent = nullptr);

    /**
     * @brief paint
     * @param painter
     * @param option
     * @param index
     */
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    /**
     * @brief editorEvent
     * @param event
     * @param model
     * @param option
     * @param index
     * @return
     */
    bool editorEvent(QEvent* event,
                     QAbstractItemModel* model,
                     const QStyleOptionViewItem& option,
                     const QModelIndex& index) override;

private:
    /// Property tree view.
    GtPropertyTreeView* m_view;

    /**
     * @brief Returns style option for drawing close button of container entry.
     * @param option
     * @return
     */
    QStyleOptionViewItem containerStyleOption(
            const QStyleOptionViewItem& option) const;

signals:
    /**
     * @brief Emited if an container entry schould be deleted.
     * @param index Index of container entry
     */
    void deleteRequested(const QModelIndex& index);

};

#endif // GTPROPERTYIDDELEGATE_H
