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
#include "gt_task.h"
#include "gt_extendedtaskdata.h"
#include "gt_taskfactory.h"
#include "gt_palette.h"

#include "gt_calculatoroverviewpage.h"
#include "gt_calculatorsettingspage.h"

#include "gt_taskoverviewpage.h"

#include "gt_processwizard.h"

GtProcessWizard::GtProcessWizard(GtProject* project,
                                 GtCalculatorProvider* provider,
                                 QWidget* parent) :
    GtWizard(parent),
    m_provider(provider),
    m_scope(project),
    m_execSettingBtn(nullptr)
{
    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);

    setWindowTitle(tr("New Calculator Wizard"));

    if (gtApp->inDarkMode())
    {
        setAutoFillBackground(true);
        setWizardStyle(QWizard::WizardStyle::ModernStyle);
    }
    else
    {
        setWizardStyle(QWizard::AeroStyle);
        setAutoFillBackground(false);
    }

    if (!provider->componentInitialized())
    {
        setPage(startCalculatorPage, new GtCalculatorOverviewPage(this));
    }

    QString classname = provider->componentClassName();

    bool customWizard = false;

    if (!classname.isEmpty())
    {
        GtCalculatorData calcData =
            gtCalculatorFactory->calculatorData(classname);

        GtExtendedCalculatorDataImpl* eData =
            dynamic_cast<GtExtendedCalculatorDataImpl*>(
                calcData.get());

        if (eData)
        {
            setWindowIcon(eData->icon);

            QList<GtProcessWizardPage*> cPages;

            if (eData->wizard)
            {
                foreach (const QMetaObject& metaObj, eData->wizard->pages())
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
                    pwp->setScope(m_scope);

                    cPages << pwp;
                }
            }

            if (!cPages.isEmpty())
            {
                customWizard = true;

                setEnableExecutionSettings(true);

                foreach (GtProcessWizardPage* pwp, cPages)
                {
                    addPage(pwp);
                }

                QSize cwSize = eData->wizard->size();

                if (!cwSize.isEmpty())
                {
                    resize(cwSize);
                }
            }
        }
    }

    if (!customWizard)
    {
        setPage(calculatorSettingsPage, new GtCalculatorSettingsPage(m_scope,
                this));
    }
}

GtProcessWizard::GtProcessWizard(GtProject* project, GtTaskProvider* provider,
                                 QWidget* parent) :
    GtWizard(parent),
    m_provider(provider),
    m_scope(project),
    m_execSettingBtn(nullptr)
{
    setWindowTitle(tr("New Task Wizard"));

    if (gtApp->inDarkMode())
    {
        setAutoFillBackground(true);
        setWizardStyle(QWizard::WizardStyle::ModernStyle);
    }
    else
    {
        setWizardStyle(QWizard::AeroStyle);
        setAutoFillBackground(false);
    }

    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);

    if (!provider->componentInitialized())
    {
        setPage(startTaskPage, new GtTaskOverviewPage(this));
    }

    QString classname = provider->componentClassName();

    bool customWizard = false;

    if (!classname.isEmpty())
    {
        GtTaskData taskData = gtTaskFactory->taskData(classname);

        if (!taskData)
        {
            return;
        }

        setWindowTitle(tr("Config ") + taskData->id);

        GtExtendedTaskDataImpl* eData =
                dynamic_cast<GtExtendedTaskDataImpl*>(
                    taskData.get());

        if (eData)
        {
            // set icon
            setWindowIcon(eData->icon);

            QList<GtProcessWizardPage*> cPages;

            if (eData->wizard)
            {
                foreach (const QMetaObject& metaObj, eData->wizard->pages())
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
                    pwp->setScope(m_scope);

                    cPages << pwp;
                }
            }

            if (!cPages.isEmpty())
            {
                customWizard = true;

                foreach (GtProcessWizardPage* pwp, cPages)
                {
                    addPage(pwp);
                }

                QSize cwSize = eData->wizard->size();

                if (!cwSize.isEmpty())
                {
                    resize(cwSize);
                }
            }
        }
    }

    if (!customWizard)
    {
        setPage(calculatorSettingsPage, new GtCalculatorSettingsPage(m_scope,
                this));
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
//        if (m_execSettingBtn)
//        {
//            delete m_execSettingBtn;
//            m_execSettingBtn = nullptr;
//        }

//        setButton(QWizard::CustomButton1, m_execSettingBtn);
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

    QDialog dialog;
    dialog.setWindowTitle(tr("Execution Settings"));
    dialog.setWindowIcon(gtApp->icon(QStringLiteral("configIcon_16.png")));

    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    dialog.setWindowFlags(flags);

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
        qDebug() << "#### EXECUTION SETTINGS END";
        qDebug() << calc->toMemento().toByteArray();

        m_provider->setComponentData(calc->toMemento());
        currentPage()->initializePage();
    }

    delete calc;
}
