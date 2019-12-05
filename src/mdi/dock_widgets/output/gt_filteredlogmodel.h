/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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
 * @brief The GtFilteredLogModel class
 */
class GtFilteredLogModel : public QSortFilterProxyModel
{
    friend class GtOutputDock;

    Q_OBJECT

public slots:
    /**
     * @brief toggleDebugLevel
     * @param val
     */
    void toggleDebugLevel(bool val);

    /**
     * @brief toggleInfoLevel
     * @param val
     */
    void toggleInfoLevel(bool val);

    /**
     * @brief toggleWarningLevel
     * @param val
     */
    void toggleWarningLevel(bool val);

    /**
     * @brief toggleErrorLevel
     * @param val
     */
    void toggleErrorLevel(bool val);

    /**
     * @brief toggleFatalLevel
     * @param val
     */
    void toggleFatalLevel(bool val);

    /**
     * @brief filterData
     * @param val
     */
    void filterData(const QString& val);

protected:
    /// Show debug level message indicator
    bool m_debugLevel;

    /// Show info level message indicator
    bool m_infoLevel;

    /// Show warning level message indicator
    bool m_warningLevel;

    /// Show error level message indicator
    bool m_errorLevel;

    /// Show fatal level message indicator
    bool m_fatalLevel;

    /**
     * @brief GtFilteredLogModel
     * @param parent
     */
    GtFilteredLogModel(QObject* parent = 0);

    /**
     * @brief filterAcceptsRow
     * @param source_row
     * @param source_parent
     * @return
     */
    virtual bool filterAcceptsRow(int source_row,
                                  const QModelIndex& source_parent) const;

};

#endif // GTFILTEREDLOGMODEL_H
