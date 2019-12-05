/* GTlab - Gas Turbine laboratory
 * Source File: gt_calculatorsettingspage.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 12.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QDebug>

#include "gt_processwizard.h"
#include "gt_propertytreeview.h"
#include "gt_abstractprocessprovider.h"
#include "gt_processcomponent.h"
#include "gt_searchwidget.h"
#include "gt_treefiltermodel.h"
#include "gt_objectmemento.h"
#include "gt_processfactory.h"
#include "gt_project.h"

#include "gt_calculatorsettingspage.h"

GtCalculatorSettingsPage::GtCalculatorSettingsPage(GtProject* project,
        GtProcessWizard* parent) :
    QWizardPage(parent),
    m_wizard(parent),
    m_component(Q_NULLPTR)
{
    setTitle(tr("Settings"));

    QVBoxLayout* layout = new QVBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_view = new GtPropertyTreeView(project, this);
    m_view->setFrameStyle(QTreeView::NoFrame);

    layout->addWidget(m_view);

    GtSearchWidget* searchWidget = new GtSearchWidget(this);
    layout->addWidget(searchWidget);

    setLayout(layout);

    connect(m_view, SIGNAL(searchRequest()), searchWidget,
            SLOT(enableSearch()));
    connect(searchWidget, SIGNAL(textEdited(QString)),
            m_view->filterModel(), SLOT(setFilterRegExp(QString)));
    connect(searchWidget, SIGNAL(textChanged(QString)),
            m_view->filterModel(), SLOT(setFilterRegExp(QString)));
}

void GtCalculatorSettingsPage::initializePage()
{
    if (m_wizard == Q_NULLPTR)
    {
        return;
    }

    if (m_wizard->provider() == Q_NULLPTR)
    {
        return;
    }

    if (m_component != Q_NULLPTR)
    {
        delete m_component;
    }

    GtObjectMemento memento = m_wizard->provider()->componentData();

    if (memento.isNull())
    {
        // TODO: error page
        return;
    }

    m_component = memento.restore<GtProcessComponent*>(gtProcessFactory);

    if (m_component == Q_NULLPTR)
    {
        return;
    }

    m_component->setParent(this);

    connect(m_component.data(),
            SIGNAL(dataChanged(GtObject*, GtAbstractProperty*)),
            SLOT(onComponentDataChange()));

    m_view->setObject(m_component);
}

void
GtCalculatorSettingsPage::showEvent(QShowEvent* /*e*/)
{
    m_view->resizeColumns();
}

void
GtCalculatorSettingsPage::onComponentDataChange()
{
    if (m_component == Q_NULLPTR)
    {
        return;
    }

    if (m_wizard == Q_NULLPTR)
    {
        return;
    }

    if (m_wizard->provider() == Q_NULLPTR)
    {
        return;
    }

    m_wizard->provider()->setComponentData(m_component->toMemento());
}
