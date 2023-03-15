/* GTlab - Gas Turbine laboratory
 * Source File: gt_processwizard.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPushButton>
#include <QVBoxLayout>

#include "gt_calculatorprovider.h"
#include "gt_taskprovider.h"
#include "gt_project.h"
#include "gt_calculatorfactory.h"
#include "gt_extendedcalculatordata.h"
#include "gt_processwizardpage.h"
#include "gt_customprocesswizard.h"
#include "gt_application.h"
#include "gt_propertytreeview.h"
#include "gt_processfactory.h"
#include "gt_objectmemento.h"
#include "gt_calculator.h"
#include "gt_extendedtaskdata.h"
#include "gt_taskfactory.h"
#include "gt_icons.h"
#include "gt_dialog.h"

#include "gt_calculatoroverviewpage.h"
#include "gt_calculatorsettingspage.h"

#include "gt_taskoverviewpage.h"

#include "gt_processwizard.h"

template <typename ProcessData, typename Provider, typename ProcessDataGetter>
bool setupWizard(GtProcessWizard* wizard,
                 Provider* provider,
                 GtProject* scope,
                 ProcessDataGetter const& getProcessData)
{
    wizard->setWindowTitle(QObject::tr("New Calculator Wizard"));

    QString classname = provider->componentClassName();

    if (classname.isEmpty()) return false;

    auto calcData = getProcessData(classname);

    auto* eData =
            dynamic_cast<ProcessData*>(calcData.get());

    if (!eData) return false;

    wizard->setWindowIcon(eData->icon);

    if (!eData->wizard) return false;

    QList<GtProcessWizardPage*> cPages;

    for (const QMetaObject& metaObj : eData->wizard->pages())
    {
        QObject* obj = metaObj.newInstance();

        if (!obj)
        {
            cPages.clear();
            break;
        }

        GtProcessWizardPage* pwp =
                qobject_cast<GtProcessWizardPage*>(obj);

        if (!pwp)
        {
            delete obj;
            cPages.clear();
            break;
        }

        pwp->setProvider(provider);
        pwp->setScope(scope);

        cPages << pwp;
    }

    if (cPages.isEmpty()) return false;

    for (GtProcessWizardPage* pwp : qAsConst(cPages))
    {
        wizard->addPage(pwp);
    }

    QSize cwSize = eData->wizard->size();

    if (!cwSize.isEmpty())
    {
        wizard->resize(cwSize);
    }

    return true;
}

GtProcessWizard::GtProcessWizard(GtProject* project,
                                 GtCalculatorProvider* provider,
                                 QWidget* parent) :
    GtWizard(parent),
    m_provider(provider),
    m_scope(project),
    m_execSettingBtn(nullptr)
{
    if (!provider->componentInitialized())
    {
        setPage(startCalculatorPage, new GtCalculatorOverviewPage(this));
    }

    bool customWizard = setupWizard<GtExtendedCalculatorDataImpl>
                        (this, provider, m_scope, [](QString const& classname){
        return gtCalculatorFactory->calculatorData(classname);
    });

    if (!customWizard)
    {
        setPage(calculatorSettingsPage,
                new GtCalculatorSettingsPage(m_scope, this));
    }
    else
    {
        setEnableExecutionSettings(true);
    }
}

GtProcessWizard::GtProcessWizard(GtProject* project,
                                 GtTaskProvider* provider,
                                 QWidget* parent) :
    GtWizard(parent),
    m_provider(provider),
    m_scope(project),
    m_execSettingBtn(nullptr)
{
    if (!provider->componentInitialized())
    {
        setPage(startTaskPage, new GtTaskOverviewPage(this));
    }

    bool customWizard = setupWizard<GtExtendedTaskDataImpl>
                        (this, provider, m_scope, [](QString const& classname){
        return gtTaskFactory->taskData(classname);
    });

    if (!customWizard)
    {
        setPage(calculatorSettingsPage,
                new GtCalculatorSettingsPage(m_scope, this));
    }
}

GtAbstractProcessProvider*
GtProcessWizard::provider()
{
    return m_provider;
}

GtProject*
GtProcessWizard::scope()
{
    return m_scope;
}

void
GtProcessWizard::setEnableExecutionSettings(bool enabled)
{
    // add execution settings button
    if (enabled)
    {
        if (!m_execSettingBtn)
        {
            m_execSettingBtn = new QPushButton(this);
            connect(m_execSettingBtn, SIGNAL(clicked(bool)),
                    SLOT(executionSettings()), Qt::UniqueConnection);
        }

        setButton(QWizard::CustomButton1, m_execSettingBtn);
        setButtonText(QWizard::CustomButton1, tr("Execution Settings"));
        setOption(QWizard::HaveCustomButton1, true);

        // change button order
        QList<QWizard::WizardButton> layout;
        layout << QWizard::CustomButton1 << QWizard::Stretch <<
               QWizard::BackButton << QWizard::FinishButton <<
               QWizard::NextButton << QWizard::CancelButton;
        setButtonLayout(layout);
    }
    else
    {
        setOption(QWizard::HaveCustomButton1, false);

        QList<QWizard::WizardButton> layout;
        layout << QWizard::Stretch <<
                  QWizard::BackButton << QWizard::FinishButton <<
                  QWizard::NextButton << QWizard::CancelButton;
        setButtonLayout(layout);
    }
}

void
GtProcessWizard::executionSettings()
{
    if (!m_provider)
    {
        return;
    }

    currentPage()->validatePage();

    GtObjectMemento memento = m_provider->componentData();

    if (memento.isNull())
    {
        return;
    }

    GtCalculator* calc = memento.restore<GtCalculator*>(gtProcessFactory);

    if (!calc)
    {
        return;
    }

    GtDialog dialog;
    dialog.setWindowTitle(tr("Execution Settings"));
    dialog.setWindowIcon(gt::gui::icon::config());

    GtPropertyTreeView view(m_scope);
    view.setFrameStyle(GtPropertyTreeView::NoFrame);
    view.setCategoryFilter(QStringList() << tr("Execution"));
    view.setObject(calc);

    QVBoxLayout layout;
    layout.addWidget(&view);
    layout.setContentsMargins(0, 0, 0, 0);

    QHBoxLayout hLayout;
    hLayout.setContentsMargins(5, 5, 5, 5);
    layout.addLayout(&hLayout);

    hLayout.addStretch(1);

    QPushButton okBtn(tr("Ok"));
    QPushButton cancelBtn(tr("Cancel"));

    connect(&okBtn, SIGNAL(clicked(bool)), &dialog, SLOT(accept()));
    connect(&cancelBtn, SIGNAL(clicked(bool)), &dialog, SLOT(reject()));

    hLayout.addWidget(&okBtn);
    hLayout.addWidget(&cancelBtn);

    dialog.setLayout(&layout);
    dialog.resize(400, 200);

    if (dialog.exec())
    {
        m_provider->setComponentData(calc->toMemento());
        currentPage()->initializePage();
    }

    delete calc;
}
