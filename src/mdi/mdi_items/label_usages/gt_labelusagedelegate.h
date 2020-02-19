/* GTlab - Gas Turbine laboratory
 * Source File: gt_labelusagedelegate.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 22.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLABELUSAGEDELEGATE_H
#define GTLABELUSAGEDELEGATE_H

#include <QStyledItemDelegate>

/**
 * @brief The GtLabelUsageDelegate class
 */
class GtLabelUsageDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    GtLabelUsageDelegate(QObject* parent);

    /**
     * @brief paint
     * @param painter
     * @param option
     * @param index
     */
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const Q_DECL_OVERRIDE;

};

#endif // GTLABELUSAGEDELEGATE_H