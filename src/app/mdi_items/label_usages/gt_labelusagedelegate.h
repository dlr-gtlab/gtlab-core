/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_labelusagedelegate.h
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
    explicit GtLabelUsageDelegate(QObject* parent);

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

#endif // GTLABELUSAGEDELEGATE_H
