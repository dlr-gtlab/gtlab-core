/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 27.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QFrame>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>

#include "gt_propertiesdock.h"
#include "gt_propertytreeview.h"
#include "gt_searchwidget.h"
#include "gt_treefiltermodel.h"
#include "gt_application.h"
#include "gt_processcomponentsettingsbutton.h"
#include "gt_project.h"
#include "gt_command.h"
#include "gt_logging.h"

GtPropertiesDock::GtPropertiesDock() : m_obj(Q_NULLPTR)
{
    setObjectName(tr("Properties"));

    QFrame* frame = new QFrame;

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    frame->setLayout(layout);

    QHBoxLayout* hLay = new QHBoxLayout;
    hLay->setContentsMargins(0, 0, 0, 0);

    m_label = new QLabel;
    m_label->setVisible(false);
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setMinimumHeight(20);
    //m_label->setStyleSheet("QLabel { background-color : #f2f3f5; "
    //                       "color : black; }");
    hLay->addWidget(m_label);

    layout->addLayout(hLay);

    m_processComponentSettingBtn = new GtProcessComponentSettingsButton;
    m_processComponentSettingBtn->setVisible(false);

    m_treeView = new GtPropertyTreeView(gtApp->session());

    m_treeView->setColumnWidth(0, 100);
    m_treeView->setColumnWidth(1, 70);
    m_treeView->setColumnWidth(2, 50);

    layout->addWidget(m_processComponentSettingBtn);

    layout->addWidget(m_treeView);

    QHBoxLayout* toolbarLayout = new QHBoxLayout;
    toolbarLayout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(toolbarLayout);

    m_search = new GtSearchWidget;

    toolbarLayout->addWidget(m_search);

    //    toolbarLayout->addWidget(calcSettingBtn);

    frame->setAutoFillBackground(true);
    frame->setFrameShape(m_treeView->frameShape());
    frame->setFrameStyle(m_treeView->frameStyle());
    frame->setFrameShadow(m_treeView->frameShadow());

    m_treeView->setFrameStyle(QTreeView::NoFrame);

    setWidget(frame);

    connect(m_treeView, SIGNAL(searchRequest()), m_search,
            SLOT(enableSearch()));
    connect(m_search, SIGNAL(textEdited(QString)),
            m_treeView->filterModel(), SLOT(setFilterRegExp(QString)));
    connect(m_search, SIGNAL(textChanged(QString)),
            m_treeView->filterModel(), SLOT(setFilterRegExp(QString)));
    connect(gtApp, SIGNAL(objectSelected(GtObject*)),
            SLOT(objectSelected(GtObject*)));
}

Qt::DockWidgetArea
GtPropertiesDock::getDockWidgetArea()
{
    return Qt::RightDockWidgetArea;
}

void
GtPropertiesDock::objectSelected(GtObject* obj)
{
    m_treeView->setObject(obj);
    m_processComponentSettingBtn->setVisible(false);

    if (obj != Q_NULLPTR)
    {
        m_treeView->setScope(obj->findParent<GtProject*>());

    }

    if (m_obj != Q_NULLPTR)
    {
        disconnect(m_obj, SIGNAL(destroyed(QObject*)), this,
                   SLOT(refreshTitle()));
        disconnect(m_obj, SIGNAL(objectNameChanged(QString)), this,
                   SLOT(refreshTitle()));
    }

    m_obj = obj;

    if (m_obj != Q_NULLPTR)
    {
        connect(m_obj.data(), SIGNAL(destroyed(QObject*)),
                SLOT(refreshTitle()));
        connect(m_obj.data(), SIGNAL(objectNameChanged(QString)),
                SLOT(refreshTitle()));
    }

    m_processComponentSettingBtn->setProcessComponent(
        qobject_cast<GtProcessComponent*>(obj));

    if (m_processComponentSettingBtn->hasCustomWizard())
    {
        m_processComponentSettingBtn->setVisible(true);
    }

    refreshTitle();
}

void
GtPropertiesDock::refreshTitle()
{
    if (m_obj == Q_NULLPTR)
    {
        m_label->setVisible(false);
        return;
    }

    m_label->setVisible(true);

    if (gtApp->devMode())
    {
        m_label->setText("<b>" + m_obj->objectName() + "</b> - " +
                         m_obj->metaObject()->className());
    }
    else
    {
        m_label->setText("<b>" + m_obj->objectName() + "</b>");
    }
}
