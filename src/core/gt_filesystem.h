/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#ifndef IOUTILS_H
#define IOUTILS_H

#include <QDir>
#include <QRegularExpression>

#include "gt_core_exports.h"

namespace gt
{

    namespace filesystem
    {

        enum class CopyStatus
        {
            Success = 0,
            SrcNotExists,
            TargetDirAlreadyExists,
            CannotWriteTarget,
            Error
        };

        enum CopyFlags
        {
            OverwriteFiles = 1,
            Recursive = 2
        };


        /**
         * @brief Copies all files (and directories) of sourceDir into the destDir
         *
         * If destDir not exist, it will be created.
         * If destDir exists, the files will be merged with the existing files
         *
         * The following flags can be set:
         *  - OverwriteFiles -> Overwrite existing files, otherwise they will be skipped
         *  - Recursive -> Also copy subdirectories and their content
         *
         * @param sourceDir Source directory
         * @param destDir   Destination directory
         * @param copyFlags Flags for the copy operation
         * @param reFilter Copies only files that match this filter.
         *        Keep empty to match all files
         * @return
         */
        GT_CORE_EXPORT
        CopyStatus copyDir(
            const QDir& sourceDir, const QDir& destDir,
            int copyFlags = OverwriteFiles | Recursive,
            const QRegularExpression& reFilter = {});


        /**
         * @brief Returns a list of relative filenames that exist in dir
         * @param dir The directory to search
         * @param recursive If true, search also in subdirs
         * @param reFilter Regex to filter files to be returned
         * @return
         */
        GT_CORE_EXPORT
        QStringList directoryEntries(
            const QDir& dir, bool recursive,
            const QRegularExpression& reFilter = {});
    } // namespace filesystem

} // namespace gt

#endif // IOUTILS_H
