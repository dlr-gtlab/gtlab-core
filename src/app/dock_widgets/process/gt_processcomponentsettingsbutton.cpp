/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processcomponentsettingsbutton.cpp
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
#include "gt_extendedtaskdata.h"
#include "gt_stylesheets.h"
#include "gt_icons.h"

#include "gt_processcomponentsettingsbutton.h"

namespace {
    GtCustomProcessWizard* calcWizard(GtCalculator* calc)
    {
        if (!calc) return nullptr;

        GtCalculatorData calcData =
            gtCalculatorFactory->calculatorData(
                calc->metaObject()->className());

        auto* eData = dynamic_cast<GtExtendedCalculatorDataImpl*>(
            calcData.get());

        if (!eData) return nullptr;

        return eData->wizard;
    }

    GtCustomProcessWizard* taskWizard(GtTask* task)
    {
        if (!task) return nullptr;

        GtTaskData taskData = gtTaskFactory->taskData(
            task->metaObject()->className());

        auto* eData = dynamic_cast<GtExtendedTaskDataImpl*>(taskData.get());

        if (!eData) return nullptr;

        return eData->wizard;
    }
}


GtProcessComponentSettingsButton::GtProcessComponentSettingsButton() :
    m_pc(nullptr), m_task(nullptr)
{
    setText(tr("Config..."));
    setIcon(gt::gui::icon::config());
    setToolTip(tr("Open Wizard"));
    setStyleSheet(gt::gui::stylesheet::button());

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

    if (!m_pc) return;

    m_task = m_pc->rootTask();

    if (!m_task) return;

    connect(m_task.data(), SIGNAL(destroyed(QObject*)), SLOT(updateState()));
    connect(m_task.data(), SIGNAL(stateChanged(GtProcessComponent::STATE)),
            SLOT(updateState()));
}

bool
GtProcessComponentSettingsButton::hasCustomWizard()
{
    return processComponentWizard(m_pc) != nullptr;
}

GtCustomProcessWizard*
GtProcessComponentSettingsButton::processComponentWizard(
    GtProcessComponent* pc)
{
    if (auto* calc = qobject_cast<GtCalculator*>(pc))
    {
        return calcWizard(calc);
    }
    else if (auto* task = qobject_cast<GtTask*>(m_pc))
    {
        return taskWizard(task);
    }

    return nullptr;
}

void
GtProcessComponentSettingsButton::setProcessComponentByProvider(
    GtProcessComponent* pc, GtAbstractProcessProvider* provider)
{
    if (!provider) return;

    GtObjectMemento memento = provider->componentData();

    if (memento.isNull()) return;

    auto command = gtApp->makeCommand(pc, pc->objectName() +
                                      tr(" configuration changed"));
    Q_UNUSED(command)

    pc->fromMemento(memento);
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
    if (!m_pc) return;

    if (!m_pc->isReady()) return;

    if (auto* calc = qobject_cast<GtCalculator*>(m_pc))
    {
        if (GtCustomProcessWizard* pcWizard = calcWizard(calc))
        {
            GtCalculatorProvider provider(calc);
            GtProcessWizard wizard(gtApp->currentProject(), &provider,
                                   parentWidget());

            if (!wizard.exec()) return;

            setProcessComponentByProvider(calc, &provider);
        }
    }
    else if (auto* task = qobject_cast<GtTask*>(m_pc))
    {
        if (GtCustomProcessWizard* pcWizard = taskWizard(task))
        {
            GtTaskProvider provider(task);
            GtProcessWizard wizard(gtApp->currentProject(), &provider,
                                   parentWidget());

            if (!wizard.exec()) return;

            setProcessComponentByProvider(task, &provider);
        }
    }
}
