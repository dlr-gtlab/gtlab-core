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
#include "gt_guiutilities.h"
#include "gt_objectuiaction.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenuBar>

TestMdiViewer::TestMdiViewer()
{
    setObjectName("Test Mdi Viewer");

    auto* scene = new QGraphicsScene();
    m_view = new GtGraphicsView(scene, GtGraphicsView::DestroyActiveSceneOnDeletion);

    auto* grid = new GtGrid(m_view);
    m_view->setGrid(grid);

    // dimensions
    auto resetGrid = [grid](GtObject* = nullptr){
        grid->setSpacing(50);
        grid->setSubdivisions(5);
    };
    resetGrid();

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
    auto showAction =
        gt::gui::makeAction(tr("Show"),
                            [grid](auto){ grid->setVisible(true); })
            .setVisibilityMethod([grid](auto){ return !grid->isVisible(); })
            .setIcon(gt::gui::icon::eye());

    auto hideAction =
        gt::gui::makeAction(tr("Hide"),
                            [grid](auto){ grid->setVisible(false); })
            .setVisibilityMethod([grid](auto){ return grid->isVisible(); })
            .setIcon(gt::gui::icon::eyeOff());

    auto showMajorGridAction =
        gt::gui::makeAction(tr("Enable Grid"),
                            [grid](auto){ grid->enableGrid(true); })
            .setVisibilityMethod([grid](auto){ return !grid->isGridEnabled(); })
            .setIcon(gt::gui::icon::grid());

    auto hideMajorGridAction =
        gt::gui::makeAction(tr("Disable Grid"),
                            [grid](auto){ grid->enableGrid(false); })
            .setVisibilityMethod([grid](auto){ return grid->isGridEnabled(); })
            .setIcon(gt::gui::colorize(gt::gui::icon::grid(),
                                       gt::gui::color::disabled()));

    auto showMinorGridAction =
        gt::gui::makeAction(tr("Enable Minor Grid"),
                            [grid](auto){ grid->enableMinorGrid(true); })
            .setVisibilityMethod([grid](auto){ return !grid->isMinorGridEnabled(); })
            .setIcon(gt::gui::icon::grid());

    auto hideMinorGridAction =
        gt::gui::makeAction(tr("Disable Minor Grid"),
                            [grid](auto){ grid->enableMinorGrid(false); })
            .setVisibilityMethod([grid](auto){ return grid->isMinorGridEnabled(); })
            .setIcon(gt::gui::colorize(gt::gui::icon::grid(),
                                       gt::gui::color::disabled()));

    auto hideAxisAction =
        gt::gui::makeAction(tr("Hide Axis"),
                            [grid](auto){ grid->setActiveAxis({}); })
            .setVisibilityMethod([grid](auto){ return grid->isAxisVisible(); })
            .setIcon(gt::gui::colorize(gt::gui::icon::mathPlus(),
                                       gt::gui::color::disabled()));

    auto showHAxisAction =
        gt::gui::makeAction(tr("Enable Horizontal Axis only"),
                            [grid](auto){ grid->setActiveAxis(Qt::Horizontal); })
            .setVisibilityMethod([grid](auto){
                return grid->activeAxis() != Qt::Horizontal;
            })
            .setIcon(gt::gui::icon::mathMinus());

    auto showVAxisAction =
        gt::gui::makeAction(tr("Enable Vertical Axis only"),
                            [grid](auto){ grid->setActiveAxis(Qt::Vertical); })
            .setVisibilityMethod([grid](auto){
                return grid->activeAxis() != Qt::Vertical;
            })
            .setIcon(gt::gui::icon::mathBar());

    auto showAllAxisAction =
        gt::gui::makeAction(tr("Enable Both Axis"),
                            [grid](auto){ grid->setActiveAxis(Qt::Horizontal | Qt::Vertical); })
            .setVisibilityMethod([grid](auto){
                return grid->activeAxis() != (Qt::Horizontal | Qt::Vertical);
            })
            .setIcon(gt::gui::icon::mathPlus());

    auto* menuBar = new QMenuBar;
    menuBar->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    menuBar->setContentsMargins(0, 0, 0, 0);

    constexpr int group3 = 1;
    constexpr int group1 = 1;
    QMenu* gridMenu = menuBar->addMenu(tr("Visibility"));
    gt::gui::addToMenu(
        {
            showMajorGridAction,
            hideMajorGridAction,
            showMinorGridAction,
            gt::gui::makeSeparator().setOrderPriority(group3),
            hideMinorGridAction,
            hideAxisAction.setOrderPriority(gt::gui::OrderPriority::Last), // placed last
            showHAxisAction.setOrderPriority(group3),
            showVAxisAction.setOrderPriority(group3),
            showAllAxisAction.setOrderPriority(group3),
            gt::gui::makeSeparator(group1),
            showAction.setOrderPriority(group1),
            hideAction.setOrderPriority(group1)
        },
        *gridMenu, nullptr);

    QKeySequence scIncSpacing = registerShortCut(
        "Increment Spacing", QKeySequence{Qt::Key_Plus  | Qt::ShiftModifier});
    QKeySequence scDecSpacing = registerShortCut(
        "Decrement Spacing", QKeySequence{Qt::Key_Minus | Qt::ShiftModifier});
    QKeySequence scIncSubdiv  = registerShortCut(
        "Increment Subdivisons", QKeySequence{Qt::Key_Plus  | Qt::ControlModifier});
    QKeySequence scDecSubdiv  = registerShortCut(
        "Decrement Subdivisons", QKeySequence{Qt::Key_Minus | Qt::ControlModifier});

    auto canIncrHSpacing = [grid](GtObject* = nullptr){ return grid->hSpacing() < 1000; };
    auto canIncrVSpacing = [grid](GtObject* = nullptr){ return grid->vSpacing() < 1000; };
    auto canDecrHSpacing = [grid](GtObject* = nullptr){ return grid->hSpacing() > 10; };
    auto canDecrVSpacing = [grid](GtObject* = nullptr){ return grid->vSpacing() > 10; };

    auto const incrHSpacing = [=](GtObject* = nullptr){
        if (canIncrHSpacing()) grid->setHSpacing(grid->hSpacing() + 10);
    };
    auto const incrVSpacing = [=](GtObject* = nullptr){
        if (canIncrVSpacing()) grid->setVSpacing(grid->vSpacing() + 10);
    };
    auto const decrHSpacing = [=](GtObject* = nullptr){
        if (canDecrHSpacing()) grid->setHSpacing(grid->hSpacing() - 10);
    };
    auto const decrVSpacing = [=](GtObject* = nullptr){
        if (canDecrVSpacing()) grid->setVSpacing(grid->vSpacing() - 10);
    };

    auto incrementHSpacingAction =
        gt::gui::makeAction(tr("Horizontal Spacing +10"), incrHSpacing)
            .setVerificationMethod(canIncrHSpacing)
            .setIcon(gt::gui::icon::mathPlus())
            .setShortCut(scIncSpacing);

    auto decrementHSpacingAction =
        gt::gui::makeAction(tr("Horizontal Spacing -10"), decrHSpacing)
            .setVerificationMethod(canDecrHSpacing)
            .setIcon(gt::gui::icon::mathPlus())
            .setShortCut(scDecSpacing);

    auto incrementVSpacingAction =
        gt::gui::makeAction(tr("Vertical Spacing +10"), incrVSpacing)
            .setVerificationMethod(canIncrVSpacing)
            .setIcon(gt::gui::icon::mathPlus());

    auto decrementVSpacingAction =
        gt::gui::makeAction(tr("Vertical Spacing -10"), decrVSpacing)
            .setVerificationMethod(canDecrVSpacing)
            .setIcon(gt::gui::icon::mathMinus());

    auto canIncrHSubdivs = [grid](GtObject* = nullptr){ return grid->hSubdivisions() < 20; };
    auto canIncrVSubdivs = [grid](GtObject* = nullptr){ return grid->vSubdivisions() < 20; };
    auto canDecrHSubdivs = [grid](GtObject* = nullptr){ return grid->hSubdivisions() > 1; };
    auto canDecrVSubdivs = [grid](GtObject* = nullptr){ return grid->vSubdivisions() > 1; };

    auto const incrHSubdivs = [=](GtObject* = nullptr){
        if (canIncrHSubdivs()) grid->setHSubdivisions(grid->hSubdivisions() + 1);
    };
    auto const incrVSubdivs = [=](GtObject* = nullptr){
        if (canIncrVSubdivs()) grid->setVSubdivisions(grid->hSubdivisions() + 1);
    };
    auto const decrHSubdivs = [=](GtObject* = nullptr){
        if (canDecrHSubdivs()) grid->setHSubdivisions(grid->hSubdivisions() - 1);
    };
    auto const decrVSubdivs = [=](GtObject* = nullptr){
        if (canDecrVSubdivs()) grid->setVSubdivisions(grid->vSubdivisions() - 1);
    };

    auto incrementHSubdivsAction =
        gt::gui::makeAction(tr("Horizontal Subdivisons +1"), incrHSubdivs)
            .setVerificationMethod(canIncrHSubdivs)
            .setIcon(gt::gui::icon::mathMinus())
            .setShortCut(scIncSubdiv);

    auto decrementHSubdivsAction =
        gt::gui::makeAction(tr("Horizontal Subdivisons -1"), decrHSubdivs)
            .setVerificationMethod(canDecrHSubdivs)
            .setIcon(gt::gui::icon::mathMinus())
            .setShortCut(scDecSubdiv);

    auto incrementVSubdivsAction =
        gt::gui::makeAction(tr("Vertical Subdivisons +1"), incrVSubdivs)
            .setVerificationMethod(canIncrVSubdivs)
            .setIcon(gt::gui::icon::mathMinus());

    auto decrementVSubdivsAction =
        gt::gui::makeAction(tr("Vertical Subdivisons -1"), decrVSubdivs)
            .setVerificationMethod(canDecrVSubdivs)
            .setIcon(gt::gui::icon::mathMinus());

    auto resetGridAction=
        gt::gui::makeAction(tr("Reset"), resetGrid)
            .setIcon(gt::gui::icon::revert());

    QMenu* spacingMenu = menuBar->addMenu(tr("Spacing"));
    gt::gui::addToMenu(
        {
            incrementHSpacingAction,
            decrementHSpacingAction,
            gt::gui::makeSeparator(),
            incrementVSpacingAction,
            decrementVSpacingAction,
            gt::gui::makeSeparator(),
            incrementHSubdivsAction,
            decrementHSubdivsAction,
            gt::gui::makeSeparator(),
            incrementVSubdivsAction,
            decrementVSubdivsAction,
            gt::gui::makeSeparator(gt::gui::OrderPriority::First), // placed 2nd
            resetGridAction.setOrderPriority(gt::gui::OrderPriority::First) // placed 1st
        },
        *spacingMenu, nullptr);

    auto* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(menuBar);
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
