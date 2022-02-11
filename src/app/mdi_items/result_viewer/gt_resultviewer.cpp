/* GTlab - Gas Turbine laboratory
 * Source File: gt_resultviewer.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 04.08.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QGridLayout>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>

#include "gt_application.h"
#include "gt_resulttableview.h"
#include "gt_datazonemodel.h"
#include "gt_abstractdatazone.h"
#include "gt_datazone.h"
#include "gt_stylesheets.h"
#include "gt_externalizedobjecthelper.h"

#include "gt_resultviewer.h"

GtResultViewer::GtResultViewer() :
    m_label(Q_NULLPTR),
    m_model(Q_NULLPTR),
    m_axSelection(Q_NULLPTR),
    m_tickSelection(Q_NULLPTR),
    m_fixedValueLabel(Q_NULLPTR)
{
    setObjectName("Result Viewer");

    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    m_label = new QLabel;
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setMinimumHeight(20);
    m_label->setStyleSheet(GtStyleSheets::resultViewerTitleLabel());
    lay->addWidget(m_label);

    QGridLayout* grid = new QGridLayout;
    lay->addLayout(grid);

    m_axSelection = new QComboBox;
    m_tickSelection = new QComboBox;

    m_fixedValueLabel = new QLabel("fixed Value:");
    m_fixedValueLabel->setAlignment(Qt::AlignCenter);

    grid->addWidget(m_fixedValueLabel, 0, 0);
    grid->addWidget(m_axSelection, 0, 1);
    grid->addWidget(m_tickSelection, 0, 2);


    m_axSelection->setVisible(false);
    m_tickSelection->setVisible(false);
    m_fixedValueLabel->setVisible(false);

    GtResultTableView* editor = new GtResultTableView(widget());
    editor->setFrameStyle(GtResultTableView::NoFrame);

    m_model = new GtDataZoneModel(this);
    editor->setModel(m_model);

    widget()->setLayout(lay);

    lay->addWidget(editor);
}

GtResultViewer::~GtResultViewer()
{

}

QIcon
GtResultViewer::icon() const
{
    return gtApp->icon(QStringLiteral("prpIcon_16.png"));
}

void
GtResultViewer::setData(GtObject* obj)
{
    m_model->clearResultData();

    GtExternalizedObjectHelper<GtAbstractDataZone> dataZone
            (obj, GtExternalizedObject::Discard);

    if (!dataZone.isValid())
    {
        return;
    }

    GtDataZone* dz = qobject_cast<GtDataZone*>(dataZone);

    if (dz)
    {
        if (dz->nDims() == 2)
        {
            m_axSelection->setVisible(true);
            m_tickSelection->setVisible(true);
            m_fixedValueLabel->setVisible(true);

            m_axSelection->insertItems(0, dz->axisNames());

            int index = m_axSelection->currentIndex();

            foreach (double val, dz->allAxisTicks().at(index))
            {
                m_tickSelection->addItem(QString::number(val));
            }

            connect(m_axSelection, SIGNAL(currentIndexChanged(int)),
                    this, SLOT(axSelectorChanged(int)));

            connect(m_tickSelection, SIGNAL(currentIndexChanged(int)),
                    this, SLOT(tickSelectorChanged(int)));
        }
    }

    m_model->setResultData(dataZone);

    m_model->m_indexOfInterestAx1 = 0;

    QString str = dataZone->objectPath();

    QStringList strList = str.split(QStringLiteral(";"));

    QString id;

    if (strList.size() >= 4)
    {
        for (int i = 0; i < 3; i++)
        {
            id = id + strList.at(strList.size() - 4 + i) +
                 QStringLiteral(" - ");
        }
        id = id + strList.last();
    }

    id = QStringLiteral("<b>") + id + QStringLiteral("</b>");

    m_label->setText(id);

    connect(dataZone, SIGNAL(destroyed(QObject*)), m_model,
            SLOT(clearResultData()));
    connect(dataZone, SIGNAL(destroyed(QObject*)), SLOT(deleteLater()));
}

void
GtResultViewer::axSelectorChanged(int index)
{
    m_tickSelection->clear();

    GtAbstractDataZone* dataZone = m_model->getResultData();

    if (dataZone == Q_NULLPTR)
    {
        return;
    }

    GtDataZone* dz = qobject_cast<GtDataZone*>(dataZone);

    if (dz)
    {
        if (dz->nDims() == 2)
        {
            foreach (double val, dz->allAxisTicks().at(index))
            {
                m_tickSelection->addItem(QString::number(val));
            }
        }
    }

    m_model->m_axOfInterest = index;

    m_model->axIndexChanges(index);
}

void
GtResultViewer::tickSelectorChanged(int index)
{
    m_model->m_indexOfInterestAx1 = index;

    m_model->tickIndexChanges(index);
}
