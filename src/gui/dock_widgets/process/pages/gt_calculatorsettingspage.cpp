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
#include "gt_palette.h"
#include "gt_application.h"

#include "gt_calculatorsettingspage.h"

GtCalculatorSettingsPage::GtCalculatorSettingsPage(GtProject* project,
        GtProcessWizard* parent) :
    QWizardPage(parent),
    m_wizard(parent),
    m_component(nullptr)
{
    setTitle(tr("Settings"));

    auto layout = new QVBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_view = new GtPropertyTreeView(project, this);
    m_view->setFrameStyle(QTreeView::NoFrame);

    layout->addWidget(m_view);

    auto searchWidget = new GtSearchWidget(this);
    layout->addWidget(searchWidget);

    setLayout(layout);

    connect(m_view, SIGNAL(searchRequest()), searchWidget,
            SLOT(enableSearch()));
    connect(searchWidget, SIGNAL(textEdited(QString)),
            m_view->filterModel(), SLOT(setFilterRegExp(QString)));
    connect(searchWidget, SIGNAL(textChanged(QString)),
            m_view->filterModel(), SLOT(setFilterRegExp(QString)));

    GtGUI::applyThemeToWidget(this);
}

void
GtCalculatorSettingsPage::initializePage()
{
    if (!m_wizard)
    {
        return;
    }

    if (!m_wizard->provider())
    {
        return;
    }


    GtObjectMemento memento = m_wizard->provider()->componentData();

    if (memento.isNull())
    {
        // TODO: error page
        return;
    }

    delete m_component;
    m_component = memento.restore<GtProcessComponent*>(gtProcessFactory);

    if (!m_component)
    {
        return;
    }

    m_component->setParent(this);

    connect(m_component.data(),
            SIGNAL(dataChanged(GtObject*,GtAbstractProperty*)),
            SLOT(onComponentDataChange()));

    m_view->setObject(m_component, false);

    QWizardPage::initializePage();
}

void
GtCalculatorSettingsPage::showEvent(QShowEvent* /*e*/)
{
    m_view->resizeColumns();
}

void
GtCalculatorSettingsPage::onComponentDataChange()
{
    if (!m_component)
    {
        return;
    }

    if (!m_wizard)
    {
        return;
    }

    if (!m_wizard->provider())
    {
        return;
    }

    m_wizard->provider()->setComponentData(m_component->toMemento());
}
