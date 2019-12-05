/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 28.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>
#include <QTime>
#include <QCoreApplication>
#include <QDir>

#include "gt_abstractrunnable.h"
#include "gt_abstractproperty.h"
#include "gt_objectlinkproperty.h"
#include "gt_modeproperty.h"
#include "gt_modetypeproperty.h"
#include "gt_calculatorexecutorlist.h"
#include "gt_abstractcalculatorexecutor.h"
#include "gt_labelproperty.h"
#include "gt_boolproperty.h"
#include "gt_environment.h"
#include "gt_objectpathproperty.h"
#include "gt_objectpath.h"

#include "gt_calculator.h"

GtCalculator::~GtCalculator()
{
    if (m_labelProperty != Q_NULLPTR)
    {
        delete m_labelProperty;
    }
}

bool
GtCalculator::exec()
{
    // clear old linked objects
    m_linkedObjects.clear();

    // revert temp directory
    m_tempPath.clear();

    // check skipped indicator
    if (m_skipped)
    {
        setState(GtCalculator::SKIPPED);
        gtDebug() << objectName() << tr(" skipped");
        return true;
    }

    qDebug() << objectName() << "::exec()";
    // initialize pointer to runnable
    m_runnable = Q_NULLPTR;

    // initialize calculator
    setState(GtCalculator::RUNNING);

    // set associated runnable
    m_runnable = findParent<GtAbstractRunnable*>();

    // check whether runnable was found
    if (m_runnable == Q_NULLPTR)
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
                m_runnable->data<GtObject*>(objLink->linkedObjectUUID());

            if (linkedObj != Q_NULLPTR)
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
                m_runnable->data<GtObject*>(objPath->path());

            if (linkedObj != Q_NULLPTR)
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
    QString execMode = m_execMode->get();
    qDebug() << "|-> exec mode: " << execMode;

    if (execMode != "local")
    {
        // plugin execution
        // find executor
        GtAbstractCalculatorExecutor* executor =
            gtCalcExecList->executor(execMode);

        if (executor == Q_NULLPTR)
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
    if (!m_tempPath.isEmpty() && m_deleteTempPath)
    {
        if (m_runnable != Q_NULLPTR)
        {
            if (!m_runnable->clearTempDir(m_tempPath))
            {
                gtWarning() << tr("could not remove temp dir") << "! - "
                            << m_tempPath;
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
    return m_execMode->get();
}

void
GtCalculator::setExecMode(const QString& execMode)
{
    m_execMode->setVal(execMode);
}

void
GtCalculator::setExecModeLocal()
{
    m_execMode->setVal("local");
}

const QList<QPointer<GtObject>>&
GtCalculator::linkedObjects()
{
    return m_linkedObjects;
}

QDir
GtCalculator::tempDir()
{
    if (m_runnable == Q_NULLPTR)
    {
        return QDir();
    }

    // check whether temporary dir was already set
    if (m_tempPath.isEmpty())
    {
        // create new temporary directory
        m_tempPath = m_runnable->tempDir().absolutePath();
    }

    return QDir(m_tempPath);
}

const QString&
GtCalculator::executionLabel()
{
    return m_labelProperty->get();
}

void
GtCalculator::setExecutionLabel(const QString& label)
{
    m_labelProperty->setVal(label);
}

bool
GtCalculator::isSkipped()
{
    return m_skipped;
}

void
GtCalculator::setSkipped(bool val)
{
    m_skipped = val;
}

bool
GtCalculator::runFailsOnWarning()
{
    return m_failRunOnWarning;
}

void
GtCalculator::setState(GtProcessComponent::STATE state)
{
    if (m_skipped)
    {
        GtProcessComponent::setState(GtProcessComponent::SKIPPED);
    }
    else
    {
        GtProcessComponent::setState(state);
    }
}

bool
GtCalculator::run()
{
    // nothing to do here
    return true;
}

GtCalculator::GtCalculator():
    m_deleteTempPath(true),
    m_execMode(Q_NULLPTR),
    m_labelProperty(Q_NULLPTR),
    m_runnable(Q_NULLPTR),
    m_skipped(QStringLiteral("skip"), tr("Skip"),
              tr("Skip Calculator"), false),
    m_failRunOnWarning(QStringLiteral("failOnWarn"), tr("Fail Run on Warning"),
                       tr("Terminate process execution if calculator"
                          " throws a warning."), false)
{
    setObjectName(QStringLiteral("Calculator"));

    // execution label property
    m_labelProperty = new GtLabelProperty(QStringLiteral("execLabel"),
                                          tr("Label"), tr("Execution label"),
                                          this);
    registerProperty(*m_labelProperty, tr("Execution"));

    // execution mode property
    m_execMode = new GtModeProperty(QStringLiteral("execMode"),
                                    tr("Mode"),
                                    tr("Execution mode"));
    m_execMode->setParent(this);

    // local execution mode
    GtModeTypeProperty* localMode = new GtModeTypeProperty("local",
            tr("local"));
    localMode->setParent(this);
    m_execMode->registerSubProperty(*localMode);

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

        m_execMode->registerSubProperty(*pluginMode);
    }

    registerProperty(*m_execMode, tr("Execution"));
    registerProperty(m_skipped, tr("Execution"));
    registerProperty(m_failRunOnWarning, tr("Execution"));

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
    m_labelProperty->hide(val);
}

QString
GtCalculator::projectPath()
{
    if (m_runnable == Q_NULLPTR)
    {
        return QString();
    }

    return m_runnable->projectPath();
}

QString
GtCalculator::dataHelper(GtObjectLinkProperty& prop)
{
    return prop.linkedObjectUUID();
}

GtObjectPath
GtCalculator::pathHelper(GtObjectPathProperty& prop)
{
    return prop.path();
}
