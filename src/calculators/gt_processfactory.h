/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 28.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSFACTORY_H
#define GTPROCESSFACTORY_H

#include "gt_calculators_exports.h"

#include "gt_factorygroup.h"

#include <QObject>

#define gtProcessFactory (GtProcessFactory::instance())

/**
 * @brief The GtProcessFactory class
 */
class GT_CALCULATORS_EXPORT GtProcessFactory : public QObject,
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
    GtProcessFactory(QObject* parent = NULL);

};

#endif // GTPROCESSFACTORY_H
