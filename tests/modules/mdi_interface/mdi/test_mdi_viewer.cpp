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

#include "gt_grid.h"
#include "gt_ruler.h"
#include "gt_logging.h"
#include "gt_colors.h"
#include "gt_icons.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>

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
    grid->setMajorLineColor(gt::gui::color::disabled());

    // axis pen
    pen = grid->axisPen();
    pen.setWidth(2);
    pen.setColor(gt::gui::color::warningText());
    grid->setAxisPen(pen);

    grid->setActiveAxis(Qt::Horizontal | Qt::Vertical);

    // scaling strategy
    grid->setScalingStrategy(GtGrid::ScalingStrategy::Base10);

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

    // overlay

    auto* showAllBtn = new QPushButton();
    showAllBtn->setToolTip(tr("Show Grid and Axis"));
    showAllBtn->setIcon(gt::gui::icon::eye());
    showAllBtn->setFlat(true);
    showAllBtn->setCheckable(true);
    showAllBtn->setChecked(true);

    connect(showAllBtn, &QPushButton::clicked, grid, [grid](){
        grid->setVisible(!grid->isVisible());
        gtInfo() << "is object visible?" << grid->isVisible();
    });

    auto* showGridBtn = new QPushButton();
    showGridBtn->setToolTip(tr("Toggle Grid"));
    showGridBtn->setIcon(gt::gui::icon::grid());
    showGridBtn->setFlat(true);
    showGridBtn->setCheckable(true);
    showGridBtn->setChecked(true);

    connect(showGridBtn, &QPushButton::clicked, grid, [grid](){
        grid->enableGrid(!grid->isGridEnabled());
        gtInfo() << "is grid visible?" << grid->isGridVisible();
    });

    auto* showMinorGridBtn = new QPushButton();
    showMinorGridBtn->setToolTip(tr("Toggle Minor Grid"));
    showMinorGridBtn->setIcon(gt::gui::icon::gridSnap());
    showMinorGridBtn->setFlat(true);
    showMinorGridBtn->setCheckable(true);
    showMinorGridBtn->setChecked(true);

    connect(showMinorGridBtn, &QPushButton::clicked, grid, [grid](){
        grid->enableMinorGrid(!grid->isMinorGridEnabled());
        gtInfo() << "is minor grid visible?" << grid->isMinorGridVisible();
    });

    auto* showAxisBtn = new QPushButton();
    showAxisBtn->setToolTip(tr("Toggle Active Axis"));
    showAxisBtn->setIcon(gt::gui::icon::add());
    showAxisBtn->setFlat(true);

    connect(showAxisBtn, &QPushButton::clicked, grid, [grid](){
        switch (grid->activeAxis())
        {
        default:
        case GtGrid::ActiveAxis{}:
            grid->setActiveAxis(Qt::Horizontal);
            break;
        case Qt::Horizontal:
            grid->setActiveAxis(Qt::Horizontal | Qt::Vertical);
            break;
        case Qt::Horizontal | Qt::Vertical:
            grid->setActiveAxis(Qt::Vertical);
            break;
        case Qt::Vertical:
            grid->setActiveAxis({});
            break;
        }

        auto flags = grid->activeAxis();
        gtInfo() << "active axis:"
                 << (flags.testFlag(Qt::Horizontal) ? "Horizontal": "-") << "|"
                 << (flags.testFlag(Qt::Vertical) ? "Vertical": "-")
                 << "\nany axis visible?"
                 << grid->isAxisVisible();
    });

    auto* gridSpacingBtn = new QPushButton();
    gridSpacingBtn->setToolTip(tr("Change Grid Spacing"));
    gridSpacingBtn->setIcon(gt::gui::icon::reload());
    gridSpacingBtn->setFlat(true);
    gridSpacingBtn->setCheckable(true);
    gridSpacingBtn->setChecked(true);

    connect(gridSpacingBtn, &QPushButton::clicked, grid, [state = true, grid]() mutable {
        if(state)
        {
            grid->setVSpacing(200);
            grid->setVSubdivisions(20);
        }
        else
        {
            grid->setSpacing(50);
            grid->setSubdivisions(5);
        }
        state = !state;
        gtInfo() << "hSpacing:" << grid->hSpacing() << "vSpacing:" << grid->vSpacing();
    });

    auto* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(showAllBtn);
    btnLayout->addWidget(showGridBtn);
    btnLayout->addWidget(showMinorGridBtn);
    btnLayout->addWidget(gridSpacingBtn);
    btnLayout->addWidget(showAxisBtn);
    btnLayout->addStretch();

    auto* overlay = new QVBoxLayout(m_view);
    overlay->addLayout(btnLayout);
    overlay->addStretch();
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
