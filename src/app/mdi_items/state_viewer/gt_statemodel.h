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
    explicit GtStateModel(QObject* parent = nullptr);

    /**
     * @brief columnCount
     * @param parent
     * @return
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

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
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

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
