/* GTlab - Gas Turbine laboratory
 * Source File: gt_processmonitoringdelegate.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 17.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSMONITORINGDELEGATE_H
#define GTPROCESSMONITORINGDELEGATE_H

#include <QStyledItemDelegate>

/**
 * @brief The GtProcessMonitoringDelegate class
 */
class GtProcessMonitoringDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    /**
     * @brief GtProcessMonitoringDelegate
     * @param parent
     */
    GtProcessMonitoringDelegate(QObject* parent = Q_NULLPTR);

    /**
     * @brief sizeHint
     * @param option
     * @param index
     * @return
     */
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const ;

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

#endif // GTPROCESSMONITORINGDELEGATE_H
