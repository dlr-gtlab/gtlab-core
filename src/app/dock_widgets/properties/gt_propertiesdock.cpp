/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 27.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QFrame>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QResizeEvent>
#include <QTabWidget>

#include "gt_propertiesdock.h"
#include "gt_propertytreeview.h"
#include "gt_searchwidget.h"
#include "gt_treefiltermodel.h"
#include "gt_application.h"
#include "gt_processcomponentsettingsbutton.h"
#include "gt_project.h"

#include "gt_logging.h"
#include "gt_propertystructcontainer.h"
#include "gt_propertycontainerwidget.h"

GtPropertiesDock::GtPropertiesDock() : m_obj(nullptr)
{
    setObjectName(tr("Properties"));

    QFrame* frame = new QFrame;

    m_mainLayout = new QVBoxLayout;
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    frame->setLayout(m_mainLayout);

    QHBoxLayout* hLay = new QHBoxLayout;
    hLay->setContentsMargins(0, 0, 0, 0);

    m_label = new QLabel;
    m_label->setVisible(false);
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setMinimumHeight(20);
    m_label->setMinimumWidth(20);
    hLay->addWidget(m_label);

    m_mainLayout->addLayout(hLay);

    m_processComponentSettingBtn = new GtProcessComponentSettingsButton;
    m_processComponentSettingBtn->setVisible(false);

    m_tab = new QTabWidget;

    m_treeView = new GtPropertyTreeView(gtApp->session());

    m_treeView->setColumnWidth(0, 100);
    m_treeView->setColumnWidth(1, 70);
    m_treeView->setColumnWidth(2, 50);
    m_treeView->setFrameStyle(QTreeView::NoFrame);

    m_mainLayout->addWidget(m_processComponentSettingBtn);

    m_tabLayout = new QVBoxLayout;
    m_tabLayout->setContentsMargins(0, 0, 0, 0);

    auto* mainTab = new QWidget;
    mainTab->setLayout(m_tabLayout);

    m_tab->addTab(mainTab, tr("Main"));
    m_tab->hide();

    m_mainLayout->addWidget(m_tab);
    m_mainLayout->addWidget(m_treeView);

    QHBoxLayout* toolbarLayout = new QHBoxLayout;
    toolbarLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->addLayout(toolbarLayout);

    m_search = new GtSearchWidget;

    toolbarLayout->addWidget(m_search);

    //    toolbarLayout->addWidget(calcSettingBtn);

    frame->setAutoFillBackground(true);

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
GtPropertiesDock::resizeEvent(QResizeEvent* /*event*/)
{
    refreshTitle();
}

void
GtPropertiesDock::objectSelected(GtObject* obj)
{
    // stop repainting until everything is ready
    // this removes flickering of the widget
    this->setUpdatesEnabled(false);
    auto _ = gt::finally([this](){
        this->setUpdatesEnabled(true);
    });

    // clear tab
    while (m_tab->count() > 1)
    {
        QWidget* tmpWid = m_tab->widget(m_tab->count() - 1);
        m_tab->removeTab(m_tab->count() - 1);
        delete tmpWid;
    }

    m_treeView->setObject(obj);
    m_processComponentSettingBtn->setVisible(false);

    if (obj)
    {
        m_treeView->setScope(obj->findParent<GtProject*>());

        // move view to active layout
        bool showTabWidget = !obj->propertyContainers().empty();
        auto* activeLayout = showTabWidget ? m_tabLayout : m_mainLayout;

        int count = activeLayout->count();
        activeLayout->insertWidget(gt::clamp(count, 0, count - 2), m_treeView);

        // check for property container
        for (GtPropertyStructContainer& c : obj->propertyContainers())
        {
            // do not draw if hidden
            if (c.getFlags() & GtPropertyStructContainer::IsHidden) continue;

            GtPropertyContainerWidget* wid = new GtPropertyContainerWidget(
                        obj, c, m_tab);
            m_tab->addTab(wid, c.name());
        }

        m_tab->setVisible(showTabWidget);
    }

    if (m_obj)
    {
        disconnect(m_obj, SIGNAL(destroyed(QObject*)), this,
                   SLOT(refreshTitle()));
        disconnect(m_obj, SIGNAL(objectNameChanged(QString)), this,
                   SLOT(refreshTitle()));
    }

    m_obj = obj;

    if (m_obj)
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
    if (!m_obj)
    {
        m_label->setVisible(false);
        return;
    }

    m_label->setVisible(true);

    auto getText = [](QString const& name, QString const& classname,
                      bool devMode)
    {
        if (devMode)
        {
            return QString("<b>" + name + "</b>") + " - " + classname;
        }
        else
        {
            return QString("<b>" + name + "</b>");
        }
    };

    QString display = getText(m_obj->objectName(),
                              m_obj->metaObject()->className(),
                              gtApp->devMode());

    int removeCounter = 4;
    QFont labelFont = m_label->font();
    /// this line does not change the displayed font.
    /// It is needed for the font metrics to calculate the size based on a
    /// bold font.
    labelFont.setBold(true);
    QFontMetrics fm(labelFont);
    QString choppedName = m_obj->objectName();

    /// check if text is too long for current label
    /// but with a minimum number of 12 letters for the name
    /// which means 9 exculding the truncation points (...)
    while (fm.horizontalAdvance(display) > width() && choppedName.size() > 12)
    {
        choppedName = m_obj->objectName();
        choppedName.chop(removeCounter);
        removeCounter += 1;
        choppedName += "...";

        display = getText(choppedName, m_obj->metaObject()->className(),
                          gtApp->devMode());
    }

    m_label->setText(display);
}
