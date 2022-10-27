/* GTlab - Gas Turbine laboratory
 * Source File: gt_processcomponentsettingsbutton.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 13.10.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_application.h"
#include "gt_extendedcalculatordata.h"
#include "gt_calculatorfactory.h"
#include "gt_calculatorprovider.h"
#include "gt_taskfactory.h"
#include "gt_taskprovider.h"
#include "gt_processwizard.h"
#include "gt_objectmemento.h"
#include "gt_project.h"
#include "gt_task.h"
#include "gt_calculator.h"
#include "gt_command.h"
#include "gt_extendedtaskdata.h"
#include "gt_stylesheets.h"
#include "gt_icons.h"

#include "gt_processcomponentsettingsbutton.h"

GtProcessComponentSettingsButton::GtProcessComponentSettingsButton() :
    m_pc(nullptr), m_task(nullptr)
{
    setText(tr("Config..."));
    setIcon(gt::gui::icon::config16());
    setToolTip(tr("Open Wizard"));
    setStyleSheet(gt::gui::stylesheet::buttonStyleSheet());

    connect(this, SIGNAL(clicked(bool)), SLOT(openProcessComponentWizard()));

    updateState();
}

void
GtProcessComponentSettingsButton::setProcessComponent(GtProcessComponent* pc)
{
    if (m_task)
    {
        disconnect(m_task.data(), SIGNAL(destroyed(QObject*)),
                   this, SLOT(updateState()));
        disconnect(m_task.data(),
                   SIGNAL(stateChanged(GtProcessComponent::STATE)),
                   this, SLOT(updateState()));
    }

    m_pc = pc;

    updateState();

    if (!m_pc)
    {
        return;
    }

    m_task = m_pc->rootTask();

    if (!m_task)
    {
        return;
    }

    connect(m_task.data(), SIGNAL(destroyed(QObject*)), SLOT(updateState()));
    connect(m_task.data(), SIGNAL(stateChanged(GtProcessComponent::STATE)),
            SLOT(updateState()));
}

bool
GtProcessComponentSettingsButton::hasCustomWizard()
{
    if (!m_pc)
    {
        return false;
    }

    if (GtCalculator* calc = qobject_cast<GtCalculator*>(m_pc))
    {
        GtCalculatorData calcData =
            gtCalculatorFactory->calculatorData(
                calc->metaObject()->className());

        GtExtendedCalculatorDataImpl* eData =
            dynamic_cast<GtExtendedCalculatorDataImpl*>(
                calcData.get());

        if (eData && eData->wizard)
        {
            return true;
        }
    }
    else if (GtTask* task = qobject_cast<GtTask*>(m_pc))
    {
        GtTaskData taskData =
            gtTaskFactory->taskData(
                task->metaObject()->className());

        GtExtendedTaskDataImpl* eData =
            dynamic_cast<GtExtendedTaskDataImpl*>(
                taskData.get());

        if (eData && eData->wizard)
        {
            return true;
        }
    }

    return false;
}

void
GtProcessComponentSettingsButton::updateState()
{
    if (!m_task)
    {
        setEnabled(false);
        return;
    }

    if (!m_pc || !m_pc->isReady())
    {
        setEnabled(false);
    }
    else
    {
        setEnabled(true);
    }

    if (qobject_cast<GtCalculator*>(m_pc))
    {
        setText(tr("Config Calculator..."));
        setToolTip(tr("Open Calculator Wizard"));
    }
    else if (qobject_cast<GtTask*>(m_pc))
    {
        setText(tr("Config Task..."));
        setToolTip(tr("Open Task Wizard"));
    }
}

void
GtProcessComponentSettingsButton::openProcessComponentWizard()
{
    if (!m_pc)
    {
        return;
    }

    if (!m_pc->isReady())
    {
        return;
    }

    if (GtCalculator* calc = qobject_cast<GtCalculator*>(m_pc))
    {
        GtCalculatorData calcData =
            gtCalculatorFactory->calculatorData(
                calc->metaObject()->className());

        GtExtendedCalculatorDataImpl* eData =
            dynamic_cast<GtExtendedCalculatorDataImpl*>(
                calcData.get());

        if (eData && eData->wizard)
        {
            GtCalculatorProvider provider(calc);
            GtProcessWizard wizard(gtApp->currentProject(), &provider,
                                   parentWidget());

            if (!wizard.exec())
            {
                return;
            }

            GtObjectMemento memento = provider.componentData();

            if (memento.isNull())
            {
                return;
            }

            GtCommand command =
                gtApp->startCommand(gtApp->currentProject(),
                                    calc->objectName() +
                                    tr(" configuration changed"));
            calc->fromMemento(memento);
            gtApp->endCommand(command);
        }
    }
    else if (GtTask* task = qobject_cast<GtTask*>(m_pc))
    {
        GtTaskData taskData =
            gtTaskFactory->taskData(
                task->metaObject()->className());

        GtExtendedTaskDataImpl* eData =
            dynamic_cast<GtExtendedTaskDataImpl*>(
                taskData.get());

        if (eData && eData->wizard)
        {
            GtTaskProvider provider(task);
            GtProcessWizard wizard(gtApp->currentProject(), &provider,
                                   parentWidget());

            if (!wizard.exec())
            {
                return;
            }

            GtObjectMemento memento = provider.componentData();

            if (memento.isNull())
            {
                return;
            }

            GtCommand command =
                gtApp->startCommand(gtApp->currentProject(),
                                    task->objectName() +
                                    tr(" configuration changed"));
            task->fromMemento(memento);
            gtApp->endCommand(command);
        }
    }
}
