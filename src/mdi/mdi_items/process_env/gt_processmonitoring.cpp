/* GTlab - Gas Turbine laboratory
 * Source File: gt_processmonitoring.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 14.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QVBoxLayout>
#include <QDebug>

#include "gt_task.h"
#include "gt_application.h"
#include "gt_processmonitoringmodel.h"
#include "gt_processmonitoringview.h"

#include "qwt_plot.h"
#include "qwt_plot_canvas.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_grid.h"
#include "qwt_plot_layout.h"
#include "qwt_plot_legenditem.h"
#include "qwt_scale_widget.h"
#include "qwt_symbol.h"

#include "gt_processmonitoring.h"

GtProcessMonitoring::GtProcessMonitoring() : m_task(Q_NULLPTR)
{
    setObjectName(tr("Process Monitoring Table"));

    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    widget()->setBackgroundRole(QPalette::Base);

    m_view = new GtProcessMonitoringView(widget());
    m_view->setFrameStyle(QTreeView::NoFrame);
    m_view->setAlternatingRowColors(true);

    m_model = new GtProcessMonitoringModel(this);
    m_view->setModel(m_model);

    widget()->setLayout(lay);

    lay->addWidget(m_view);

    QHBoxLayout* plotLayout = new QHBoxLayout;

    lay->addLayout(plotLayout);

    m_analyzer = new QwtPlot(widget());

    plotLayout->addWidget(m_analyzer);

    m_analyzer->setVisible(false);
    m_analyzer->setAutoFillBackground(true);
    m_analyzer->setPalette(Qt::white);
    m_analyzer->setCanvasBackground(Qt::white);
    m_analyzer->setAxisTitle(QwtPlot::xBottom, tr("Iteration"));
    m_analyzer->plotLayout()->setAlignCanvasToScales(true);

    QwtPlotCanvas* c = dynamic_cast<QwtPlotCanvas*>(m_analyzer->canvas());
    if (c != Q_NULLPTR)
    {
        c->setFocusIndicator(QwtPlotCanvas::CanvasFocusIndicator);
        c->setFocusPolicy(Qt::StrongFocus);
        c->setPalette(Qt::white);
        c->setAutoFillBackground(true);
        c->setFrameStyle(QFrame::Box | QFrame::Plain);
        c->setLineWidth(2);
        c->setCursor(Qt::ArrowCursor);
    }

    // connect signals
    connect(m_model, SIGNAL(selectionChanged()), SLOT(onSelectionChanged()));

    for (int i = 0; i < m_analyzer->axisCnt; i++)
    {
        QwtScaleWidget* scaleWid = m_analyzer->axisWidget(i);

        if (scaleWid != Q_NULLPTR)
        {
            scaleWid->setMargin(0);
        }

        QwtScaleDraw* scaleDraw = m_analyzer->axisScaleDraw(i);

        if (scaleDraw != Q_NULLPTR)
        {
            scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
        }
    }
}

GtProcessMonitoring::~GtProcessMonitoring()
{
    /// clear model
    m_model->clearModel();

    if (widget() != Q_NULLPTR)
    {
        if (m_analyzer != Q_NULLPTR)
        {
            delete m_analyzer;
        }
    }

    clearPlotItems();
}

void
GtProcessMonitoring::clearPlotItems()
{
    foreach (QwtPlotItem* item, m_plotItems)
    {
        if (item != Q_NULLPTR)
        {
            item->detach();
            delete item;
        }
    }

    m_plotItems.clear();
}

void
GtProcessMonitoring::setData(GtObject* obj)
{
    m_model->clearModel();

    // check object
    if (obj == Q_NULLPTR)
    {
        return;
    }

    // cast object to task
    GtTask* task = qobject_cast<GtTask*>(obj);

    // set internal task pointer
    m_task = task;

    // check casted task
    if (task == Q_NULLPTR)
    {
        // casting failed
        return;
    }

    // set new mdi window title
    setObjectName(tr("Process Monitoring Table") + QStringLiteral(" - ") +
                  task->objectName());

    m_model->setMonitoringData(task);
    m_view->expandAll();
    m_view->resizeColumns();

    // connect signals
    connect(task, SIGNAL(destroyed(QObject*)), SLOT(deleteLater()));
    connect(task, SIGNAL(monitoringDataAvailable()),
            SLOT(onMonitoringDataAvailable()));
}

QIcon
GtProcessMonitoring::icon() const
{
    return gtApp->icon(QStringLiteral("monitoringIcon.png"));
}

void
GtProcessMonitoring::createPlotCurve(const QString& compUuid,
                                     const QString& propId,
                                     GtProcessMonitoringItem* item,
                                     GtProcessMonitoringItem* parentItem)
{
    // check strings
    if (propId.isEmpty() || compUuid.isEmpty())
    {
        gtError() << tr("Monitoring item invalid!");
        return;
    }

    // check items
    if (item == Q_NULLPTR || parentItem == Q_NULLPTR)
    {
        gtError() << tr("Monitoring item invalid!");
        return;
    }

    // check task
    if (m_task == Q_NULLPTR)
    {
        gtError() << tr("Task not found!");
        return;
    }

    // get specific data
    GtMonitoringDataTable dataTable = m_task->monitoringDataTable();

    QVector<QVariant> data = dataTable.getData(compUuid, propId);

    if (data.isEmpty())
    {
        return;
    }

    QVector<double> samplesX;
    QVector<double> samplesY;

    for (int i = 0; i < data.size(); i++)
    {
        QVariant var = data[i];

        if (var.isValid())
        {
            samplesX << (i + 1);
            samplesY << var.toDouble();
        }
    }

    QColor color = item->color();
    QString id = parentItem->data(0).toString() + QStringLiteral(" - ") +
                 item->data(0).toString();

    QwtPlotCurve* curve = new QwtPlotCurve();
    curve->setPen(color, 2);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->setSamples(samplesX, samplesY);
    curve->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
                                   Qt::white,
                                   QPen(color),
                                   QSize(5, 5)));
    curve->setTitle(id);
    curve->attach(m_analyzer);

    m_plotItems.append(curve);
}

void
GtProcessMonitoring::onSelectionChanged()
{
    // check model
    if (m_model == Q_NULLPTR)
    {
        return;
    }

    // get list of selected items
    QList<GtProcessMonitoringItem*> items = m_model->selectedItems();

    // check whether item list is empty or not
    if (!items.isEmpty())
    {
        m_analyzer->setVisible(true);
        onMonitoringDataAvailable();
    }
    else
    {
        m_analyzer->setVisible(false);
    }
}

void
GtProcessMonitoring::onMonitoringDataAvailable()
{
    qDebug() << "#### GtProcessMonitoring::onMonitoringDataAvailable()!";

    // check whether analyzer widget is shown
    if (!m_analyzer->isVisible())
    {
        // analyzer widget is hidden
        return;
    }

    // check model
    if (m_model == Q_NULLPTR)
    {
        return;
    }

    m_analyzer->detachItems(QwtPlotItem::Rtti_PlotItem, true);

    /// Test 08.01.2018
    //m_plotItems.clear();
    clearPlotItems();


    // legend
    m_legend = new QwtPlotLegendItem;
//    legend->setRenderHint(QwtPlotItem::RenderAntialiased);
    m_legend->setTextPen(QColor(Qt::black));
    m_legend->setBorderPen(QColor(Qt::darkGray));
    QColor legendBg(Qt::white);
    legendBg.setAlpha(180);
    m_legend->setBorderRadius(5);
    m_legend->setBackgroundBrush(legendBg);
    m_legend->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_legend->attach(m_analyzer);

    // get list of selected items
    QList<GtProcessMonitoringItem*> items = m_model->selectedItems();

    // iterate over selected items
    foreach (GtProcessMonitoringItem* item, items)
    {
        // get parent item
        GtProcessMonitoringItem* parentItem =
                qobject_cast<GtProcessMonitoringItem*>(item->parent());

        // check parent item
        if (parentItem == Q_NULLPTR)
        {
            gtError() << tr("Monitoring item invalid!");
            return;
        }

        const QString propId = item->propertyId();
        const QString compUuid = parentItem->componentUuid();

        // check strings
        if (propId.isEmpty() || compUuid.isEmpty())
        {
            gtError() << tr("Monitoring item invalid!");
            return;
        }

        qDebug() << " -> " << compUuid << " (" << propId << ")";

        // create item speific plot curve
        createPlotCurve(compUuid, propId, item, parentItem);
    }

    m_analyzer->replot();
}
