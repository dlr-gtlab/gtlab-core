/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 05.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSDATA_H
#define GTPROCESSDATA_H

#include "gt_core_exports.h"
#include "gt_objectgroup.h"

#include <memory>

class GtTask;
class GtTaskGroup;

/**
 * @brief The GtProcessData class
 */
class GT_CORE_EXPORT GtProcessData : public GtObjectGroup
{
    Q_OBJECT

public:
    /**
     * @brief GtProcessData
     */
    Q_INVOKABLE GtProcessData();

    /**
     * @brief Destructor.
     */
    ~GtProcessData();

    /**
     * @brief processList
     * @return
     */
    QList<GtTask*> processList();

    /**
     * @brief findProcess
     * @param val
     * @return
     */
    GtTask* findProcess(const QString& val);

    // Returns current task group
    GtTaskGroup* taskGroup();

    // Initializes process data for given project
    void init(const QString& projectPath);

    // Stores process data for given project
    void save(const QString& projectPath);

    QStringList userGroupIds() const;

    QStringList customGroupIds() const;

private:
    /// Private implementation
    class Impl;
    std::unique_ptr<Impl> m_pimpl;

};

#endif // GTPROCESSDATA_H
