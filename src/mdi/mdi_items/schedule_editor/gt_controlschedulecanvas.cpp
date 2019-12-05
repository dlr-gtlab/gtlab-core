/* GTlab - Gas Turbine laboratory
 * Source File: gt_controlschedulecanvas.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 26.02.2018
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#include "gt_controlschedulecanvas.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QLabel>

#include <qmath.h>

#include "qwt_symbol.h"

#include "gt_logging.h"
#include "gt_application.h"
#include "gt_controlschedule.h"
#include "gt_controlscheduleplot.h"

GtControlScheduleCanvas::GtControlScheduleCanvas(GtControlSchedulePlot* plot) :
    QwtPlotCanvas(), m_plot(plot), m_selectedMarker(Q_NULLPTR),
    m_readyToMove(false), m_addPointActive(false), m_dataProbeActive(false),
    m_tmpMarker(Q_NULLPTR)
{
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setLineWidth(2);
    setCursor(Qt::ArrowCursor);

    if (m_plot != Q_NULLPTR)
    {
        m_plot->setCanvas(this);
    }
}

void
GtControlScheduleCanvas::mouseMoveEvent(QMouseEvent* event)
{
    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    const QwtScaleMap xMap = m_plot->canvasMap(QwtPlot::xBottom);
    const QwtScaleMap yMap = m_plot->canvasMap(QwtPlot::yLeft);

    double xCoord = xMap.invTransform(event->pos().x());
    double yCoord = yMap.invTransform(event->pos().y());

    if (dataProbeActive())
    {
        if (m_tmpMarker != Q_NULLPTR)
        {
            GtControlSchedule* sched = m_plot->schedule();

            if (sched == Q_NULLPTR)
            {
                m_tmpMarker->detach();
                delete m_tmpMarker;
                m_tmpMarker = Q_NULLPTR;
            }
            else
            {
                yCoord = sched->getValue1D(sched->valueNames().first(),
                                           xCoord);

                m_tmpMarker->setValue(xCoord, yCoord);
                m_plot->replot();
            }
        }
    }
    else if (addPointActive())
    {
        if (m_tmpMarker != Q_NULLPTR)
        {
            m_tmpMarker->setValue(xCoord, yCoord);
            m_plot->replot();
        }
    }
    else if (m_selectedMarker != Q_NULLPTR)
    {
        if (!m_readyToMove)
        {
            return;
        }

        m_plot->setMarkerPosition(m_selectedMarker, xCoord, yCoord);
    }

    QwtPlotCanvas::mouseMoveEvent(event);
}

void
GtControlScheduleCanvas::mousePressEvent(QMouseEvent* event)
{
    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    if (m_plot->editModeActive())
    {
        if (!addPointActive())
        {
            double dist = 10e10;
            GtControlScheduleMarker* marker = closestMarker(event->pos(),
                                                            dist);

            if (dist <= 3)
            {
                if (marker != Q_NULLPTR)
                {
                    m_plot->selectMarker(marker);
                    m_plot->switchToClickPointTrackerMachine();
                    m_selectedMarker = marker;
                    m_readyToMove = true;
                }
            }
            else
            {
                if (m_plot->hasSelectedMarkers())
                {
                    m_plot->deselectMarkers();
                }

                m_selectedMarker = Q_NULLPTR;
            }
        }
        else if (addPointActive() && event->button() == Qt::LeftButton)
        {
            if (m_tmpMarker == Q_NULLPTR)
            {
                m_tmpMarker = new QwtPlotMarker;
                m_tmpMarker->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
                                                     QBrush(Qt::darkRed),
                                                     QPen(Qt::darkRed),
                                                     QSize(7, 7)));
                m_tmpMarker->attach(m_plot);
            }

            const QwtScaleMap xMap = m_plot->canvasMap(QwtPlot::xBottom);
            const QwtScaleMap yMap = m_plot->canvasMap(QwtPlot::yLeft);

            double xCoord = xMap.invTransform(event->pos().x());
            double yCoord = yMap.invTransform(event->pos().y());

            m_tmpMarker->setValue(xCoord, yCoord);
            m_tmpMarker->attach(m_plot);
            m_plot->replot();
        }
    }
    else if (dataProbeActive() && event->button() == Qt::LeftButton)
    {
        if (m_tmpMarker == Q_NULLPTR)
        {
            m_tmpMarker = new QwtPlotMarker;
            m_tmpMarker->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
                                                 QBrush(Qt::darkRed),
                                                 QPen(Qt::darkRed),
                                                 QSize(7, 7)));
            m_tmpMarker->attach(m_plot);
        }

        const QwtScaleMap xMap = m_plot->canvasMap(QwtPlot::xBottom);
        const QwtScaleMap yMap = m_plot->canvasMap(QwtPlot::yLeft);

        double xCoord = xMap.invTransform(event->pos().x());
        double yCoord = yMap.invTransform(event->pos().y());

        GtControlSchedule* sched = m_plot->schedule();

        if (sched == Q_NULLPTR)
        {
            delete m_tmpMarker;
            m_tmpMarker = Q_NULLPTR;
        }
        else
        {
            yCoord = sched->getValue1D(sched->valueNames().first(),
                                       xCoord);

            m_tmpMarker->setValue(xCoord, yCoord);
            m_tmpMarker->attach(m_plot);
            m_plot->replot();
        }
    }

    if (event->button() == Qt::RightButton)
    {
        if (!dataProbeActive() && !addPointActive())
        {
            m_readyToMove = false;
            m_plot->switchToClickPointMachine();

            rightClickContextMenu();
        }
    }

    QwtPlotCanvas::mousePressEvent(event);
}

void
GtControlScheduleCanvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (addPointActive())
    {
        if (event->button() == Qt::RightButton)
        {
            return;
        }

        const QwtScaleMap xMap = m_plot->canvasMap(QwtPlot::xBottom);
        const QwtScaleMap yMap = m_plot->canvasMap(QwtPlot::yLeft);

        double xCoord = xMap.invTransform(event->pos().x());
        double yCoord = yMap.invTransform(event->pos().y());

        m_plot->addPoint(xCoord, yCoord);
        setCursor(Qt::ArrowCursor);
        emit changeAddPointMode();
    }
    else if (dataProbeActive())
    {
        if (event->button() == Qt::RightButton)
        {
            return;
        }

        const QwtScaleMap xMap = m_plot->canvasMap(QwtPlot::xBottom);
        const QwtScaleMap yMap = m_plot->canvasMap(QwtPlot::yLeft);

        double xCoord = xMap.invTransform(event->pos().x());
        double yCoord = yMap.invTransform(event->pos().y());

        setCursor(Qt::ArrowCursor);

        GtControlSchedule* sched = m_plot->schedule();

        if (sched != Q_NULLPTR)
        {
            yCoord = sched->getValue1D(sched->valueNames().first(),
                                       xCoord);

            emit dataProbeTaken(QPointF(xCoord, yCoord));
        }

        onDataProbeRequest();
    }

    if (m_tmpMarker != Q_NULLPTR)
    {
        m_tmpMarker->detach();
        delete m_tmpMarker;
        m_tmpMarker = Q_NULLPTR;
    }

    if (m_readyToMove)
    {
        emit pointMoved();
        m_readyToMove = false;
    }

    // Keep selection in table view after moving a point
    if (m_selectedMarker != Q_NULLPTR)
    {
        m_plot->deselectMarkers();
        m_plot->selectMarker(m_selectedMarker);
    }

    m_plot->replot();
    m_plot->switchToClickPointMachine();

    QwtPlotCanvas::mousePressEvent(event);
}

GtControlScheduleMarker*
GtControlScheduleCanvas::closestMarker(const QPoint& pos, double& dist)
{
    if (m_plot == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    GtControlScheduleMarker* retVal = Q_NULLPTR;

    const QwtPlotItemList& itmList = m_plot->itemList();

    for (QwtPlotItemIterator it = itmList.begin(); it != itmList.end(); ++it)
    {
        if ((*it)->rtti() == QwtPlotItem::Rtti_PlotUserItem)
        {
            GtControlScheduleMarker* marker =
                static_cast<GtControlScheduleMarker*>(*it);

            if (marker != Q_NULLPTR)
            {
                QPointF val = marker->value();

                const QwtScaleMap xMap = m_plot->canvasMap(QwtPlot::xBottom);
                const QwtScaleMap yMap = m_plot->canvasMap(QwtPlot::yLeft);

                const double cx = xMap.transform(val.x()) - pos.x();
                const double cy = yMap.transform(val.y()) - pos.y();

                const double f = qSqrt(qAbs(cx)) + qSqrt(qAbs(cy));

                if (f < dist)
                {
                    dist = f;
                    retVal = marker;
                }
            }
        }
    }

    return retVal;
}

void
GtControlScheduleCanvas::rightClickContextMenu()
{
    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    QMenu menu(m_plot);

    QAction* actStartEdit = menu.addAction(tr("Edit schedule"));
    actStartEdit->setIcon(gtApp->icon("monitoringIcon.png"));

    QAction* actEndEdit = menu.addAction(tr("Exit editing mode"));
    actEndEdit->setIcon(gtApp->icon("monitoringIcon.png"));

    menu.addSeparator();

    QAction* actAddPoint = menu.addAction(tr("Add point"));
    actAddPoint->setIcon(gtApp->icon("addIcon.png"));

    QAction* actModAxis = menu.addAction(tr("Configure schedule"));
    actModAxis->setIcon(gtApp->icon("configIcon.png"));

    QAction* actModCoords = menu.addAction(tr("Modify point coordinates"));
    actModCoords->setIcon(gtApp->icon("operatingPoint.png"));

    QAction* actDeletePoint = menu.addAction(tr("Delete Point"));
    actDeletePoint->setIcon(gtApp->icon("closeIcon.png"));

    menu.addSeparator();

    QAction* actRevert = menu.addAction(tr("Revert and discard changes"));
    actRevert->setIcon(gtApp->icon("undoIcon.png"));

    QAction* actSave = menu.addAction(tr("Accept changes"));
    actSave->setIcon(gtApp->icon("checkIcon.png"));

    QAction* actTakeProbe = menu.addAction(tr("Data probe"));
    actTakeProbe->setIcon(gtApp->icon("cursorIcon.png"));

    if (m_plot->editModeActive())
    {
        actStartEdit->setVisible(false);
        actTakeProbe->setVisible(false);

        if (m_selectedMarker != Q_NULLPTR)
        {
            actEndEdit->setVisible(false);
            actAddPoint->setVisible(false);
            actModAxis->setVisible(false);
            actSave->setVisible(false);
            actRevert->setVisible(false);
        }
    }
    else
    {
        actEndEdit->setVisible(false);
        actAddPoint->setVisible(false);
        actModAxis->setVisible(false);
        actSave->setVisible(false);
        actRevert->setVisible(false);
    }

    if (m_selectedMarker == Q_NULLPTR || !m_plot->editModeActive())
    {
        actModCoords->setVisible(false);
        actDeletePoint->setVisible(false);
    }

    QAction* a = menu.exec(QCursor::pos());

    if (a == actModCoords)
    {
        onModifyRequest();
    }
    else if (a == actDeletePoint)
    {
        onDeletePointRequest();
    }
    else if (a == actStartEdit || a == actEndEdit)
    {
        onChangeEditMode();
    }
    else if (a == actAddPoint)
    {
        onAddPointRequest();
    }
    else if (a == actModAxis)
    {
        onModAxisRequest();
    }
    else if (a == actRevert)
    {
        onRevertRequest();
    }
    else if (a == actSave)
    {
        onSaveRequest();
    }
    else if (a == actTakeProbe)
    {
        onDataProbeRequest();
    }
}

void
GtControlScheduleCanvas::onModifyRequest()
{
    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    if (m_selectedMarker == Q_NULLPTR)
    {
        return;
    }

    QDialog dialog(this);

    dialog.setWindowTitle("Point modification...");

    QFormLayout formLay(&dialog);

    dialog.setWindowIcon(gtApp->icon("operatingPoint.png"));

    formLay.addRow(new QLabel("Please enter the desired point corrdinates:"));

    QDoubleSpinBox* xCoord = new QDoubleSpinBox(&dialog);
    xCoord->setSingleStep(0.1);
    xCoord->setMaximum(1e12);
    xCoord->setMinimum(-1e12);
    xCoord->setDecimals(5);
    xCoord->setValue(m_selectedMarker->value().x());

    QDoubleSpinBox* yCoord = new QDoubleSpinBox(&dialog);
    yCoord->setSingleStep(0.1);
    yCoord->setMaximum(1e12);
    yCoord->setMinimum(-1e12);
    yCoord->setDecimals(5);
    yCoord->setValue(m_selectedMarker->value().y());

    formLay.addRow(m_plot->xTitle() + ":", xCoord);
    formLay.addRow(m_plot->yTitle() + ":", yCoord);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    formLay.addRow(&buttonBox);

    connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    dialog.layout()->setSizeConstraint(QLayout::SetFixedSize);

    if (dialog.exec() == QDialog::Accepted)
    {
        m_plot->setMarkerPosition(m_selectedMarker,
                                  xCoord->value(), yCoord->value());
    }

}

void
GtControlScheduleCanvas::onDeletePointRequest()
{
    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    m_plot->deleteMarker(m_selectedMarker);
    m_selectedMarker = Q_NULLPTR;
}

void
GtControlScheduleCanvas::onChangeEditMode()
{
    emit changeEditMode();
}

void
GtControlScheduleCanvas::onAddPointRequest()
{
    emit changeAddPointMode();
}

void
GtControlScheduleCanvas::onModAxisRequest()
{
    emit changeAxisData();
}

void
GtControlScheduleCanvas::onDataProbeRequest()
{
    emit changeDataProbeMode();
}

void
GtControlScheduleCanvas::activateAddPointState(bool activate)
{
    if (activate)
    {
        if (dataProbeActive())
        {
            onDataProbeRequest();
        }

        m_addPointActive = true;
        setCursor(Qt::CrossCursor);
        m_plot->deselectMarkers();
        m_selectedMarker = Q_NULLPTR;
        m_plot->switchToPickerTrackerMachine();
    }
    else
    {
        m_addPointActive = false;
        setCursor(Qt::ArrowCursor);
        m_plot->deselectMarkers();
        m_selectedMarker = Q_NULLPTR;
        m_plot->switchToClickPointMachine();
    }
}

void
GtControlScheduleCanvas::activateDataProbeState(bool activate)
{
    if (activate)
    {
        if (addPointActive())
        {
            onAddPointRequest();
        }

        m_dataProbeActive = true;
        setCursor(Qt::CrossCursor);
        m_plot->deselectMarkers();
        m_selectedMarker = Q_NULLPTR;
        m_plot->switchToPickerTrackerMachine();
    }
    else
    {
        m_dataProbeActive = false;
        setCursor(Qt::ArrowCursor);
        m_plot->deselectMarkers();
        m_selectedMarker = Q_NULLPTR;
        m_plot->switchToClickPointMachine();
    }
}

bool
GtControlScheduleCanvas::addPointActive()
{
    return m_addPointActive;
}

bool
GtControlScheduleCanvas::dataProbeActive()
{
    return m_dataProbeActive;
}

void
GtControlScheduleCanvas::onRevertRequest()
{
    emit revertAndDiscard();
}

void
GtControlScheduleCanvas::onSaveRequest()
{
    emit save();
}
