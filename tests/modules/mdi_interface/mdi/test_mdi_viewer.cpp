/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 * Created on: 12.08.2020
 * Author: M. Bröcker
 */

#include "test_mdi_viewer.h"

#include "gt_graphicsview.h"
#include "gt_graphicsscene.h"

#include "gt_grid.h"
#include "gt_ruler.h"
#include "gt_logging.h"
#include "gt_colors.h"

#include <QGridLayout>

TestMdiViewer::TestMdiViewer()
{
    setObjectName("Test Mdi Viewer");

    auto* scene = new QGraphicsScene();
    m_view = new GtGraphicsView(scene, GtGraphicsView::DestroyActiveSceneOnDeletion);

    auto* grid = new GtGrid(m_view);
    m_view->setGrid(grid);

    // dimensions
    grid->setSpacing(50);
    grid->setSubdivisions(5);

    // minor pen
    QPen pen;
    pen = grid->minorPen();
    pen.setStyle(Qt::DotLine);
    grid->setMinorPen(pen);
    grid->setMinorLineColor(gt::gui::color::disabled());

    // major pen
    grid->setLineColor(gt::gui::color::disabled());

    // axis pen
    pen = grid->axisPen();
    pen.setWidth(2);
    pen.setColor(gt::gui::color::warningText());
    grid->setAxisPen(pen);

    grid->setShowAxis(true);
    grid->setVisibleAxis(Qt::Horizontal | Qt::Vertical);

    // scaling strategy
    grid->setScalingStrategy(GtGrid::Base10);

    // rulers
    auto* hruler = new GtRuler(Qt::Horizontal);
    auto* vruler = new GtRuler(Qt::Vertical);
    m_view->connectHorizontalRuler(hruler);
    m_view->connectVerticalRuler(vruler);

    QFont font = vruler->font();
    font.setItalic(true);
    vruler->setFont(font);
    hruler->setFont(font);

    vruler->flipAxis(true);

    auto* lay = new QGridLayout();
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    lay->addWidget(hruler, 0, 1);
    lay->addWidget(vruler, 1, 0);
    lay->addWidget(m_view, 1, 1);

    widget()->setLayout(lay);
}

TestMdiViewer::~TestMdiViewer()
{
    gtDebug() << __FUNCTION__;
}

bool
TestMdiViewer::allowsMultipleInstances() const
{
    return false;
}
