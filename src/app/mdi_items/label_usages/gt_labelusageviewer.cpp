/* GTlab - Gas Turbine laboratory
 * Source File: gt_labelusageviewer.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 21.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QListView>

#include "gt_label.h"
#include "gt_labelcontainer.h"
#include "gt_logging.h"
#include "gt_project.h"
#include "gt_label.h"
#include "gt_labelusagemodel.h"
#include "gt_labelusagedelegate.h"

#include "gt_labelusageviewer.h"

GtLabelUsageViewer::GtLabelUsageViewer()
{
    setObjectName(tr("Label Usage"));

    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);

    QFrame* frame = new QFrame;
    QVBoxLayout* frameLayout = new QVBoxLayout;
    frame->setLayout(frameLayout);

    frame->setAutoFillBackground(true);

    m_label = new QLabel(tr("No usage found!"));
    frameLayout->addWidget(m_label);

    m_mainView = new QListView;
    m_mainView->setFrameStyle(QListView::NoFrame);
    frameLayout->addWidget(m_mainView);

    lay->addWidget(frame);

    widget()->setLayout(lay);

    m_model = new GtLabelUsageModel(this);
    m_mainView->setModel(m_model);
    m_mainView->setAlternatingRowColors(true);
    m_mainView->setItemDelegate(new GtLabelUsageDelegate(this));
}

void
GtLabelUsageViewer::setData(GtObject* obj)
{
    GtLabelContainer* container = dynamic_cast<GtLabelContainer*>(obj);

    if (!container || !container->isValid())
    {
        return;
    }

    QList<GtLabel*> labelList = container->project()->findLabelUsages(
                                    container->label());

    QString nfo = QStringLiteral("<b>") + QString::number(labelList.size()) +
                  QStringLiteral(" ") + tr("usages found!") +
                  QStringLiteral("</b>");

    QString title = container->label()->objectName() + QStringLiteral(" ") +
                    tr("Usage") + QStringLiteral(" (") +
                    container->project()->objectName() + QStringLiteral(")");

    setObjectName(title);

    m_label->setText(nfo);

    m_model->setLabelUsage(labelList);
}
