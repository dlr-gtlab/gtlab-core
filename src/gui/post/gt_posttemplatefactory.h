/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 20.01.2016
 *  Author: Maximilian Vieweg (AT-TW)
 *  Tel.: +49 2203 601 4799
 */

#ifndef GTPOSTTEMPLATEFACTORY_H
#define GTPOSTTEMPLATEFACTORY_H

#include "gt_abstractobjectfactory.h"
#include "qobject.h"

#define gtPostTemplateFactory (GtPostTemplateFactory::instance())

class GtPostTemplateFactory : public QObject, public GtAbstractObjectFactory
{
public:
    /**
     * @brief instance
     * @return
     */
    static GtPostTemplateFactory* instance();

    /**
     * @brief registerPlot
     * @param metaObject
     */
    void registerPlots(const QList<QMetaObject>& metaObjects);

    /**
     * @brief knownPlots
     * @return
     */
    const QStringList& knownPlots();

private:
    ///
    QStringList m_plotIds;

    /**
     * @brief GtPostTemplateFactory
     */
    explicit GtPostTemplateFactory(QObject* parent = nullptr);
};

#endif // GTPOSTTEMPLATEFACTORY_H
