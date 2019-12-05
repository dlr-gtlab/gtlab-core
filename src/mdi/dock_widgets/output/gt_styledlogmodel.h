/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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
 * @brief The GtStyledLogModel class
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
                  int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

protected:
    /**
     * @brief GtStyledLogModel
     * @param parent
     */
    GtStyledLogModel(QObject* parent = Q_NULLPTR);

};

#endif // GTSTYLEDLOGMODEL_H
