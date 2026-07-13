/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 08.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTFILTEREDLOGMODEL_H
#define GTFILTEREDLOGMODEL_H

#include <QSortFilterProxyModel>
#include <QSet>

class GtOutputDock;

namespace gt
{
    class LogFilterProxyModel;
}

/**
 * @brief The GtFilteredLogModel class.
 * 
 * Simplified filter model that only handles level filters (trace, debug, info, etc.).
 * Text search and category filters are now handled by LogFilterProxyModel.
 * This class acts as a bridge between the UI toggle buttons and the filtering logic.
 */
class GtFilteredLogModel : public QSortFilterProxyModel
{
    friend class GtOutputDock;

    Q_OBJECT

public:
    explicit GtFilteredLogModel(gt::LogFilterProxyModel* filterModel,
                                QObject* parent = nullptr);

public slots:
    /**
     * @brief filterTraceLevel
     * @param val
     */
    void filterTraceLevel(bool val);

    /**
     * @brief filterDebugLevel
     * @param val
     */
    void filterDebugLevel(bool val);

    /**
     * @brief filterInfoLevel
     * @param val
     */
    void filterInfoLevel(bool val);

    /**
     * @brief filterWarningLevel
     * @param val
     */
    void filterWarningLevel(bool val);

    /**
     * @brief filterErrorLevel
     * @param val
     */
    void filterErrorLevel(bool val);

    /**
     * @brief filterFatalLevel
     * @param val
     */
    void filterFatalLevel(bool val);

    /**
     * @brief filterData
     * @param val
     */
    void filterData(const QString& val);

protected:
    bool filterAcceptsRow(int srcRow,
                          const QModelIndex& srcParent) const override;

private:
    gt::LogFilterProxyModel* m_filterModel;
    QSet<int> m_activeLevels;
    int m_filter;

    void setFilter(int levelBit, bool enabled);
    void updateFilterModel();
};

#endif // GTFILTEREDLOGMODEL_H
