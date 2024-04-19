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
    const auto errorString = [=](){
        return QObject::tr("Loading module '%1' failed:").arg(plugin->ident());
    };

    if (!GtModuleLoader::check(plugin))
    {
        return false;
    }

    GtProcessInterface* proi = dynamic_cast<GtProcessInterface*>(plugin);

    if (proi)
    {
        QList<GtCalculatorData> const calcDataList = proi->calculators();
        QList<GtTaskData> const taskDataList = proi->tasks();
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

    GtCalculatorExecInterface* cexecp =
            dynamic_cast<GtCalculatorExecInterface*>(plugin);

    // contains dynamic linked calculator execution classes
    if (cexecp)
    {
        // check for GtAbstractCalculatorExecutor class
    }

    GtNetworkInterface* neti = dynamic_cast<GtNetworkInterface*>(plugin);

    if (neti)
    {
        if (gtAccessManager->groupExists(neti->accessId()))
        {
            gtWarning() << errorString()
                        << QObject::tr("The network access Id '%1' is already "
                                       "defined!").arg(neti->accessId());
            return false;
        }

        QMetaObject meta = neti->accessConnection();

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

void
GtProcessModuleLoader::insert(GtModuleInterface* plugin)
{
    GtModuleLoader::insert(plugin);

    GtProcessInterface* proi = dynamic_cast<GtProcessInterface*>(plugin);

    if (proi)
    {
        QList<GtCalculatorData> const calcDataList = proi->calculators();
        QList<GtTaskData> const taskDataList = proi->tasks();

        for (GtCalculatorData const& calcData : calcDataList)
        {
            gtCalculatorFactory->registerCalculatorData(calcData);
            gtObjectFactory->registerClass(calcData->metaData());
            gtEnvironment->addEnvironmentVariables(
                        calcData->environmentVariables());
        }

        for (GtTaskData const& taskData : taskDataList)
        {
            // includes registration in objectFactory
            gtTaskFactory->registerTaskData(taskData);
        }
    }

    GtCalculatorExecInterface* cexecp =
            dynamic_cast<GtCalculatorExecInterface*>(plugin);

    // contains dynamic linked calculator execution classes
    if (cexecp)
    {
        QString execId = cexecp->executorId();

        if (gtCalcExecList->executorExists(execId))
        {
            gtError() << QObject::tr("Calculator executor '%1' already exsits!")
                         .arg(execId);
            return;
        }

        gtCalcExecList->addExecutor(cexecp);
    }

    GtNetworkInterface* neti = dynamic_cast<GtNetworkInterface*>(plugin);

    if (neti)
    {
        gtAccessManager->addAccessGroup(neti->accessId(),
                                        neti->accessConnection());
    }
}
