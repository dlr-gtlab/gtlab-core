/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 08.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSTYLEDLOGMODEL_H
#define GTSTYLEDLOGMODEL_H

#include <QIdentityProxyModel>

class GtOutputDock;

/**
 * @brief The GtStyledLogModel class.
 * Styles the basic log model to return icons as levels instead, provide
 * tooltips as well as colored text and background.
 */
class GtStyledLogModel : public QIdentityProxyModel
{
    friend class GtOutputDock;

    Q_OBJECT

public:
    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

protected:
    /**
     * @brief GtStyledLogModel
     * @param parent
     */
    explicit GtStyledLogModel(QObject* parent = nullptr);

};

#endif // GTSTYLEDLOGMODEL_H
