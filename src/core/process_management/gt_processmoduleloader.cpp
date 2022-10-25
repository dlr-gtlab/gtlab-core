/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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

    GtProcessInterface* proi = dynamic_cast<GtProcessInterface*>(plugin);

    if (proi)
    {
        QList<GtCalculatorData> calcDataList = proi->calculators();
        QList<GtTaskData> taskDataList = proi->tasks();
        QList<QMetaObject> metaList;

        foreach (GtCalculatorData calcData, calcDataList)
        {
            if (!calcData->isValid())
            {
                return false;
            }

            metaList << calcData->metaData();
        }

        foreach (GtTaskData taskData, taskDataList)
        {
            if (!taskData->isValid())
            {
                return false;
            }

            metaList << taskData->metaData();
        }

        if (gtProcessFactory->containsDuplicates(metaList))
        {
            return false;
        }

        if (gtObjectFactory->containsDuplicates(metaList))
        {
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
            return false;
        }

        QMetaObject meta = neti->accessConnection();

        QObject* obj = meta.newInstance();

        if (!obj)
        {
            return false;
        }

        if (qobject_cast<GtAbstractAccessDataConnection*>(obj))
        {
            delete obj;
            return true;
        }

        delete obj;
        return false;
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
        QList<GtCalculatorData> calcDataList = proi->calculators();
        QList<GtTaskData> taskDataList = proi->tasks();

        foreach (GtCalculatorData calcData, calcDataList)
        {
            gtCalculatorFactory->registerCalculatorData(calcData);
            gtObjectFactory->registerClass(calcData->metaData());
            gtEnvironment->addEnvironmentVariables(
                        calcData->environmentVariables());
        }

        foreach (GtTaskData taskData, taskDataList)
        {
            gtTaskFactory->registerTaskData(taskData);
            gtObjectFactory->registerClass(taskData->metaData());
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
            gtError() << QObject::tr("Calculator executor already exsits!");
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
