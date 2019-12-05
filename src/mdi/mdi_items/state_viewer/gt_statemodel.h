/* GTlab - Gas Turbine laboratory
 * Source File: gt_statemodel.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSTATEMODEL_H
#define GTSTATEMODEL_H

#include "gt_objectmodel.h"

/**
 * @brief The GtStateModel class
 */
class GtStateModel : public GtObjectModel
{
    Q_OBJECT

public:
    /**
     * @brief GtStateModel
     */
    GtStateModel(QObject* parent = Q_NULLPTR);

    /**
     * @brief columnCount
     * @param parent
     * @return
     */
    virtual int columnCount(
            const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    /**
     * @brief Reloads state model.
     */
    void reload();

private:
    /**
     * @brief Converts variant content to string.
     * @param Variant.
     * @return Converted string.
     */
    QString variantToString(const QVariant& variant) const;

};

#endif // GTSTATEMODEL_H
