/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 28.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSFACTORY_H
#define GTPROCESSFACTORY_H

#include "gt_core_exports.h"

#include "gt_factorygroup.h"

#include <QObject>

#define gtProcessFactory (GtProcessFactory::instance())

/**
 * @brief The GtProcessFactory class
 */
class GT_CORE_EXPORT GtProcessFactory : public QObject,
        public GtFactoryGroup
{
public:
    /**
     * @brief instance
     * @return
     */
    static GtProcessFactory* instance();

    /**
     * @brief taskFactory
     * @return
     */
    GtAbstractObjectFactory* taskFactory();

    /**
     * @brief calculatorFactory
     * @return
     */
    GtAbstractObjectFactory* calculatorFactory();

private:
    /**
     * @brief GtProcessFactory
     */
    explicit GtProcessFactory(QObject* parent = nullptr);

};

#endif // GTPROCESSFACTORY_H
