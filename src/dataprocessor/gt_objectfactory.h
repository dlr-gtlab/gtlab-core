/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 24.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOBJECTFACTORY_H
#define GTOBJECTFACTORY_H

#include <QObject>

#include "gt_datamodel_exports.h"

#include "gt_abstractobjectfactory.h"

#define gtObjectFactory (GtObjectFactory::instance())

/**
 * @brief The GtObjectFactory class
 */
class GT_DATAMODEL_EXPORT GtObjectFactory : public QObject,
        public GtAbstractObjectFactory
{
public:
    /**
     * @brief instance
     * @return
     */
    static GtObjectFactory* instance();

private:
    /**
     * @brief GtObjectFactory
     * @param parent
     */
    explicit GtObjectFactory(QObject* parent = nullptr);

};

#endif // GTOBJECTFACTORY_H
