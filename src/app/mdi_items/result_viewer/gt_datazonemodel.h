/* GTlab - Gas Turbine laboratory
 * Source File: gt_datazonemodel.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 04.08.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTDATAZONEMODEL_H
#define GTDATAZONEMODEL_H

#include <QAbstractTableModel>
#include <QPointer>

#include "gt_externalizedobjectfetcher.h"
#include "gt_abstractdatazone.h"

/**
 * @brief The GtDataZoneModel class
 */
class GtDataZoneModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    /**
     * @brief GtDataZoneModel
     * @param parent
     */
    explicit GtDataZoneModel(QObject* parent = nullptr);

    /**
     * @brief setResultData
     * @param data
     */
    void setResultData(GtAbstractDataZone* dataZone);

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

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
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    /**
     * @brief m_indeOfInterestAx1
     */
    int m_indexOfInterestAx1{0};

    /**
     * @brief m_axOfInterest
     */
    int m_axOfInterest{0};

    /**
     * @brief getResultData
     * @return
     */
    GtAbstractDataZone* getResultData();

public slots:
    /**
     * @brief clearResultData
     */
    void clearResultData();

private:

    GtExternalizedObjectFetcher<GtAbstractDataZone> m_result{};

signals:
    void axIndexChanges(int index);

    void tickIndexChanges(int index);

private slots:
    /**
     * @brief onResultChanged
     */
    void onResultChanged();

    /**
     * @brief indexChanged
     */
    void indexChanged();
};

#endif // GTDATAZONEMODEL_H
