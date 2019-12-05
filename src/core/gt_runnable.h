/* GTlab - Gas Turbine laboratory
 * Source File: gt_runnable.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 22.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#ifndef GTRUNNABLE_H
#define GTRUNNABLE_H

#include "gt_abstractrunnable.h"

#include "gt_processcomponent.h"

/**
 * @brief The GtRunnable class
 */
class GtRunnable : public GtAbstractRunnable
{
    Q_OBJECT

public:
    /**
     * @brief GtRunnable
     */
    GtRunnable();

    /**
     * @brief ~GtRunnable
     */
    virtual ~GtRunnable();

    /**
     * @brief run
     */
    void run() Q_DECL_OVERRIDE;

    /**
     * @brief appendCalculator
     * @param calc
     * @return
     */
    bool appendCalculator(GtProcessComponent* calc);

    /**
     * @brief Sets the execution path for the execution process.
     * @param path
     */
    void setExecutionPath(const QString& path);

    /**
     * @brief Requests the termination of current running task.
     */
    void requestInterruption();

    /**
     * @brief Returns process specific temporary directory.
     * @return
     */
    QDir tempDir() Q_DECL_OVERRIDE;

    /**
     * @brief Returns path of current project.
     * @return Path to current project.
     */
    QString projectPath() Q_DECL_OVERRIDE;

    /**
     * @brief clearTempDir
     * @param path
     * @return
     */
    bool clearTempDir(const QString& path) Q_DECL_OVERRIDE;

private:
    /// Execution path
    QString m_executionPath;

    /// Uuid of created temporary path
    QString m_tempDirId;

    /**
     * @brief transferObjects
     */
    void readObjects();

    /**
     * @brief writeObjects
     */
    void writeObjects();

};

#endif // GTRUNNABLE_H
