#include "gt_filesystem.h"

#include <QDir>

#include <QRegularExpression>

namespace
{

void
dirEntriesImpl(const QDir& dir, const QString& prefix, bool recursive,  QStringList& result)
{
    if (!dir.exists())
    {
        return;
    }

    auto files = dir.entryList(QDir::Files);
    std::transform(files.begin(), files.end(), files.begin(),
                   [&prefix](const QString& fname) {
        return prefix + fname;
    });

    result.append(std::move(files));

    if (recursive)
    {
        foreach (const QString& directoryName, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
        {
            QDir subDir(dir.path() + "/" + directoryName);
            dirEntriesImpl(subDir, prefix + directoryName + "/", true, result);
        }
    }

    return;
}

} // namespace

QStringList
gt::filesystem::directoryEntries(const QDir& dir, bool recursive,
                                 const QRegularExpression& expr)
{
    QStringList files;
    dirEntriesImpl(dir, "", recursive, files);

    // filter according to expr
    QStringList filesFiltered;
    std::copy_if(files.begin(), files.end(), std::back_inserter(filesFiltered),
        [&expr](const QString& fname) {
            return expr.match(fname).hasMatch();
        });

    return filesFiltered;
}


gt::filesystem::CopyStatus
gt::filesystem::copyDir(const QDir& originDirectory, const QDir& destinationDirectory,
                 int flags, const QRegularExpression& reFilter)
{

    if (!originDirectory.exists())
    {
        return CopyStatus::SrcNotExists;
    }

    if (!destinationDirectory.exists())
    {
        destinationDirectory.mkpath(".");
    }

    auto files = directoryEntries(originDirectory, flags & Recursive, reFilter);

    foreach (QString fileName, files)
    {
        QString targetFileName = destinationDirectory.path() + "/" + fileName;

        QFileInfo targetFileInfo(targetFileName);

        if (!targetFileInfo.exists() || flags & OverwriteFiles)
        {
            auto subDir = targetFileInfo.dir();
            if (!subDir.exists()) subDir.mkdir(".");

            QFile::copy(originDirectory.path() + "/" + fileName,
                        targetFileName);
        }
    }

    /*! Possible race-condition mitigation? */
    QDir finalDestination(destinationDirectory);
    finalDestination.refresh();

    if (finalDestination.exists())
    {
        return CopyStatus::Success;
    }

    return CopyStatus::Error;
}
