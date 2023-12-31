/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_labelusagemodel.h
 *
 *  Created on: 21.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLABELUSAGEMODEL_H
#define GTLABELUSAGEMODEL_H

#include <QAbstractListModel>

class GtLabel;

/**
 * @brief The GtLabelUsageModel class
 */
class GtLabelUsageModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /**
     * @brief GtLabelUsageModel
     * @param parent
     */
    explicit GtLabelUsageModel(QObject* parent = nullptr);

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(
            const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    /**
     * @brief setLabelUsage
     * @param labels
     */
    void setLabelUsage(const QList<GtLabel*>& labels);

private:
    /// Label list.
//    QList<GtLabel*> m_labels;

    QStringList m_labelPaths;

};

#endif // GTLABELUSAGEMODEL_H
