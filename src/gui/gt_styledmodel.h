/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 07.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSTYLEDMODEL_H
#define GTSTYLEDMODEL_H

#include <QIdentityProxyModel>
#include "gt_gui_exports.h"

/**
 * @brief The GtStyledModel class
 */
class GT_GUI_EXPORT GtStyledModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    /**
     * @brief GtStyledModel
     * @param parent
     */
    explicit GtStyledModel(QObject* parent = nullptr);

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

};

#endif // GTSTYLEDMODEL_H
