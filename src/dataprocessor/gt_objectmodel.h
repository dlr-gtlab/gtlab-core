/* GTlab - Gas Turbine laboratory
 * Source File: gt_objectmodel.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOBJECTMODEL_H
#define GTOBJECTMODEL_H

#include "gt_datamodel_exports.h"

#include <QAbstractItemModel>
#include <QPointer>

class GtObject;

/**
 * @brief The GtObjectModel class
 */
class GT_DATAMODEL_EXPORT GtObjectModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Parent object.
     */
    explicit GtObjectModel(QObject* parent = nullptr);

    /**
     * @brief Constructor.
     * @param Root object.
     * @param Parent object.
     */
    GtObjectModel(GtObject* root, QObject* parent = nullptr);

    /**
     * @brief Sets new root object and resets model.
     * @param New root object.
     */
    void setRootObject(GtObject* obj);

    /**
     * @brief columnCount
     * @param parent
     * @return
     */
    virtual int columnCount(
            const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    virtual int rowCount(
            const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief index
     * @param row
     * @param col
     * @param parent
     * @return
     */
    virtual QModelIndex index(int row, int col,
                const QModelIndex& parent =QModelIndex()) const override;

    /**
     * @brief parent
     * @param index
     * @return
     */
    virtual QModelIndex parent(const QModelIndex& index) const override;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    virtual QVariant data(const QModelIndex& index,
                          int role = Qt::DisplayRole ) const override;

    /**
     * @brief objectFromIndex
     * @param index
     * @return
     */
    virtual GtObject* objectFromIndex(const QModelIndex& index) const;

    /**
     * @brief indexFromObject
     * @param obj
     * @return
     */
    virtual QModelIndex indexFromObject(GtObject* obj) const;

private:
    /// Root object.
    QPointer<GtObject> m_root;

};

#endif // GTOBJECTMODEL_H
