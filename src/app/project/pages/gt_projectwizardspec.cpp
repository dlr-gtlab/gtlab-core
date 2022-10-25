/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QGridLayout>
#include <QDir>
#include <QDebug>
#include <QLineEdit>

#include "gt_projectwizardspec.h"
#include "gt_projectprovider.h"
#include "gt_projectspecwidget.h"

GtProjectWizardSpec::GtProjectWizardSpec(GtProjectProvider* provider,
                                         QWidget* parent) :
    GtAbstractProjectWizardPage(provider, parent)
{
    setTitle(tr("Project Specifications"));

    QVBoxLayout* layout = new QVBoxLayout;

    m_specWidget = new GtProjectSpecWidget;
    layout->addWidget(m_specWidget);

    setLayout(layout);

    // fields
    registerField(QStringLiteral("spec.projectname*"),
                  m_specWidget->nameLine());

    registerField(QStringLiteral("spec.projectpath*"),
                  m_specWidget->pathLine());

    connect(m_specWidget, SIGNAL(statesUpdated()), SIGNAL(completeChanged()));
}

bool
GtProjectWizardSpec::isComplete() const
{
    return m_specWidget->isValid();
}

bool
GtProjectWizardSpec::validatePage()
{
    m_provider->setProjectName(m_specWidget->nameLine()->text());
    m_provider->setProjectPath(m_specWidget->pathLine()->text());

    return true;
}
