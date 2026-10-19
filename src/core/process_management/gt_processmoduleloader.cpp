/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 11.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_processmoduleloader.h"
#include "gt_processinterface.h"
#include "gt_operationinterface.h"
#include "gt_executableoperation.h"
#include "gt_processfactory.h"
#include "gt_calculatorfactory.h"
#include "gt_taskfactory.h"
#include "gt_moduleinterface.h"
#include "gt_calculatorexecinterface.h"
#include "gt_calculatorexecutorlist.h"
#include "gt_calculatordata.h"
#include "gt_taskdata.h"
#include "gt_objectfactory.h"
#include "gt_accessmanager.h"
#include "gt_networkinterface.h"
#include "gt_abstractaccessdataconnection.h"
#include "gt_environment.h"
#include "gt_logging.h"
#include "gt_utilities.h"
#include "gt_qtutilities.h"

GtProcessModuleLoader::GtProcessModuleLoader()
{
}

bool
GtProcessModuleLoader::check(GtModuleInterface* plugin) const
{

    if (!GtModuleLoader::check(plugin))
    {
        return false;
    }

    const auto errorString = [=](){
        return QObject::tr("Loading module '%1' failed:").arg(plugin->ident());
    };

    GtProcessInterface* processInterface =
        checkInterface<GtProcessInterface>(plugin->ident(), plugin);

    if (processInterface)
    {
        QList<GtCalculatorData> const calcDataList =
            processInterface->calculators();
        QList<GtTaskData> const taskDataList = processInterface->tasks();
        QList<QMetaObject> metaList;

        for (GtCalculatorData const& calcData : calcDataList)
        {
            if (!calcData->isValid())
            {
                gtWarning() << errorString()
                            << QObject::tr("Invalid calculator data:")
                            << gt::squoted(calcData->id);
                return false;
            }

            metaList << calcData->metaData();
        }

        for (GtTaskData const& taskData : taskDataList)
        {
            if (!taskData->isValid())
            {
                gtWarning() << errorString()
                            << QObject::tr("Invalid task data:")
                            << gt::squoted(taskData->id);
                return false;
            }

            metaList << taskData->metaData();
        }

        if (gtProcessFactory->containsDuplicates(metaList))
        {
            gtWarning() << errorString()
                        << QObject::tr("A process element is already defined!");
            return false;
        }
    }

    const GtOperationInterface* operationInterface =
        checkInterface<GtOperationInterface>(plugin->ident(), plugin);

    if (operationInterface)
    {
        const QList<QMetaObject> operations = operationInterface->operations();

        if (gtObjectFactory->containsDuplicates(operations))
        {
            gtWarning() << errorString()
                        << QObject::tr("An operation is already defined!");
            return false;
        }

        QSet<QString> declaredClassNames;
        for (QMetaObject const& operation : operations)
        {
            const QString className = QString::fromLatin1(operation.className());
            if (declaredClassNames.contains(className))
            {
                gtWarning() << errorString()
                            << QObject::tr("An operation is declared more than once:")
                            << gt::squoted(className);
                return false;
            }
            declaredClassNames.insert(className);

            std::unique_ptr<QObject> object(operation.newInstance());
            if (!qobject_cast<GtExecutableOperation*>(object.get()))
            {
                gtWarning() << errorString()
                            << QObject::tr("Invalid executable operation class:")
                            << gt::squoted(className);
                return false;
            }
        }
    }

    const GtCalculatorExecInterface* calculatorExecutorInterface =
        checkInterface<GtCalculatorExecInterface>(plugin->ident(), plugin);

    // contains dynamic linked calculator execution classes
    if (calculatorExecutorInterface)
    {
        // check for GtAbstractCalculatorExecutor class
    }

    GtNetworkInterface* networkInterface =
        checkInterface<GtNetworkInterface>(plugin->ident(), plugin);

    if (networkInterface)
    {
        if (gtAccessManager->groupExists(networkInterface->accessId()))
        {
            gtWarning() << errorString()
                        << QObject::tr("The network access Id '%1' is already "
                                       "defined!")
                               .arg(networkInterface->accessId());
            return false;
        }

        QMetaObject meta = networkInterface->accessConnection();

        std::unique_ptr<QObject> obj(meta.newInstance());

        if (!gt::unique_qobject_cast<GtAbstractAccessDataConnection>(std::move(obj)))
        {
            gtWarning() << errorString()
                        << QObject::tr("Invalid network access connection data class!");
            return false;
        }
    }

    return true;
}

bool
GtProcessModuleLoader::insertAdditional(GtModuleInterface* plugin)
{
    // Reject executor conflicts before registering any process interfaces.
    GtCalculatorExecInterface* calculatorExecutorInterface =
        checkInterface<GtCalculatorExecInterface>(plugin->ident(), plugin);
    if (calculatorExecutorInterface &&
        gtCalcExecList->executorExists(
            calculatorExecutorInterface->executorId()))
    {
        gtError() << QObject::tr("Calculator executor '%1' already exists!")
                         .arg(calculatorExecutorInterface->executorId());
        return false;
    }

    // Register calculators and tasks.
    GtProcessInterface* processInterface =
        checkInterface<GtProcessInterface>(plugin->ident(), plugin);

    if (processInterface)
    {
        QList<GtCalculatorData> const calcDataList =
            processInterface->calculators();
        QList<GtTaskData> const taskDataList = processInterface->tasks();

        for (GtCalculatorData const& calcData : calcDataList)
        {
            gtCalculatorFactory->registerCalculatorData(calcData,
                                                        plugin->ident());
            gtObjectFactory->registerClass(calcData->metaData(),
                                           plugin->ident());
            gtEnvironment->addEnvironmentVariables(
                        calcData->environmentVariables());
        }

        for (GtTaskData const& taskData : taskDataList)
        {
            // includes registration in objectFactory
            gtTaskFactory->registerTaskData(taskData, plugin->ident());
        }
    }

    // Register executable operations.
    const GtOperationInterface* operationInterface =
        checkInterface<GtOperationInterface>(plugin->ident(), plugin);

    if (operationInterface)
    {
        gtObjectFactory->registerClasses(operationInterface->operations(),
                                         plugin->ident());
    }

    // Register the calculator executor.
    if (calculatorExecutorInterface)
    {
        gtCalcExecList->addExecutor(calculatorExecutorInterface);
    }

    // Register the network access group.
    GtNetworkInterface* networkInterface =
        checkInterface<GtNetworkInterface>(plugin->ident(), plugin);

    if (networkInterface)
    {
        gtAccessManager->addAccessGroup(networkInterface->accessId(),
                                        networkInterface->accessConnection());
    }
    return true;
}
