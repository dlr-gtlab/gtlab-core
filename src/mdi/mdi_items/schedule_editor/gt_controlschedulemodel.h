/* GTlab - Gas Turbine laboratory
 * Source File: gt_controlschedulemodel.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 17.07.2018
 *      Author: Carsten Hollmann (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#ifndef GTCONTROLSCHEDULEMODEL_H
#define GTCONTROLSCHEDULEMODEL_H

#include <QAbstractItemModel>

class GtControlSchedulePlot;

class GtControlScheduleModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    /**
     * @brief GtControlScheduleModel
     * @param parent
     */
    GtControlScheduleModel(QObject* parent = Q_NULLPTR);

    /**
     * @brief GtControlScheduleModel
     * @param plot
     * @param parent
     */
    GtControlScheduleModel(GtControlSchedulePlot* plot,
                           QObject* parent = Q_NULLPTR);

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int columnCount(
            const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(
            const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(
            const QModelIndex& index, int role) const Q_DECL_OVERRIDE;

    /**
     * @brief index
     * @param row
     * @param column
     * @param parent
     * @return
     */
    QModelIndex index(int row, int column,
            const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    /**
     * @brief parent
     * @param child
     * @return
     */
    QModelIndex parent(
            const QModelIndex &child) const Q_DECL_OVERRIDE;

    /**
     * @brief setData
     * @param index
     * @param value
     * @param role
     * @return
     */
    bool setData(
            const QModelIndex &index, const QVariant &value,
            int role = Qt::EditRole) Q_DECL_OVERRIDE;

    /**
     * @brief setPlot
     * @param plot
     */
    void setPlot(GtControlSchedulePlot* plot);

    /**
     * @brief flags
     * @param index
     * @return
     */
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    /// plot holding schedule data points
    GtControlSchedulePlot* m_plot;

    bool m_editAllowed;

private slots:
    /**
     * @brief onEditModeChanged
     * @param active
     */
    void onEditModeChanged(bool active);

    /**
     * @brief onDataChanged
     */
    void onDataChanged();

};

#endif // GTCONTROLSCHEDULEMODEL_H
