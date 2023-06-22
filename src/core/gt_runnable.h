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
     * @brief Constrcutor. Sets a custom project path for the execution process
     * @param projectPath Project path
     */
    explicit GtRunnable(QString projectPath = {});

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
     * @brief Returns path of current project.
     * @return Path to current project.
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
