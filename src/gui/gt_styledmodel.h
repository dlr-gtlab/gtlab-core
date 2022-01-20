/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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
    GtStyledModel(QObject* parent = Q_NULLPTR);

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    virtual QVariant data(const QModelIndex& index,
                          int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

};

#endif // GTSTYLEDMODEL_H
