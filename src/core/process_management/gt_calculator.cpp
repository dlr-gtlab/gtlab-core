/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 28.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>
#include <QDir>

#include "gt_abstractrunnable.h"
#include "gt_objectlinkproperty.h"
#include "gt_modeproperty.h"
#include "gt_modetypeproperty.h"
#include "gt_calculatorexecutorlist.h"
#include "gt_abstractcalculatorexecutor.h"
#include "gt_labelproperty.h"
#include "gt_objectpathproperty.h"
#include "gt_modeproperty.h"

#include "gt_calculator.h"

struct GtCalculator::Impl
{
    explicit Impl(GtCalculator& pub)
      : execMode(QStringLiteral("execMode"), tr("Mode"), tr("Execution mode")),
        // execution label property
        labelProperty(QStringLiteral("execLabel"),
                      tr("Label"), tr("Execution label"), &pub),
        failRunOnWarning(QStringLiteral("failOnWarn"), tr("Fail Run on Warning"),
                         tr("Terminate process execution if calculator"
                            " throws a warning."), false)
    {}

    /// Execution mode indicator.
    GtModeProperty execMode;

    /// Execution label property
    GtLabelProperty labelProperty;

    /// Fail run on warning indicator.
    GtBoolProperty failRunOnWarning;
};

GtCalculator::~GtCalculator() = default;

bool
GtCalculator::exec()
{
    // clear old linked objects
    linkedObjects().clear();

    // revert temp directory
    setTempPath("");

    // check skipped indicator
    if (isSkipped())
    {
        setState(GtCalculator::SKIPPED);
        gtDebug() << objectName() << tr(" skipped");
        return true;
    }

    qDebug() << objectName() << "::exec()";
    // initialize pointer to runnable
    setRunnable(nullptr);

    // initialize calculator
    setState(GtCalculator::RUNNING);

    // set associated runnable
    setRunnable(findParent<GtAbstractRunnable*>());

    // check whether runnable was found
    if (!runnable())
    {
        setState(GtCalculator::FAILED);
        return false;
    }

    // collect all calculator properties
    QList<GtAbstractProperty*> props = fullPropertyList();

    // search for object link and object path properties
    foreach (GtAbstractProperty* prop, props)
    {
        if (GtObjectLinkProperty* objLink =
                    qobject_cast<GtObjectLinkProperty*>(prop))
        {
            // object link property found
            GtObject* linkedObj =
                runnable()->data<GtObject*>(objLink->linkedObjectUUID());

            if (linkedObj)
            {
                // linked object found -> store inside list
                linkedObjects().append(linkedObj);
            }
            else
            {
                qDebug() << "Linked object not found in runnable";
            }
        }
        else if (GtObjectPathProperty* objPath =
                     qobject_cast<GtObjectPathProperty*>(prop))
        {
            // object path property found
            GtObject* linkedObj =
                runnable()->data<GtObject*>(objPath->path());

            if (linkedObj)
            {
                // linked object found -> store inside list
                linkedObjects().append(linkedObj);
            }
            else
            {
                qDebug() << "Linked object not found in runnable";
            }
        }
    }

    // current execution mode identification string
    QString execMode = pimpl->execMode.get();

    if (execMode != "local")
    {
        // plugin execution
        // find executor
        GtAbstractCalculatorExecutor* executor =
            gtCalcExecList->executor(execMode);

        if (!executor)
        {
            // executor plugin not found
            gtError() << tr("Calculator execution plugin error!");
            setState(GtCalculator::FAILED);
            return false;
        }

        // run execution plugin
        if (!executor->exec(this))
        {
            // execution failed
            setState(GtCalculator::FAILED);
            return false;
        }
    }
    else
    {
        // local execution (default)
        if (!run())
        {
            // execution failed
            setState(GtCalculator::FAILED);
            return false;
        }
    }

    // handle temporary path cleanup
    if (!tempPath().isEmpty() && m_deleteTempPath)
    {
        if (runnable())
        {
            if (!runnable()->clearTempDir(tempPath()))
            {
                gtWarning() << tr("could not remove temp dir") << "! - "
                            << tempPath();
            }
        }
    }

    // finish successful run
    if (hasWarnings())
    {
        setState(GtCalculator::WARN_FINISHED);
    }
    else
    {
        setState(GtCalculator::FINISHED);
    }

    // trigger transfer of monitoring properties
    emit transferMonitoringProperties();

    // return success
    return true;
}

const QString&
GtCalculator::execMode()
{
    return pimpl->execMode.get();
}

void
GtCalculator::setExecMode(const QString& execMode)
{
    pimpl->execMode.setVal(execMode);
}

void
GtCalculator::setExecModeLocal()
{
    pimpl->execMode.setVal("local");
}

const QString&
GtCalculator::executionLabel()
{
    return pimpl->labelProperty.get();
}

void
GtCalculator::setExecutionLabel(const QString& label)
{
    pimpl->labelProperty.setVal(label);
}

bool
GtCalculator::runFailsOnWarning()
{
    return pimpl->failRunOnWarning;
}

bool
GtCalculator::run()
{
    // nothing to do here
    return true;
}

GtCalculator::GtCalculator():
    m_deleteTempPath(true),
    // execution mode property
    pimpl(std::make_unique<Impl>(*this))
{
    setRunnable(nullptr);
    setObjectName(QStringLiteral("Calculator"));



    registerProperty(pimpl->labelProperty, tr("Execution"));



    // local execution mode
    auto* localMode = new GtModeTypeProperty("local", tr("local"));
    localMode->setParent(this);
    pimpl->execMode.registerSubProperty(*localMode);


    // collect plugin execution modes
    foreach (const QString& str, gtCalcExecList->executorIds())
    {
        auto* pluginMode = new GtModeTypeProperty(str, str);
        pluginMode->setParent(this);


        // collect exec mode specific settings
        foreach (GtAbstractProperty* execSetting, gtCalcExecList->settings(str))
        {
            execSetting->setParent(this);
            pluginMode->registerSubProperty(*execSetting);
        }

        pimpl->execMode.registerSubProperty(*pluginMode);
    }

    registerProperty(pimpl->execMode, tr("Execution"));
    registerProperty(pimpl->failRunOnWarning, tr("Execution"));

    setFlag(GtObject::UserRenamable, true);
}

void
GtCalculator::hideLabelProperty(bool val)
{
    pimpl->labelProperty.hide(val);
}


