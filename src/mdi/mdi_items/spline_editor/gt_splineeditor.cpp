/* GTlab - Gas Turbine laboratory
 * Source File: gt_splineeditor.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 17.12.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QTreeView>

#include "gt_splineplot.h"
#include "gt_spline.h"

#include "gt_splineeditor.h"

GtSplineEditor::GtSplineEditor()
{
    setObjectName(tr("Spline Editor"));

    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    m_plot = new GtSplinePlot;
    lay->addWidget(m_plot);

    widget()->setLayout(lay);
}

void
GtSplineEditor::setData(GtObject* obj)
{
    GtSpline* spline = qobject_cast<GtSpline*>(obj);

    m_plot->initPlot(spline);

    m_plot->setTitle("Spline:" + spline->objectName());

    connect(obj, SIGNAL(destroyed(QObject*)),
            SLOT(deleteLater()));
}
