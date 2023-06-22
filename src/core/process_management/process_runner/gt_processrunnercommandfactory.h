/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 17.10.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTPROCESSRUNNERCOMMANDFACTORY_H
#define GTPROCESSRUNNERCOMMANDFACTORY_H

#include "gt_core_exports.h"

#include "gt_factorygroup.h"

#include <QObject>

#define gtProcessRunnerCommandFactory \
    (GtProcessRunnerCommandFactory::instance())

/**
 * @brief The GtProcessCommandFactory class
 * Factory group for registering process runner command classes
 */
class GtProcessRunnerCommandFactory : public QObject, public GtFactoryGroup
{
public:

    /**
     * @brief instance
     * @return
     */
    static GtProcessRunnerCommandFactory& instance();

private:
    /**
     * @brief GtProcessCommandFactory
     */
    explicit GtProcessRunnerCommandFactory(QObject* parent = nullptr);
};

#endif // GTPROCESSRUNNERCOMMANDFACTORY_H
