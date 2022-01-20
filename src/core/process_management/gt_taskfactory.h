/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 12.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTASKFACTORY_H
#define GTTASKFACTORY_H

#include "gt_core_exports.h"

#include <QObject>

#include "gt_abstractobjectfactory.h"
#include "gt_taskdata.h"

#define gtTaskFactory (GtTaskFactory::instance())

/**
 * @brief The GtTaskFactory class
 */
class GT_CORE_EXPORT GtTaskFactory : public QObject,
        public GtAbstractObjectFactory
{
public:
    /**
     * @brief Returns instance of task factory based on singleton pattern.
     * @return Instance of task factory
     */
    static GtTaskFactory* instance();

    /**
     * @brief Registers task data to factory.
     * @param taskData New task data.
     * @return Returns true if task data was successfully added.
     */
    virtual bool registerTaskData(GtTaskData taskData);

    /**
     * @brief Returns full list of registered task data.
     * @return List of task data.
     */
    QList<GtTaskData> taskDataList() const;

    /**
     * @brief Returns specific task data based on given classname.
     * Returns invalid data if no matching data could be found.
     * @param className Classname of requested task data.
     * @return Task data.
     */
    GtTaskData taskData(const QString& className);

    /**
     * @brief Checks if task data exists for given class name.
     * @param className Task class name.
     * @return True if task data for given class name exists.
     */
    bool taskDataExists(const QString& className);

private:
    /// Task data
    QHash<QString, GtTaskData> m_taskData;

    /**
     * @brief Constructor.
     * @param parent Parent object.
     */
    GtTaskFactory(QObject* parent = NULL);

};

#endif // GTTASKFACTORY_H
