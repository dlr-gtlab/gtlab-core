/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QLabel>
#include <QListView>
#include <QDebug>

#include "gt_projectwizardmodules.h"
#include "gt_projectprovider.h"
#include "gt_checkablestringlistmodel.h"
#include "gt_application.h"

GtProjectWizardModules::GtProjectWizardModules(GtProjectProvider* provider,
                                               QWidget* parent) :
    GtAbstractProjectWizardPage(provider, parent)
{
    setTitle("Project Modules");

    QVBoxLayout* layout = new QVBoxLayout;

    QLabel* label = new QLabel(tr("Select project specific modules"));
    layout->addWidget(label);

    m_model = new GtCheckableStringListModel(
                  gtApp->moduleDatamodelInterfaceIds(), this);

    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            SIGNAL(completeChanged()));

    QListView* view = new QListView;
    view->setModel(m_model);
    view->setFrameStyle(QListView::NoFrame);
    layout->addWidget(view);

    layout->addStretch(1);

    setLayout(layout);
}

GtProjectWizardModules::~GtProjectWizardModules()
{
//    delete m_model;
}

bool
GtProjectWizardModules::validatePage()
{
    QStringList retval = m_model->selectedStringList();

    m_provider->setProjectModules(retval);

    return true;
}

bool
GtProjectWizardModules::isComplete() const
{
    QStringList retval = m_model->selectedStringList();

    if (retval.isEmpty())
    {
        return false;
    }

    return true;
}
