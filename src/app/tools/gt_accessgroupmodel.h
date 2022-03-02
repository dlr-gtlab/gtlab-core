/* GTlab - Gas Turbine laboratory
 * Source File: gt_accessgroupmodel.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 26.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_ACCESSGROUPMODEL_H
#define GT_ACCESSGROUPMODEL_H

#include <QAbstractTableModel>
#include <QPointer>

class GtAccessGroup;
class GtAccessData;

/**
 * @brief The GtAccessGroupModel class
 */
class GtAccessGroupModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Parent object.
     */
    explicit GtAccessGroupModel(GtAccessGroup* group,
                                QObject* parent = nullptr);

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(const QModelIndex& parent) const override;

    /**
     * @brief columnCount
     * @param parent
     * @return
     */
    int columnCount(const QModelIndex& parent) const override;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex &index, int role) const override;

    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const override;

    /**
     * @brief addAccessData
     * @param accessData
     * @return
     */
    bool addAccessData(const GtAccessData& accessData);

    /**
     * @brief removeAccessData
     * @param index
     * @return
     */
    bool removeAccessData(const QModelIndex& index);

    /**
     * @brief setAccessDat
     * @param accessData
     * @param index
     * @return
     */
    bool setAccessData(const GtAccessData& accessData,
                       const QModelIndex& index);

private:
    /// Pointer to access group object.
    QPointer<GtAccessGroup> m_group;

};

#endif // GT_ACCESSGROUPMODEL_H
