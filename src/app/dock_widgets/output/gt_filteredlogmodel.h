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

class GtOutputDock;
/**
 * @brief The GtFilteredLogModel class.
 * Filter proxy model for filtering out certain log levels
 */
class GtFilteredLogModel : public QSortFilterProxyModel
{
    friend class GtOutputDock;

    Q_OBJECT

    enum FilterLevel
    {
        FilterTrace   = 1 << 0,
        FilterDebug   = 1 << 1,
        FilterInfo    = 1 << 2,
        FilterWarning = 1 << 3,
        FilterError   = 1 << 4,
        FilterFatal   = 1 << 5,
    };

public slots:
    /**
     * @brief toggleDebugLevel
     * @param val
     */
    void filterTraceLevel(bool val);

    /**
     * @brief toggleDebugLevel
     * @param val
     */
    void filterDebugLevel(bool val);

    /**
     * @brief toggleInfoLevel
     * @param val
     */
    void filterInfoLevel(bool val);

    /**
     * @brief toggleWarningLevel
     * @param val
     */
    void filterWarningLevel(bool val);

    /**
     * @brief toggleErrorLevel
     * @param val
     */
    void filterErrorLevel(bool val);

    /**
     * @brief toggleFatalLevel
     * @param val
     */
    void filterFatalLevel(bool val);

    /**
     * @brief filterData
     * @param val
     */
    void filterData(const QString& val);

protected:

    /// Filter
    int m_filter{-1};

    void setFilter(FilterLevel level, bool enabled);

    /**
     * @brief GtFilteredLogModel
     * @param parent
     */
    explicit GtFilteredLogModel(QObject* parent = nullptr);

    /**
     * @brief filterAcceptsRow
     * @param source_row
     * @param source_parent
     * @return
     */
    bool filterAcceptsRow(int srcRow,
                          const QModelIndex& srcParent) const override;

};

#endif // GTFILTEREDLOGMODEL_H
