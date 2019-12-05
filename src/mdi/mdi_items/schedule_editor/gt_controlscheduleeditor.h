/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 02.08.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */
#ifndef GTCONTROLSCHEDULEEDITOR_H
#define GTCONTROLSCHEDULEEDITOR_H

#include <QPointF>

#include "gt_mdi_exports.h"
#include "gt_mdiitem.h"
#include "gt_controlschedule.h"

class GtControlSchedulePlot;
class GtControlScheduleMarker;
class GtControlScheduleModel;

class QLabel;
class QPushButton;
class QComboBox;
class QKeyEvent;
class QTableWidget;
class QTabWidget;
class QTableView;

/**
 * @brief The GtScheduleEditor class
 */
class GT_MDI_EXPORT GtControlScheduleEditor : public GtMdiItem
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    Q_INVOKABLE GtControlScheduleEditor();

    /**
     * @brief event filter needed for key events in the whole widget area
     * @param obj member widget
     * @param event event
     * @return
     */
    virtual bool eventFilter(QObject* obj, QEvent* event);

    /**
     * @brief overloaded function from GtMdiItem
     * @param obj is here a GtControlSchedule
     */
    void setData(GtObject* obj) Q_DECL_OVERRIDE;

    /**
     * @brief key press event implementation
     * @param event
     */
    void onKeyPressEvent(const QKeyEvent* event);

private:
    /// pointer to plot widget
    GtControlSchedulePlot* m_plot;

    /// line edit
    QLabel* m_title;

    /// add point button
    QPushButton* m_addPointButton;

    /// data probe button
    QPushButton* m_probeButton;

    /// edit mode button
    QPushButton* m_editButton;

    /// save button
    QPushButton* m_saveButton;

    /// revert and discard button
    QPushButton* m_revertButton;

    /// revert and discard button
    QPushButton* m_modAxisButton;

    /// table clear button
    QPushButton* m_tableClearButton;

    /// table clear button
    QPushButton* m_probeAtButton;

    /// combo box for interpolation methods
    QComboBox* m_interpBox;

    /// combo box for extrapolation methods (low extrapolation)
    QComboBox* m_loExtrapBox;

    /// combo box for extrapolation methods (high extrapolation)
    QComboBox* m_hiExtrapBox;

    /// schedule which proviedes the data on which the plot is based
    GtControlSchedule* m_schedule;

    /// data probe table widget
    QTableWidget* m_probeTable;

    /// table view
    QTableView* m_tableView;

    /// model
    GtControlScheduleModel* m_model;

    /// Tab Widget for data probe table and points table;
    QTabWidget* m_tabWidget;

    /// add button in data points table view
    QPushButton* m_tableAddButton;

    /// delete button in data points table view
    QPushButton* m_tableDeleteButton;

    /// original interpolation method of table
    GtTableAxis::InterpMethod m_oldInterp;

    /// original extrapolation method of table (low extrapolation)
    GtTableAxis::ExtrapMethod m_oldLoExtrap;

    /// original interpolation method of table (high extrapolation)
    GtTableAxis::ExtrapMethod m_oldHiExtrap;

    /**
     * @brief initiate combo box from extrapolation method
     * @param cBox
     * @param em
     */
    void initExtrpComboBox(QComboBox* cBox,
                           const GtTableAxis::ExtrapMethod em);

    /**
     * @brief initialize combobox from interpolation method
     * @param cBox
     * @param im
     */
    void initInterpComboBox(QComboBox *cBox,
                            const GtTableAxis::InterpMethod im);

    /**
     * @brief get extrapolation method from combobox
     * @param cBox
     * @return
     */
    GtTableAxis::ExtrapMethod findExtrpMethod(const QComboBox* cBox);

    /**
     * @brief get interpolation method from combo box
     * @param cBox
     * @return
     */
    GtTableAxis::InterpMethod findInterpMethod(const QComboBox* cBox);

    /**
     * @brief reverts a combo box to a saved state
     * @param cBox
     */
    void revertComboBox(QComboBox* cBox);

    /**
     * @brief checks if plot data is equal to data saved in the data model
     * @return
     */
    bool isEqualToDatamodel();

    /**
     * @brief cconstructs a table object from the plot data
     * @return
     */
    GtControlSchedule* constructTableFromPlot();

private slots:
    /**
     * @brief actions performeed when edit button is pressed
     */
    void onEditButtonPress();

    /**
     * @brief actions performeed when data probe button is pressed
     */
    void onDataProbePress();

    /**
     * @brief actions performeed when add point button is pressed
     */
    void onAddButtonPress();

    /**
     * @brief actions performeed when add point button is pressed
     */
    void onModAxisButtonPress();

    /**
     * @brief actions performeed when save button is pressed
     */
    void onSaveButtonpress();

    /**
     * @brief actions performeed when revert button is pressed
     */
    void onRevertButtonPress();

    /**
     * @brief actions performed when changeEditMode signal is reveived from
     * canvas
     */
    void onChangeEditMode();

    /**
     * @brief actions performed when changeAddPointMode signal is reveived from
     * canvas
     */
    void onChangeAddPointMode();

    /**
     * @brief actions performed when changeDataProbeMode signal is reveived from
     * canvas
     */
    void onChangeDataProbeMode();

    /**
     * @brief check if chosen extrapolation method is valid, if not it will
     * be set to the original extrapolation method
     */
    void onExtrapolationChanged();

    /**
     * @brief check if chosen interpolation method is valid, if not it will
     * be set to the original interpolation method
     */
    void onInterpolationChanged();
    /**
     * @brief actions performed when revert signal is reveived from canvas
     */
    void onRevertRequest();

    /**
     * @brief actions performed when save signal is reveived from canvas
     */
    void onSaveRequest();

    /**
     * @brief actions performed when a data probe was taken
     */
    void onDataProbeTaken(const QPointF point);

    /**
     * @brief clearDataProbes
     */
    void clearDataProbes();

    /**
     * @brief onTableAddButtonPress
     */
    void onTableAddButtonPressed();

    /**
     * @brief onTableDeleteButtonPressed
     */
    void onTableDeleteButtonPressed();

    /**
     * @brief onTableViewSelectionChanged
     */
    void onTableViewSelectionChanged();

    /**
     * @brief onMarkerSelected
     * @param marker
     */
    void onMarkerSelected(GtControlScheduleMarker* marker);

    /**
     * @brief onMarkersDeselected
     */
    void onMarkersDeselected();

    /**
     * @brief onChangeAxisInformation
     */
    void onChangeAxisInformation();

    /**
     * @brief onLineEditCheckRequired
     */
    void onLineEditCheckRequired();

    /**
     * @brief onDataProbeAtButtonpress
     */
    void onDataProbeAtButtonPress();
};

#endif // GTCONTROLSCHEDULEEDITOR_H
