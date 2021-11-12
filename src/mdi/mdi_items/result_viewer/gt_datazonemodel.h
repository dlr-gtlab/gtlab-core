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

class GtAbstractDataZone;
class GtDataZone;

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
    GtDataZoneModel(QObject* parent = Q_NULLPTR);
    ~GtDataZoneModel();

    /**
     * @brief setResultData
     * @param data
     */
    void setResultData(GtAbstractDataZone* data);

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(
            const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    /**
     * @brief columnCount
     * @param parent
     * @return
     */
    int columnCount(
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
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    /**
     * @brief m_indeOfInterestAx1
     */
    int m_indeOfInterestAx1;

    /**
     * @brief m_axOfInterest
     */
    int m_axOfInterest;

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
    QPointer<GtAbstractDataZone> m_result;

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

/**
 * @brief value2D
 * @param row
 * @param column
 * @param axOfInterest
 * @param indexOfInterest
 * @param dz
 * @return
 */
double value2D(int row, int column, int axOfInterest, int indexOfInterest,
               GtDataZone *dz);

#endif // GTDATAZONEMODEL_H
