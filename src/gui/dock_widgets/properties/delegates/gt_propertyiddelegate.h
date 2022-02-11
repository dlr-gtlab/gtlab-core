/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 27.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYIDDELEGATE_H
#define GTPROPERTYIDDELEGATE_H

#include <QStyledItemDelegate>

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
    explicit GtPropertyIdDelegate(QObject* parent = nullptr);

    /**
     * @brief paint
     * @param painter
     * @param option
     * @param index
     */
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

};

#endif // GTPROPERTYIDDELEGATE_H
