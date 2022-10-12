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
#include "gt_environment.h"
#include "gt_objectpathproperty.h"

#include "gt_calculator.h"

struct GtCalculator::Impl
{
    explicit Impl(GtCalculator& q) :
        labelProperty(QStringLiteral("execLabel"),
                      tr("Label"), tr("Execution label"),
                      &q),
        failRunOnWarning(QStringLiteral("failOnWarn"),
                         tr("Fail Run on Warning"),
                         tr("Terminate process execution if calculator"
                         " throws a warning."), false)
    {}

    /// Execution mode indicator.
    GtModeProperty* execMode {nullptr};

    /// Execution label property
    GtLabelProperty labelProperty;

    /// Path to process/project specific temporary path.
    QString tempPath;

    /// Pointer to runnable associated to the calculator.
    GtAbstractRunnable* runnable {nullptr};

    /// Fail run on warning indicator.
    GtBoolProperty failRunOnWarning;
};

GtCalculator::~GtCalculator() = default;

bool
GtCalculator::exec()
{
    // clear old linked objects
    m_linkedObjects.clear();

    // revert temp directory
    pimpl->tempPath.clear();

    // check skipped indicator
    if (isSkipped())
    {
        setState(GtCalculator::SKIPPED);
        gtDebug() << objectName() << tr(" skipped");
        return true;
    }

    qDebug() << objectName() << "::exec()";
    // initialize pointer to runnable
    pimpl->runnable = nullptr;

    // initialize calculator
    setState(GtCalculator::RUNNING);

    // set associated runnable
    pimpl->runnable = findParent<GtAbstractRunnable*>();

    // check whether runnable was found
    if (!pimpl->runnable)
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
                pimpl->runnable->data<GtObject*>(objLink->linkedObjectUUID());

            if (linkedObj)
            {
                // linked object found -> store inside list
                m_linkedObjects.append(linkedObj);
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
                pimpl->runnable->data<GtObject*>(objPath->path());

            if (linkedObj)
            {
                // linked object found -> store inside list
                m_linkedObjects.append(linkedObj);
            }
            else
            {
                qDebug() << "Linked object not found in runnable";
            }
        }
    }

    // current execution mode identification string
    QString execMode = pimpl->execMode->get();
    qDebug() << "|-> exec mode: " << execMode;

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
    if (!pimpl->tempPath.isEmpty() && m_deleteTempPath)
    {
        if (pimpl->runnable)
        {
            if (!pimpl->runnable->clearTempDir(pimpl->tempPath))
            {
                gtWarning() << tr("could not remove temp dir") << "! - "
                            << pimpl->tempPath;
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
    return pimpl->execMode->get();
}

void
GtCalculator::setExecMode(const QString& execMode)
{
    pimpl->execMode->setVal(execMode);
}

void
GtCalculator::setExecModeLocal()
{
    pimpl->execMode->setVal("local");
}

const QList<QPointer<GtObject>>&
GtCalculator::linkedObjects()
{
    return m_linkedObjects;
}

QDir
GtCalculator::tempDir()
{
    if (!pimpl->runnable)
    {
        return QDir();
    }

    // check whether temporary dir was already set
    if (pimpl->tempPath.isEmpty())
    {
        // create new temporary directory
        pimpl->tempPath = pimpl->runnable->tempDir().absolutePath();
    }

    return QDir(pimpl->tempPath);
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
    pimpl(std::make_unique<GtCalculator::Impl>(*this))
{
    setObjectName(QStringLiteral("Calculator"));

    registerProperty(pimpl->labelProperty, tr("Execution"));

    // execution mode property
    pimpl->execMode = new GtModeProperty(QStringLiteral("execMode"),
                                    tr("Mode"),
                                    tr("Execution mode"));
    pimpl->execMode->setParent(this);

    // local execution mode
    GtModeTypeProperty* localMode = new GtModeTypeProperty("local",
            tr("local"));
    localMode->setParent(this);
    pimpl->execMode->registerSubProperty(*localMode);

    // collect plugin execution modes
    foreach (const QString& str, gtCalcExecList->executorIds())
    {
        GtModeTypeProperty* pluginMode = new GtModeTypeProperty(str, str);
        pluginMode->setParent(this);

        // collect exec mode specific settings
        foreach (GtAbstractProperty* execSetting, gtCalcExecList->settings(str))
        {
            execSetting->setParent(this);
            pluginMode->registerSubProperty(*execSetting);
        }

        pimpl->execMode->registerSubProperty(*pluginMode);
    }

    registerProperty(*pimpl->execMode, tr("Execution"));
    registerProperty(pimpl->failRunOnWarning, tr("Execution"));

    setFlag(GtObject::UserRenamable, true);
}

QString
GtCalculator::environmentVariable(const QString& var)
{
    QVariant variant = gtEnvironment->value(var);

    if (variant.isNull())
    {
        return QString();
    }

    return variant.toString();
}

void
GtCalculator::hideLabelProperty(bool val)
{
    pimpl->labelProperty.hide(val);
}

QString
GtCalculator::projectPath()
{
    if (!pimpl->runnable)
    {
        return QString();
    }

    return pimpl->runnable->projectPath();
}

