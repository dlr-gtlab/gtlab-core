/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 17.10.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_processrunnercommandfactory.h"

#include "gt_processfactory.h"
#include "gt_processrunnerresponse.h"
#include "gt_processrunnercommand.h"
#include "gt_processrunnernotification.h"
#include "gt_processcomponent.h"

/**
 * @brief The GtProcessCommandHelperFactory struct
 * Helper factory, as a factory group cannot register any class.
 */
struct GtProcessRunnerCommandHelperFactory :
        public QObject,
        public GtAbstractObjectFactory
{
    GtProcessRunnerCommandHelperFactory(QObject* parent = nullptr) :
        QObject(parent)
    {
        setObjectName("GtProcessCommandHelperFactory");

        // commands, notifications and responses
        registerClass(GT_METADATA(GtProcessRunnerNotification),
                      builtinModuleId());
        registerClass(GT_METADATA(GtProcessRunnerResponse), builtinModuleId());
        registerClass(GT_METADATA(GtProcessRunnerInitSessionCommand),
                      builtinModuleId());
        registerClass(GT_METADATA(GtProcessRunnerRunTaskCommand),
                      builtinModuleId());
        registerClass(GT_METADATA(GtProcessRunnerCollectTaskCommand),
                      builtinModuleId());
        registerClass(GT_METADATA(GtProcessRunnerPingCommand),
                      builtinModuleId());
        registerClass(GT_METADATA(GtProcessRunnerAbortCommand),
                      builtinModuleId());

        // meta data
        registerClass(
            GT_METADATA(gt::process_runner::ProcessComponentStatePayload),
            builtinModuleId());
        registerClass(GT_METADATA(gt::process_runner::MementoDiffPayload),
                      builtinModuleId());
    }
};

GtProcessRunnerCommandFactory::GtProcessRunnerCommandFactory(QObject* parent) :
    QObject(parent)
{
    setObjectName("GtProcessCommandFactory");

    auto* factory = new GtProcessRunnerCommandHelperFactory(this);

    addFactory(factory);
    addFactory(gtProcessFactory);
}

GtProcessRunnerCommandFactory& GtProcessRunnerCommandFactory::instance()
{
    static GtProcessRunnerCommandFactory self;
    return self;
}
