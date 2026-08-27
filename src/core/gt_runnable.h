/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_runnable.h
 *
 *  Created on: 22.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#ifndef GTRUNNABLE_H
#define GTRUNNABLE_H

#include "gt_core_exports.h"
#include "gt_abstractrunnable.h"
#include "gt_executioncontext.h"

#include "gt_processcomponent.h"

/**
 * @brief The GtRunnable class
 */
class GT_CORE_EXPORT GtRunnable : public GtAbstractRunnable
{
    Q_OBJECT

public:
    /**
     * @brief Constructor. Sets a custom project path for the execution process
     * @param projectPath Project path
     */
    explicit GtRunnable(QString projectPath = {});

    /**
     * @brief Constructs a runnable with execution-specific context data.
     * @param projectPath Legacy custom project path
     * @param executionContext Borrowed project/path context installed only
     *        during run(). The referenced project must outlive run() and must
     *        not be retained for asynchronous work.
     */
    GtRunnable(QString projectPath, GtExecutionContext executionContext);

    /**
     * @brief run
     */
    void run() override;

    /**
     * @brief Requests the termination of current running task.
     */
    void requestInterruption();

    /**
     * @brief Returns process specific temporary directory.
     * @return
     */
    QDir tempDir() override;

    /**
     * @brief Returns the execution-context project path, if present.
     *
     * Resolution is context first, then the legacy custom path, then the
     * selected application project.
     */
    QString projectPath() override;

    /**
     * @brief clearTempDir
     * @param path
     * @return
     */
    bool clearTempDir(const QString& path) override;

private:

    /// custom project path (by default empty)
    QString m_projectPath;

    /// Context associated with this execution.
    GtExecutionContext m_executionContext;

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
