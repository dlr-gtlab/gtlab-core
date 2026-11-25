/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GT_PROJECT_TEST_HELPER_H
#define GT_PROJECT_TEST_HELPER_H

#include <optional>

#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QString>

#include "gt_coreapplication.h"
#include "gt_project.h"

namespace gtprojecttest
{

struct GtProjectHelperConfig
{
    // Use the running framework version by default; an empty value makes a
    // legacy project without a core-ver element.
    std::optional<QString> coreVersion;
    // Nullopt omits the comment element; the default writes an empty one.
    std::optional<QString> comment = QString{};
    // Optional XML fragment inserted after the module list.
    std::optional<QString> additionalMetadata;
};

inline bool
writeProjectFile(const QString& dirPath, const QString& projectName,
                 const GtProjectHelperConfig& config = {})
{
    QFile file(dirPath + QDir::separator() + GtProject::mainFilename());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate |
                   QIODevice::Text))
    {
        return false;
    }

    const QString coreVersion =
        config.coreVersion.value_or(GtCoreApplication::version().toString());
    const QString fileVersion =
        coreVersion.isEmpty() ? QStringLiteral("1.7.0-rc1") : coreVersion;
    const QString coreVerElement =
        coreVersion.isEmpty()
            ? QString()
            : QStringLiteral("        <core-ver>%1</core-ver>\n")
                  .arg(coreVersion);
    const QString commentElement =
        config.comment
            ? QStringLiteral("    <comment>%1</comment>\n").arg(*config.comment)
            : QString();
    const QString metadataElement =
        config.additionalMetadata
            ? QStringLiteral("    %1\n").arg(*config.additionalMetadata)
            : QString();

    const QByteArray data =
        QString("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                "<GTLAB projectname=\"%1\" version=\"%2\">\n"
                "    <env-footprint>\n"
                "%3"
                "        <modules/>\n"
                "    </env-footprint>\n"
                "%4"
                "    <MODULES/>\n"
                "%5"
                "    <PROCESSES/>\n"
                "    <LABELS/>\n"
                "</GTLAB>\n")
            .arg(projectName, fileVersion, coreVerElement, commentElement,
                 metadataElement)
            .toUtf8();

    return file.write(data) == data.size();
}

} // namespace gtprojecttest

#endif // GT_PROJECT_TEST_HELPER_H
