/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 18.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QPushButton>

#include "gt_processeditor.h"
#include "gt_processeditorview.h"
#include "gt_processeditorscene.h"
#include "gt_graphicsscene.h"
#include "gt_task.h"
#include "gt_taskentity.h"
#include "gt_tasklinkentity.h"
#include "gt_taskentityshadow.h"
//#include "gt_mdistate.h"
#include "gt_icons.h"

GtProcessEditor::GtProcessEditor()
{
    setObjectName(tr("Process Editor"));

    // state initialization
//    m_showLegendState = initializeGlobalState(tr("Show Legend"), true);

    m_scene = new GtProcessEditorScene;
    m_view = new GtProcessEditorView(m_scene, widget());
    m_view->setAutoFillBackground(true);
    m_scene->setBackgroundBrush(QBrush(QColor(250, 250, 250)));

    m_view->setRenderHint(QPainter::Antialiasing, true);

    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    widget()->setLayout(lay);

    lay->addWidget(m_view);

    m_view->setDragMode(QGraphicsView::RubberBandDrag);
    m_view->setRubberBandSelectionMode(Qt::ContainsItemShape);
    m_view->setMouseTracking(true);

    QHBoxLayout* hLay = new QHBoxLayout;
    hLay->setContentsMargins(0, 0, 0, 0);
    hLay->setSpacing(0);

    // legend button
    m_showLegendButton = new QPushButton;
    m_showLegendButton->setIcon(GtGUI::Icon::infoBlue16());
    m_showLegendButton->setMaximumSize(QSize(20, 20));
    m_showLegendButton->setFlat(true);
    m_showLegendButton->setCheckable(true);
    m_showLegendButton->setToolTip(tr("Show Legend"));
    hLay->addWidget(m_showLegendButton);

    // go back button
    QPushButton* goBackButton = new QPushButton;
    goBackButton->setIcon(
                GtGUI::icon(QStringLiteral("arrowleftIcon.png")));
    goBackButton->setMaximumSize(QSize(20, 20));
    goBackButton->setFlat(true);
    goBackButton->setToolTip(tr("Go Back"));
    hLay->addWidget(goBackButton);
    connect(goBackButton, SIGNAL(clicked(bool)), SLOT(goBack()));

    // go forward button
    QPushButton* goForwardButton = new QPushButton;
    goForwardButton->setIcon(
                GtGUI::icon(QStringLiteral("arrowrightIcon.png")));
    goForwardButton->setMaximumSize(QSize(20, 20));
    goForwardButton->setFlat(true);
    goForwardButton->setToolTip(tr("Go Farward"));
    hLay->addWidget(goForwardButton);
    connect(goForwardButton, SIGNAL(clicked(bool)), SLOT(goForward()));

    hLay->addStretch();

    // temporary preview mode button
    QPushButton* previewButton = new QPushButton;
    previewButton->setIcon(GtGUI::Icon::results16());
    previewButton->setMaximumSize(QSize(20, 20));
    previewButton->setFlat(true);
    previewButton->setToolTip(tr("Preview Mode"));
    hLay->addWidget(previewButton);
    connect(previewButton, SIGNAL(clicked(bool)), m_scene,
            SLOT(setPreviewMode()));

    // show properties button
    m_showPropertiesButton = new QPushButton;
    m_showPropertiesButton->setIcon(
                GtGUI::icon(QStringLiteral("minimizeIcons.png")));
    m_showPropertiesButton->setMaximumSize(QSize(20, 20));
    m_showPropertiesButton->setFlat(true);
    m_showPropertiesButton->setCheckable(true);
    m_showPropertiesButton->setToolTip(tr("Toggle Property Mode"));

    hLay->addWidget(m_showPropertiesButton);

    lay->addLayout(hLay);

    connect(m_showLegendButton, SIGNAL(clicked(bool)),
            SLOT(toggleLegend(bool)));
}

void
GtProcessEditor::setData(GtObject* obj)
{
    if (!obj)
    {
        return;
    }

    if (GtTask* task = qobject_cast<GtTask*>(obj))
    {
        m_scene->setRoot(task);
    }
}

void
GtProcessEditor::objectChangedEvent(GtObject* /*obj*/)
{

}

//void
//GtProcessEditor::onStateChangedEvent(const GtMdiState* state)
//{
//    if (state == m_showLegendState)
//    {
//        bool val = m_showLegendState->getValue().toBool();
//        m_showLegendButton->setChecked(val);
//        m_view->setShowLegend(val);
//    }
//}

void
GtProcessEditor::initialized()
{
//    m_showLegendButton->setChecked(m_showLegendState->getValue().toBool());
}

void
GtProcessEditor::toggleLegend(bool /*val*/)
{
//    m_showLegendState->setValue(val);
}

void
GtProcessEditor::goBack()
{

}

void
GtProcessEditor::goForward()
{

}

