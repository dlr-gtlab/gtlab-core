/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTPROJECTIO_H
#define GTPROJECTIO_H

#include "gt_core_exports.h"

#include <QString>
#include <QDomDocument>
#include <QDir>

/**
 * @brief Project file I/O utilities
 *
 * Provides static functions for reading and writing project files.
 * All functions are cross-platform and use Qt APIs.
 */
class GT_CORE_EXPORT GtProjectIO
{
public:
    GtProjectIO() = delete;
    ~GtProjectIO() = delete;

    /**
     * @brief Reads the project data from the given path
     * @param projectPath Path to the project directory
     * @return The QDomDocument containing the project data, or a null document on error
     */
    static QDomDocument readProjectData(const QDir& projectPath);

    /**
     * @brief Saves the project data to the given file path
     * @param filePath The full path to the project file
     * @param doc The QDomDocument to save
     * @return true if successful, false otherwise
     */
    static bool saveProjectFiles(const QString& filePath,
                                 const QDomDocument& doc,
                                 bool saveWithLinkedFiles = true);

    /**
     * @brief Checks if a file exists at the given path
     * @param path The file path to check
     * @return true if the file exists, false otherwise
     */
    static bool fileExists(const QString& path);
};

#endif // GTPROJECTIO_H
