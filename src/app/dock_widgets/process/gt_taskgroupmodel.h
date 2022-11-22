/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
 *
 *  Created on: 22.11.2022
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTASKGROUPMODEL_H
#define GTTASKGROUPMODEL_H

#include <QAbstractListModel>

class GtTaskGroupModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent
     */
    GtTaskGroupModel(QObject* parent = nullptr);

    void init(const QStringList& userGroups, const QStringList& customGroups);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    QStringList m_userGroups;

    QStringList m_customGroups;

    QString rowText(int row) const;

    bool rowIsSelectable(int row) const;

};

#endif // GTTASKGROUPMODEL_H
